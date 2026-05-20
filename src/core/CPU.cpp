#include "CPU.h"
#include "Bus.h"
#include "Timer.h"
#include "PPU.h"
#include "APU.h"
#include "Joypad.h"

#ifdef GAMEBOY_DOCTOR
// Code for testing via GB doctor
// LOG BEFORE EXECUTION (Standard requirement)
// Format: A:01 F:B0 B:00 C:13 D:00 E:D8 H:01 L:4D SP:FFFE PC:0100 PCMEM:00,C3,13,02

// Fetch the 4 bytes at PC for the log (without incrementing PC!)
uint8_t pcmem[4];
for (int i = 0; i < 4; i++)
{
    pcmem[i] = bus->Read(reg.pc + i);
}

char logBuffer[100];
sprintf(logBuffer, "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X",
    reg.a, reg.f, reg.b, reg.c, reg.d, reg.e, reg.h, reg.l, reg.sp, reg.pc,
    pcmem[0], pcmem[1], pcmem[2], pcmem[3]
);
std::cout << logBuffer << "\n";
#endif

// CPU Timing strategy
// Every bus R/W, and by extension, the fetch, pop, push, calls call Clock() before the actual bus access.
// This is to keep timer in sync with the CPU.
// As for the opcodes which have additional cycles consumed that are unrelated to bus access, these have to call Clock() within the opcodes.
// The return value for Tick is still the total T-cycle count for the instruction. The purpose of the Clock() is again, just to keep Timer synced with CPU.
// CPU ticks timer. Timer is not ticked in the main loop.



void CPU::StartTracing(const std::string & filename)
{
    traceFile.open(filename, std::ios::binary | std::ios::out);
    if (traceFile.is_open())
    {
        isTracing = true;
        traceBuffer.reserve(TRACE_BUFFER_FLUSH_THRESHOLD);
    }
}

void CPU::StopTracing()
{
    if (isTracing)
    {
        FlushTraceBuffer();
        traceFile.close();
        isTracing = false;
        std::cout << "Saved log file\n";
    }
}

void CPU::FlushTraceBuffer()
{
    if (!traceBuffer.empty() && traceFile.is_open())
    {
        std::string batchOutput;
        batchOutput.reserve(traceBuffer.size() * 80);

        char lineBuffer[128];
        for (const auto& entry : traceBuffer)
        {
            std::snprintf(lineBuffer, sizeof(lineBuffer),
                "PC: %04X | OP: %02X | IME: %d | IE: %02X | IF: %02X | DIV: %04X | TIMA: %02X | TMA: %02X | TAC: %02X\n",
                entry.pc, entry.opcode, entry.ime ? 1 : 0,
                entry.ie, entry.if_, entry.div, entry.tima, entry.tma, entry.tac
            );
            batchOutput.append(lineBuffer);
        }

        traceFile.write(batchOutput.data(), batchOutput.size());
        traceBuffer.clear();
    }
}

void CPU::LogInstructionTrace(uint16_t currentPc, uint8_t op)
{
    if (!isTracing) return;

    TraceEntry entry;
    entry.pc = currentPc;
    entry.opcode = op;
    entry.ime = ime;

    // We use a direct array access if available, or a side-effect-free peek 
    // to avoid triggering extra M-cycles during a trace check.
    entry.ie = interruptFlagEnabled;
    entry.if_ = interruptFlag;

    entry.div = timer->GetDiv();
    entry.tima = timer->GetTima();
    entry.tma = timer->GetTma();
    entry.tac = timer->GetTac();

    traceBuffer.push_back(entry);

    if (traceBuffer.size() >= TRACE_BUFFER_FLUSH_THRESHOLD)
    {
        FlushTraceBuffer();
    }
}






CPU::CPU()
{
    RegisterInstructions();
}

void CPU::ResetRegisters()
{
    reg.a = 0x01;
    reg.f = 0xB0;
    //reg.SetZ(1); reg.SetN(0); reg.SetH(false); reg.SetC(false); // TODO: H and C dependant on header checksum
    reg.b = 0x00;
    reg.c = 0x13;
    reg.d = 0x00;
    reg.e = 0xD8;
    reg.h = 0x01;
    reg.l = 0x4D;
    reg.pc = 0x0100;
    reg.sp = 0xFFFE;

    ime = false;
    imeNext = false;
    halted = false;
    stopped = false;
    interruptFlag = 0xE1;
    interruptFlagEnabled = 0x00;
}

int CPU::Tick()
{
    if(isTracing) LogInstructionTrace(reg.pc, bus->Read(reg.pc));

    bool applyIME = imeNext;

    if (stopped)
    {
        if (interruptFlag & 0x10)
        {
            stopped = false;
            interruptFlag &= ~0x10;
        }
        else
        {
            Clock();
            return 4;
        }
    }

    if (halted)
    {
        uint8_t pending = interruptFlagEnabled & interruptFlag & 0x1F;
        if (!pending)
        {
            Clock();
            return 4;
        }

        if (applyIME && imeNext) { ime = true; imeNext = false; }
        halted = false;
        if (ime)
        {
            HandleInterrupts();
            return 20;
        }
    }

    totalCyclesForInstruction = 0;
    if (ime)
    {
        HandleInterrupts();
        if (totalCyclesForInstruction > 0) return totalCyclesForInstruction;
    }

    if (haltBug) { --reg.pc; haltBug = false; }

    uint8_t opcode = FetchByte();
    (this->*instructions[opcode].execute)();
    totalCyclesForInstruction += instructions[opcode].cycles;
    
    if (applyIME && imeNext)
    {
        ime = true;
        imeNext = false;
    }

    return totalCyclesForInstruction;

}

int CPU::HandleInterrupts()
{
    uint8_t interrupts = interruptFlag & interruptFlagEnabled;
    if (!interrupts)
    {
        return 0;
    }

    ime = false;
    imeNext = false;

    int interruptBitToHandle = -1;

    Clock();
    for (int i = 0; i < 5; ++i)
    {
        if (interrupts & (1 << i))
        {
            interruptBitToHandle = i;
            break;
        }
    }
    if (interruptBitToHandle != -1)
    {
        interruptFlag &= ~(1 << interruptBitToHandle);
    }
    Clock();
    
    PushWord(reg.pc);
    Clock();

    switch (interruptBitToHandle)
    {
    case 0: reg.pc = 0x40; break; // VBlank
    case 1: reg.pc = 0x48; break; // STAT
    case 2: reg.pc = 0x50; break; // Timer
    case 3: reg.pc = 0x58; break; // Serial
    case 4: reg.pc = 0x60; break; // Joypad
    default:
        ASSERT(false, "Interrupt Code invalid: %i", interruptBitToHandle);
    }

    totalCyclesForInstruction += 20;
    return 20;
}

void CPU::RequestInterrupt(InterruptCode bit)
{
    ASSERT(static_cast<int>(bit) <= 4, "Interrupt bit Out of Range");
    interruptFlag |= (1 << static_cast<int>(bit));
}

void CPU::Clock()
{
    // tick 4 on other components
    timer->Tick(4);
}



void CPU::SaveState(std::ofstream& out)
{
    // Registers
    GBWrite(out, reg.a); GBWrite(out, reg.f);
    GBWrite(out, reg.b); GBWrite(out, reg.c);
    GBWrite(out, reg.d); GBWrite(out, reg.e);
    GBWrite(out, reg.h); GBWrite(out, reg.l);
    GBWrite(out, reg.sp);
    GBWrite(out, reg.pc);

    // Interrupts
    GBWrite(out, ime);
    GBWrite(out, imeNext);
    GBWrite(out, halted);
    GBWrite(out, stopped);

    GBWrite(out, interruptFlag);
    GBWrite(out, interruptFlagEnabled);
}

void CPU::LoadState(std::ifstream& in)
{
    GBRead(in, reg.a); GBRead(in, reg.f);
    GBRead(in, reg.b); GBRead(in, reg.c);
    GBRead(in, reg.d); GBRead(in, reg.e);
    GBRead(in, reg.h); GBRead(in, reg.l);
    GBRead(in, reg.sp);
    GBRead(in, reg.pc);

    GBRead(in, ime);
    GBRead(in, imeNext);
    GBRead(in, halted);
    GBRead(in, stopped);

    GBRead(in, interruptFlag);
    GBRead(in, interruptFlagEnabled);
}




// Instrucitons should interface with these functions instead of doing raw Bus Reads.
uint8_t CPU::ReadByte(Address addr)
{
    Clock();
    return bus->Read(addr);
}

uint16_t CPU::ReadWord(Address addr)
{
    uint8_t lo = ReadByte(addr);
    uint8_t hi = ReadByte(addr + 1);
    
    return (hi << 8) | lo;
}

void CPU::WriteByte(Address addr, uint8_t value)
{
    Clock();
    bus->Write(addr, value);
}

void CPU::WriteWord(Address addr, uint16_t value)
{
    uint8_t lo = value & 0xFF;
    uint8_t hi = (value >> 8) & 0xFF;

    WriteByte(addr, lo);
    WriteByte(addr + 1, hi);
}

uint8_t CPU::FetchByte()
{
    return ReadByte(reg.pc++);
    //return bus->Read(reg.pc++);
}

int8_t CPU::FetchByteSigned()
{
    return (int8_t)ReadByte(reg.pc++);
}

uint16_t CPU::FetchWord()
{
    uint16_t val = ReadWord(reg.pc); reg.pc += 2;
    return val;
}

uint8_t CPU::PopByte()
{
    return ReadByte(reg.sp++);
}

uint16_t CPU::PopWord()
{
    uint16_t val = ReadWord(reg.sp); reg.sp += 2;
    return val;
}

void CPU::PushByte(uint8_t val)
{
    WriteByte(--reg.sp, val);
}

void CPU::PushWord(uint16_t val)
{
    reg.sp -= 2;
    WriteWord(reg.sp, val);
}



// Instruction helpers
// My favourite instruction
void CPU::NOP()
{
    // im thinking migu migu oo ee oo
}

void CPU::INVALID()
{
    ASSERT(false, "HEY THE ROM'S BROKEN");
}

// Loads
void CPU::LD_r8_n8(ByteRegister& dst, uint8_t value)
{
    dst = value;
}

void CPU::LD_r8_r8(ByteRegister& dst, const ByteRegister& value)
{
    dst = value;
}

void CPU::LD_r8_addr(ByteRegister& dst, const WordRegister& addr)
{
    dst = ReadByte(addr);
}

void CPU::LD_addr_r8(const WordRegister& addr, const ByteRegister& value)
{
    WriteByte(addr, value);
}

//Test WriteWord
void CPU::LD_addr_SP(const WordRegister& addr)
{
    uint8_t lo = reg.sp & 0xFF;
    uint8_t hi = (reg.sp >> 8) & 0xFF;
    WriteByte(addr, lo);      // Low byte at address n
    WriteByte(addr + 1, hi);  // High byte at address n+1
}

void CPU::LD_r16_n16(WordRegister& dst, uint16_t value)
{
    dst = value;
}

void CPU::LD_HL_SP_e8(int8_t val)
{
    // Use standard int for calculation to avoid undefined casting behavior
    int sp_int = (int)reg.sp;
    int val_int = (int)val;

    reg.SetZ(0);
    reg.SetN(0);
    // H and C flags calculated on the lower byte
    reg.SetH(((sp_int & 0x0F) + (val_int & 0x0F)) > 0x0F);
    reg.SetC(((sp_int & 0xFF) + (val_int & 0xFF)) > 0xFF);

    // The result is strictly 16-bit
    reg.hl = (uint16_t)(sp_int + val_int);
}

// Arithmetic
void CPU::ADD(uint8_t val)
{
    uint16_t result = reg.a + val;

    reg.SetZ((result & 0xFF) == 0);                 // Zero
    reg.SetN(false);                                // Subtraction
    reg.SetH((reg.a & 0x0F) + (val & 0x0F) > 0x0F); // Half carry
    reg.SetC(result > 0xFF);                        // Carry

    reg.a = result & 0xFF;
}

void CPU::ADDC(uint8_t val)
{
    uint8_t carry = reg.GetC() ? 1 : 0;
    uint16_t result = reg.a + val + carry;

    reg.SetZ((result & 0xFF) == 0);
    reg.SetN(false);
    reg.SetH((reg.a & 0x0F) + (val & 0x0F) + carry > 0x0F); // Sum of nibbles > 0xF
    reg.SetC(result > 0xFF);

    reg.a = result & 0xFF;
}

void CPU::SUB(uint8_t val)
{
    uint16_t result = reg.a - val;

    reg.SetZ((result & 0xFF) == 0);          // Zero
    reg.SetN(true);                          // Subtraction
    reg.SetH((reg.a & 0x0F) < (val & 0x0F)); // Half carry
    reg.SetC(reg.a < val);                   // Carry

    reg.a = result & 0xFF;
}

void CPU::SUBC(uint8_t val)
{
    uint8_t carry = reg.GetC() ? 1 : 0;
    int16_t result = reg.a - val - carry;

    reg.SetZ((result & 0xFF) == 0);
    reg.SetN(true);
    reg.SetH(((reg.a & 0x0F) - (val & 0x0F) - carry) < 0); // (LowerNibble(A) - LowerNibble(Val) - Carry) < 0
    reg.SetC(result < 0);

    reg.a = result & 0xFF;
}

void CPU::ADD_HL(uint16_t val)
{
    uint32_t result = reg.hl + val;

    // Z Flag left untouched
    reg.SetN(false);
    reg.SetH((reg.hl & 0x0FFF) + (val & 0x0FFF) > 0x0FFF); // Bit 11->12 overflow for 16 bit adds
    reg.SetC(result > 0xFFFF);

    reg.hl = result & 0xFFFF;
}

void CPU::ADD_SP_e8(int8_t val)
{
    uint16_t sp = reg.sp;
    int8_t signed_val = val;

    // Flags are calculated based on the lower byte of SP + raw unsigned immediate byte
    int result = sp + signed_val;

    reg.SetZ(false); // Z is always 0 for ADD SP
    reg.SetN(false);

    // Half Carry: Overflow from bit 3
    // We mask with 0xF to isolate the lower nibble
    reg.SetH(((sp & 0xF) + (signed_val & 0xF)) > 0xF);

    // Carry: Overflow from bit 7 (byte overflow)
    // We mask with 0xFF to isolate the lower byte
    reg.SetC(((sp & 0xFF) + (signed_val & 0xFF)) > 0xFF);

    reg.sp = (uint16_t)result;
}

void CPU::CMP(uint8_t val)
{
    uint16_t result = reg.a - val;

    reg.SetZ((result & 0xFF) == 0);          // Zero
    reg.SetN(true);                          // Subtraction
    reg.SetH((reg.a & 0x0F) < (val & 0x0F)); // Half carry
    reg.SetC(reg.a < val);                   // Carry
}

void CPU::INC_r8(ByteRegister& dst)
{
    ++dst;

    reg.SetZ(dst == 0);
    reg.SetN(false);
    reg.SetH((dst & 0x0F) == 0x00); // If lower nibble is 0, there must have been a 0x0F -> 0x10 carry over
}

void CPU::INC_r16(WordRegister& dst)
{
    ++dst;
}

void CPU::DEC_r8(ByteRegister& dst)
{
    dst--;

    reg.SetZ(dst == 0);
    reg.SetN(true);
    reg.SetH((dst & 0x0F) == 0x0F);
}

void CPU::DEC_r16(WordRegister& dst)
{
    --dst;
}


// Bitwise 
void CPU::AND(uint8_t val)
{
    reg.a &= val;

    reg.SetZ(reg.a == 0);
    reg.SetN(false);
    reg.SetH(true);
    reg.SetC(false);
}

void CPU::OR(uint8_t val)
{
    reg.a |= val;

    reg.SetZ(reg.a == 0);
    reg.SetN(false);
    reg.SetH(false);
    reg.SetC(false);
}

void CPU::XOR(uint8_t val)
{
    reg.a ^= val;

    reg.SetZ(reg.a == 0);
    reg.SetN(false);
    reg.SetH(false);
    reg.SetC(false);
}

void CPU::CPL()
{
    reg.a = ~reg.a;
    reg.SetN(true);
    reg.SetH(true);
}

// Carry flag instructions
void CPU::CCF()
{
    reg.SetC(!reg.GetC());
    reg.SetN(false);
    reg.SetH(false);
}

void CPU::SCF()
{
    reg.SetC(true);
    reg.SetN(false);
    reg.SetH(false);
}

// Bit shifts and rotates
void CPU::RL(ByteRegister& dst)
{
    bool oldCarry = reg.GetC();

    reg.SetN(false);
    reg.SetH(false);
    reg.SetC((dst & 0x80) == 0x80);

    dst <<= 1;
    dst += oldCarry;

    reg.SetZ(dst == 0);
}

void CPU::RLC(ByteRegister& dst)
{
    bool first = dst & 0x80;

    reg.SetN(false);
    reg.SetH(false);
    reg.SetC(first);

    dst <<= 1;
    dst += first;

    reg.SetZ(dst == 0);
}

void CPU::RR(ByteRegister& dst)
{
    bool oldCarry = reg.GetC();

    reg.SetN(false);
    reg.SetH(false);
    reg.SetC((dst & 0x01) == 0x01);

    dst >>= 1;
    if (oldCarry) dst |= 0x80;

    reg.SetZ(dst == 0);
}

void CPU::RRC(ByteRegister& dst)
{
    bool last = dst & 0x01;

    reg.SetN(false);
    reg.SetH(false);
    reg.SetC(last);

    dst >>= 1;
    if (last) dst |= 0x80;

    reg.SetZ(dst == 0);
}

void CPU::SLA(ByteRegister& dst)
{
    reg.SetN(false);
    reg.SetH(false);
    reg.SetC((dst & 0x80) == 0x80);

    dst <<= 1;

    reg.SetZ(dst == 0);
}

void CPU::SRA(ByteRegister& dst)
{
    bool bit7 = (dst & 0x80);  // Save the sign bit

    reg.SetN(false);
    reg.SetH(false);
    reg.SetC((dst & 0x01) == 0x01);

    dst >>= 1;
    if (bit7) dst |= 0x80;

    reg.SetZ(dst == 0);
}

void CPU::SRL(ByteRegister& dst)
{
    reg.SetN(false);
    reg.SetH(false);
    reg.SetC((dst & 0x01) == 0x01);

    dst >>= 1;

    reg.SetZ(dst == 0);
}

void CPU::SWAP(ByteRegister& dst)
{
    ByteRegister hi = (dst & 0xF0) >> 4;
    ByteRegister lo = (dst & 0x0F) << 4;
    dst = hi | lo;

    reg.SetZ(dst == 0);
    reg.SetN(false);
    reg.SetH(false);
    reg.SetC(false);
}

// Bit flag instructions
void CPU::BIT(uint8_t bit, const ByteRegister& r)
{
    uint8_t test = 1 << bit;

    reg.SetZ((r & test) == 0);
    reg.SetN(0);
    reg.SetH(1);
}

void CPU::BIT_addr(uint8_t bit, const WordRegister& addr)
{
    uint8_t test = 1 << bit;

    reg.SetZ((ReadByte(addr) & test) == 0);
    reg.SetN(0);
    reg.SetH(1);
}

void CPU::SET(uint8_t bit, ByteRegister& dst)
{
    uint8_t set = 1 << bit;
    dst |= set;
}

void CPU::RES(uint8_t bit, ByteRegister& dst)
{
    uint8_t set = ~(1 << bit);
    dst &= set;
}


// Jumps
void CPU::JP(const WordRegister& addr)
{
    reg.pc = addr;
}

void CPU::JR(int8_t addr)
{
    reg.pc += addr;
}

void CPU::CALL(Address addr)
{
    PushWord(reg.pc);

    reg.pc = addr;
}

void CPU::RET()
{
    reg.pc = PopWord();
}

void CPU::RST(Address vector)
{
    // Only either 00, 08, 10, 18
    //             20, 28, 30, 38
    PushWord(reg.pc);
    reg.pc = vector;
}


// Interrupts
void CPU::DI()
{
    ime = false;
}

void CPU::EI()
{
    imeNext = true;
}

void CPU::HALT()
{
    if (!ime && (interruptFlagEnabled & interruptFlag & 0x1F))
        haltBug = true;
    halted = true;
}


// Misc
void CPU::DAA()
{
    // Register A holds the result of operations of 2 Binary coded decimals (BCDs)
    // DAA will adjust the result to be in BCD as well.
    uint8_t adjustment = 0;
    if (reg.GetN())
    {
        // Subtraction operation
        if (reg.GetH()) adjustment += 0x06;
        if (reg.GetC()) adjustment += 0x60;
        reg.a -= adjustment;
    }
    else
    {
        if (reg.GetH() || (reg.a & 0x0F) > 0x09) adjustment += 0x06;
        if (reg.GetC() || (reg.a > 0x99))
        {
            adjustment += 0x60;
            reg.SetC(true); // 40 + 60 = 1 10, overflowed 99.
        }

        reg.a += adjustment;
    }

    reg.SetZ(reg.a == 0);
    reg.SetH(false);
}

void CPU::STOP()
{
    stopped = true;
    FetchByte();
    bus->Write(0xFF04, 0);  // Internal hardware side effect, skip clocking cycles.
    //WriteByte(0xFF04, 0);
}
