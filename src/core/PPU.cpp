#include "PPU.h"

void PPU::Tick(int cycles)
{
    dots += cycles;

    if (dots <= 80)
    {
        mode = PPUMode::OAM_SCAN;
    }
    else if (dots <= 252)
    {
        mode = PPUMode::DRAWING;
    }
    else if (dots < 456)
    {
        mode = PPUMode::HBLANK;
    }
    else
    {
        dots -= 456;
        ly++; // Move to next line

        if (ly == 144)
        {
            mode = PPUMode::VBLANK;

            // Tell the Bus to Request Interrupt (Bit 0 of IF register)

        }

        if (ly > 153)   // V blank done
        {
            ly = 0; // Reset to top of screen
            mode = PPUMode::OAM_SCAN;
        }

    }

}


uint8_t PPU::Read(Address addr)
{
    if (addr == 0xFF44)
    {
        return 0x90; // Hardcode LY to 144 (V-Blank start)
    }

    switch (addr)
    {
    case 0xFF40: return lcdc;
    case 0xFF41: return stat;
    case 0xFF42: return scy;
    case 0xFF43: return scx;
    case 0xFF44: return ly;
    case 0xFF45: return lyc;
    case 0xFF47: return bgp;
    default: return 0xFF;
    }
}

void PPU::Write(Address addr, uint8_t data)
{
    switch (addr)
    {
    case 0xFF40: lcdc = data; break;
    case 0xFF41: stat = data; break; // Note: Some bits are read-only
    case 0xFF42: scy = data; break;
    case 0xFF43: scx = data; break;
    case 0xFF44: break; // LY is Read-Only! (Reset on write in real hardware)
    case 0xFF45: lyc = data; break;
    case 0xFF47: bgp = data; break;
    }
}