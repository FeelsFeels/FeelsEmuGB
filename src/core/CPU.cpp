#include "CPU.h"

void CPU::ADD(uint8_t val)
{
    uint16_t result = reg.a + val;

    reg.SetZ((result & 0xFF) == 0);                 // Zero
    reg.SetN(false);                                // Subtraction
    reg.SetH((reg.a & 0x0F) + (val & 0x0F) > 0x0F); // Half carry
    reg.SetC(result > 0xFF);                        // Carry

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

void CPU::ADD_HL(uint16_t val)
{
    uint32_t result = reg.hl + val;

    // Z Flag left untouched
    reg.SetN(false);
    reg.SetH((reg.hl & 0x0FFF) + (val & 0x0FFF) > 0x0FFF); // Bit 11->12 overflow for 16 bit adds
    reg.SetC(result > 0xFFFF);
    reg.hl = result & 0xFFFF;
}