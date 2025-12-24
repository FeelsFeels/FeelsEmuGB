#include "PPU.h"
#include "Bus.h"

//TODO: For now, we draw the 160 horizontal pixels at the end of every scanline.
//      This will not be perfectly accurate, since the gameboy's ppu outputs one pixel to the screen per dot,
//      with penalties like for background scrolling, bg fetcher, and objects.

void PPU::ResetRegisters()
{
    lcdc = 0x91;
    stat = 0x81;
    scy = 0x00;
    scx = 0x00;
    ly = 0x91;
    lyc = 0x00;
    bgp = 0xFC;

    std::fill(vram.begin(), vram.end(), 0);
    std::fill(oam.begin(), oam.end(), 0);
}

void PPU::Tick(int cycles)
{
    dots += cycles;

    // OAM SCAN HERE

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
        if (mode == PPUMode::DRAWING)
        {
            RenderScanlineToBuffer();
        }
        mode = PPUMode::HBLANK;
    }
    else
    {
        dots -= 456;
        ly++; // Move to next line

        if (ly == 144)
        {
            mode = PPUMode::VBLANK;

            // Tell the Bus to Request Interrupt
            bus->RequestInterrupt(InterruptCode::VBLANK);
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
    if (addrVRAM.Contains(addr))
    {
        return vram[addrVRAM.GetOffset(addr)];
    }
    else if (addrOAM.Contains(addr))
    {
        return oam[addrOAM.GetOffset(addr)];
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
    if (addrVRAM.Contains(addr))
    {
        vram[addrVRAM.GetOffset(addr)] = data;
    }
    else if (addrOAM.Contains(addr))
    {
        oam[addrOAM.GetOffset(addr)] = data;
    }

    switch (addr)
    {
    case 0xFF40: lcdc = data; break;
    case 0xFF41: stat = data; break; // Note: Some bits are read-only
    case 0xFF42: scy = data; break;
    case 0xFF43: scx = data; break;
    case 0xFF44: ly = 0; break; // LY is Read-Only! Reset on write
    case 0xFF45: lyc = data; break;
    case 0xFF47: bgp = data; break;
    }
}

void PPU::RenderScanlineToBuffer()
{
    //Check LCD Control Register for settings
    bool lcdEnabled = (lcdc & 0x80); // bit 7
    bool bgEnabled = (lcdc & 0x01); // bit 0

    if (!lcdEnabled) return; // Screen is off

    // Background Rendering
    if (bgEnabled)
    {
        const uint32_t paletteColors[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };

        // Determine which Tile Map to use
        uint8_t tileMap = (lcdc & 0x08); // bit 3: 0=9800-9BFF, 1=9C00-9FFF
        uint16_t mapBase = tileMap ? 0x9C00 : 0x9800;

        // Determine which Tile Data area to use
        // Note: 0x8800 method uses signed indices!
        bool signedAddressing = !(lcdc & 0x10); //  (Bit 4: 0=8800-97FF, 1=8000-8FFF)
        uint16_t dataBase = signedAddressing ? 0x9000 : 0x8000;

        // Calculate Y position in the 256x256 background map
        uint8_t yPos = scy + ly;

        // Which row of tiles are we on? each tile being 8x8
        uint16_t tileRow = (yPos / 8) * 32;

        // Which line inside the tile (0-7) are we drawing?
        uint8_t fineY = yPos % 8;

        for (int x = 0; x < 160; x++)
        {
            // Calculate X position in the 256x256 background map
            uint8_t xPos = scx + x;

            // Which column of tiles is this? (x / 8)
            uint16_t tileCol = xPos / 8;

            // Get the Tile Address in the Map
            Address tileAddress = mapBase + tileRow + tileCol;

            // Read the Tile ID (Index)
            uint8_t tileIndex = vram[addrVRAM.GetOffset(tileAddress)];

            // Calculate address of the specific Tile Data (16 bytes per tile)
            Address tileDataAddr;
            if (signedAddressing)
            {
                // In 0x8800 mode, indices are -128 to 127. 
                // 0x9000 is the center (index 0).
                tileDataAddr = 0x9000 + ((int8_t)tileIndex * 16);
            }
            else
            {
                // In 0x8000 mode, indices are 0 to 255
                tileDataAddr = 0x8000 + (tileIndex * 16);
            }

            // Read the 2 bytes for the row of pixels we need
            uint8_t byte1 = vram[addrVRAM.GetOffset(tileDataAddr + (fineY * 2))];
            uint8_t byte2 = vram[addrVRAM.GetOffset(tileDataAddr + (fineY * 2) + 1)];

            // Which bit are we decoding? (7 = left, 0 = right)
            int bit = 7 - (xPos % 8);

            // Extract the 2-bit color ID
            uint8_t lo = (byte1 >> bit) & 1;
            uint8_t hi = (byte2 >> bit) & 1;
            uint8_t colorId = (hi << 1) | lo;

            // Apply Palette (BGP - FF47)
            uint8_t paletteShift = colorId * 2;
            uint8_t shade = (bgp >> paletteShift) & 0x03;

            // Write to Screen Buffer
            screenBuffer[ly * 160 + x] = paletteColors[shade];
        }
    }

    RenderSprites();
}

void PPU::RenderSprites()
{
    // Check if Sprites are enabled (Bit 1 of LCDC)
    bool objEnabled = (lcdc & 0x02);
    if (!objEnabled) return;

    // Determine Sprite Height (8x8 or 8x16) - Bit 2 of LCDC
    bool use8x16 = (lcdc & 0x04);
    int spriteHeight = use8x16 ? 16 : 8;

    // Loop through all 40 Sprites
    // Note/TODO: On real hardware, there is priority, as well as 10 per line limit.
    for (int i = 0; i < 40; i++)
    {
        // OAM Entry: 4 Bytes per sprite
        int index = i * 4;

        // Byte 0: Y Position (Screen Y + 16)
        uint8_t yPos = oam[index];
        // Byte 1: X Position (Screen X + 8)
        uint8_t xPos = oam[index + 1];
        // Byte 2: Tile Index
        uint8_t tileIndex = oam[index + 2];
        // Byte 3: Attributes / Flags
        uint8_t attributes = oam[index + 3];

        // Is this sprite on the current scanline?
        // We compare the sprite's vertical range with the current line (ly)
        int spriteRow = ly - (yPos - 16);

        if (spriteRow < 0 || spriteRow >= spriteHeight)
        {
            continue;
        }

        // Handle Flips and 8x16 Mode
        bool yFlip = attributes & 0x40;
        bool xFlip = attributes & 0x20;

        // If Y-Flipped, read from the bottom up
        if (yFlip)
        {
            spriteRow = spriteHeight - 1 - spriteRow;
        }

        // 8x16 Mode Note: In 8x16, the tile index ignores the LSB (always even)
        if (use8x16)
        {
            tileIndex &= 0xFE;
        }

        // Fetch Tile Data Address
        // Sprites ALWAYS use 0x8000 mode, regardless of LCDC Bit 4
        Address tileAddr = 0x8000 + (tileIndex * 16) + (spriteRow * 2);

        uint8_t byte1 = vram[addrVRAM.GetOffset(tileAddr)];
        uint8_t byte2 = vram[addrVRAM.GetOffset(tileAddr + 1)];

        // 7. Draw the 8 pixels of this row
        for (int x = 0; x < 8; x++)
        {
            // Calculate screen X coordinate
            int screenX = (xPos - 8) + x;

            // Boundary Check: Don't draw off-screen
            if (screenX < 0 || screenX >= 160) continue;

            // Handle X-Flip: Read bits backwards
            int bit = xFlip ? x : (7 - x);

            // Decode Color ID (2 bits)
            uint8_t lo = (byte1 >> bit) & 1;
            uint8_t hi = (byte2 >> bit) & 1;
            uint8_t colorId = (hi << 1) | lo;

            // TRANSPARENCY CHECK: Color 0 is ALWAYS transparent for sprites
            if (colorId == 0) continue;

            // Determine Palette
            // Bit 4 of attributes: 0 = OBP0 (FF48), 1 = OBP1 (FF49)
            uint8_t paletteReg = (attributes & 0x10) ? bus->Read(0xFF49) : bus->Read(0xFF48);

            // Apply Palette to get the Shade (0-3)
            uint8_t paletteShift = colorId * 2;
            uint8_t shade = (paletteReg >> paletteShift) & 0x03;

            // Write to Screen Buffer
            // We reuse the standard palette colors
            const uint32_t paletteColors[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };

            // Priority Check (Bit 7):
            // If 0, Sprite is above BG.
            // If 1, Sprite is behind BG colors 1-3 (only visible over BG Color 0).
            bool priority = attributes & 0x80;

            // TODO: Priority checks.
            screenBuffer[ly * 160 + screenX] = paletteColors[shade];
        }
    }
}
