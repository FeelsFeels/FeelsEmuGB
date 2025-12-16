#pragma once
struct Registers
{
    // AF Pair
    union
    {
        struct
        {
            uint8_t f; // Low byte
            uint8_t a; // High byte
        };
        uint16_t af;
    };

    // BC Pair
    union
    {
        struct
        {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };

    // DE Pair
    union
    {
        struct
        {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };

    // HL Pair
    union
    {
        struct
        {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };

    uint16_t sp;
    uint16_t pc;


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