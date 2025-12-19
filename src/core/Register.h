#pragma once

using ByteRegister = uint8_t;
using WordRegister = uint16_t;

struct Registers
{
    // AF Pair
    union
    {
        struct
        {
            ByteRegister f; // Low byte
            ByteRegister a; // High byte
        };
        WordRegister af;
    };

    // BC Pair
    union
    {
        struct
        {
            ByteRegister c;
            ByteRegister b;
        };
        WordRegister bc;
    };

    // DE Pair
    union
    {
        struct
        {
            ByteRegister e;
            ByteRegister d;
        };
        WordRegister de;
    };

    // HL Pair
    union
    {
        struct
        {
            ByteRegister l;
            ByteRegister h;
        };
        WordRegister hl;
    };

    WordRegister sp;
    WordRegister pc;


    // Helpers

    // For flags, only the top 4 bits are used.
    void SetFlags(uint8_t data)
    {
        f = data & 0xF0; // Perhaps just avoid this operation in favour of just avoiding the usage of those bits.
    }

    bool GetZ() const { return (f >> 7) & 1; } // Zero
    bool GetN() const { return (f >> 6) & 1; } // Subtraction
    bool GetH() const { return (f >> 5) & 1; } // Half Carry
    bool GetC() const { return (f >> 4) & 1; } // Carry

    void SetZ(bool val) { if (val) f |= 0x80; else f &= ~0x80; }
    void SetN(bool val) { if (val) f |= 0x40; else f &= ~0x40; }
    void SetH(bool val) { if (val) f |= 0x20; else f &= ~0x20; }
    void SetC(bool val) { if (val) f |= 0x10; else f &= ~0x10; }
};
