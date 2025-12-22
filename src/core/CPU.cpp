#include "CPU.h"
#include "Bus.h"

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
}

int CPU::Tick()
{
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

    // EXECUTE
    uint8_t opCode = bus->Read(reg.pc++);
    totalCyclesForInstruction = 0;

    if (instructions[opCode].execute)
    {
        (this->*instructions[opCode].execute)();
    }
    else
    {
        ASSERT(false, "Unimplemented Opcode: %02X", opCode);
        //std::cout << " [ERROR: Unimplemented Opcode " << std::hex << (int)opCode << "]\n";
    }

    totalCyclesForInstruction += instructions[opCode].cycles;
    return totalCyclesForInstruction;
#endif


    uint8_t opcode = bus->Read(reg.pc++);

    totalCyclesForInstruction = 0;

    (this->*instructions[opcode].execute)();
    totalCyclesForInstruction += instructions[opcode].cycles;   //CB instruction increments inside OP_CB()

    lastInstruction = opcode;
    
    static int instruction_count = 0;
    return totalCyclesForInstruction;
}

void CPU::RequestInterrupt(InterruptCode bit)
{
    ASSERT(static_cast<int>(bit) <= 4, "Interrupt bit Out of Range");
    interruptFlag |= (1 << static_cast<int>(bit));
}

int CPU::HandleInterrupts()
{
    uint8_t interrupts = interruptFlag & interruptFlagEnabled;
    if (halted && interrupts)
    {
        halted = false;
    }

    uint8_t interruptBitToHandle = -1;
    if (ime && interrupts)
    {
        ime = false;
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

        switch (interruptBitToHandle)
        {
        case 0: RST(0x40); break; // VBlank
        case 1: RST(0x48); break; // STAT
        case 2: RST(0x50); break; // Timer
        case 3: RST(0x58); break; // Serial
        case 4: RST(0x60); break; // Joypad
        default:
            ASSERT(false, "Interrupt Code invalid: %i", interruptBitToHandle);
        }
        return 20;
    }
    return 0;
}






















// NOOOOOPE YALL STAY DOWN THERE WHERE I CAN'T SEE YOU
// BTW the opcode wrappers are in CPUOpcodes.cpp
// Helpers
uint8_t CPU::FetchByte()
{
    return bus->Read(reg.pc++);
}

uint16_t CPU::FetchWord()
{
    uint16_t lo = FetchByte();
    uint16_t hi = FetchByte();
    return (hi << 8) | lo;
}

uint8_t CPU::PopByte()
{
    return bus->Read(reg.sp++);
}

uint16_t CPU::PopWord()
{
    uint16_t lo = bus->Read(reg.sp++);
    uint16_t hi = bus->Read(reg.sp++);
    return (hi << 8) | lo;
}

void CPU::PushByte(uint8_t val)
{
    bus->Write(--reg.sp, val);
}

void CPU::PushWord(uint16_t val)
{
    uint8_t hi = (val >> 8) & 0xFF;
    uint8_t lo = val & 0xFF;

    bus->Write(--reg.sp, hi);
    bus->Write(--reg.sp, lo);
}



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

void CPU::LD_r8_addr(ByteRegister & dst, const WordRegister& addr)
{
    dst = bus->Read(addr);
}

void CPU::LD_addr_r8(const WordRegister& addr, const ByteRegister& value)
{
    bus->Write(addr, value);
}

void CPU::LD_addr_SP(const WordRegister& addr)
{
    bus->Write(addr, reg.sp);
}

void CPU::LD_r16_n16(WordRegister& dst, uint16_t value)
{
    dst = value;
}

void CPU::LD_HL_SP_e8(int8_t val)
{
    int16_t sp = static_cast<int16_t>(reg.sp);
    uint8_t unsigned_val = static_cast<uint8_t>(val);   // Casting for calculation of flags

    reg.SetZ(0);
    reg.SetN(0);
    // Special case: the gameboy checks H and C flags as if doing unsigned addition.
    reg.SetH(((sp & 0x0F) + (unsigned_val & 0x0F)) > 0x0F);
    reg.SetC(((sp & 0xFF) + unsigned_val) > 0xFF);

    reg.hl = sp + val;
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
    int16_t sp = static_cast<int16_t>(reg.sp);
    uint8_t unsigned_val = static_cast<uint8_t>(val);   // Casting for calculation of flags

    reg.SetZ(0);
    reg.SetN(0);
    // Special case: the gameboy checks H and C flags as if doing unsigned addition.
    reg.SetH(((sp & 0x0F) + (unsigned_val & 0x0F)) > 0x0F);
    reg.SetC(((sp & 0xFF) + unsigned_val) > 0xFF);

    reg.sp = sp + val;
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

void CPU::DEC_r8(ByteRegister & dst)
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

void CPU::RRC(ByteRegister & dst)
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

    reg.SetZ((bus->Read(addr) & test) == 0);
    reg.SetN(0);
    reg.SetH(1);
}

void CPU::SET(uint8_t bit, ByteRegister & dst)
{
    uint8_t set = 1 << bit;
    dst |= set;
}

void CPU::RES(uint8_t bit, ByteRegister & dst)
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
}


