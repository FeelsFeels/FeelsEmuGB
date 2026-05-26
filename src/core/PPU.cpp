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
    obp0 = 0x00; // NOTE: actual behaviour for obp is left uninitialized, but most often 0x00 or 0xFF.
    obp1 = 0x00; //       set as 00 since the emulator supports switching roms during runtime.
    wy = 0x00;
    wx = 0x00;

    std::fill(vram.begin(), vram.end(), 0);
    std::fill(oam.begin(), oam.end(), 0);
}

void PPU::Tick(int cycles)
{
    dots += cycles;

    // We assume a consistent amount of time for each scanline.
    // 80 for OAM, the shortest length of time for Drawing Pixels (172 dots),
    // and the longest time for HBlank (204 dots)

    if (dots <= 80)
    {
        if (mode != PPUMode::OAM_SCAN)
        {
            mode = PPUMode::OAM_SCAN;
            UpdateSTATInterrupt();
        }
    }
    else if (dots <= 252)
    {
        if (mode != PPUMode::DRAWING)
        {
            mode = PPUMode::DRAWING;
            UpdateSTATInterrupt();
        }
    }
    else if (dots < 456)
    {
        if (mode != PPUMode::HBLANK)
        {
            RenderScanlineToBuffer();
            mode = PPUMode::HBLANK;
            UpdateSTATInterrupt();
        }
    }
    else
    {
        dots -= 456;
        ly++; // Move to next line

        if (ly == lyc)
            stat |= 0x04; // Bit 2
        else
            stat &= ~0x04; // Clear Bit 2
        UpdateSTATInterrupt();

        if (ly == 144)  // Start of VBlank
        {
            windowRowsDrawn = 0;
            windowYCondition = false;

            mode = PPUMode::VBLANK;
            UpdateSTATInterrupt();
            bus->RequestInterrupt(InterruptCode::VBLANK);
        }
        else if (ly > 153)   // V blank done
        {
            ly = 0; // Reset to top of screen

            if (ly == lyc)
                stat |= 0x04;
            else
                stat &= ~0x04;
            UpdateSTATInterrupt();

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
    case 0xFF48: return obp0;
    case 0xFF49: return obp1;
    case 0xFF4A: return wy;
    case 0xFF4B: return wx;
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
    case 0xFF41: stat = data; UpdateSTATInterrupt();  break; // Note: Some bits are read-only
    case 0xFF42: scy = data; break;
    case 0xFF43: scx = data; break;
    case 0xFF44: ly = 0; break; // LY is Read-Only! Reset on write
    case 0xFF45: 
        lyc = data; 
        if (ly == lyc) stat |= 0x04; 
        else stat &= ~0x04;
        UpdateSTATInterrupt();
        break;
    case 0xFF47: bgp = data; break;
    case 0xFF48: obp0 = data; break;
    case 0xFF49: obp1 = data; break;
    case 0xFF4A: wy = data; break;
    case 0xFF4B: wx = data; break;
    }
}

void PPU::RenderScanlineToBuffer()
{
    // OAM SCAN - determine up to 10 candidate sprites
    // Scan the background tilemap
    // - Get background color index, save it to a 160byte buffer
    //    - For CGB, BG priority flag. I should just keep it but set to 0 on DMG. Save to buffer as well.
    // Sprites
    // - Sprite pixel buffer
    // - Sprite priority buffer
    // Priority Resolution

    const uint32_t paletteColors[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };

    bool lcdEnabled = (lcdc & 0x80); // bit 7

    if (!lcdEnabled) return; // Screen is off

    bool bgEnabled = (lcdc & 0x01); // bit 0
    bool windowEnabled = (lcdc & 0x20); // bit 5

    bool windowDrawnThisRow = false;

    bgColorIndex.fill(0);
    bgPriority.fill(0);
    spritePixels.fill(OAMPixel{});
    scanlineSprites.count = 0;
    
    if (!windowYCondition && windowEnabled && ly == wy
        && wx <= 166 && wy <= 143)
        windowYCondition = true;


    bool objEnabled = (lcdc & 0x02);
    
    if (objEnabled)
    {
        bool use8x16 = (lcdc & 0x04);
        int spriteHeight = use8x16 ? 16 : 8;

        // OAM SCAN is actually here
        for (int i = 0; i < 40; ++i)
        {
            int index = i * 4;
            OAMObject sprite;
            sprite.yPos = oam[index];
            sprite.xPos = oam[index + 1];
            sprite.tileIndex = oam[index + 2];
            sprite.attributes = oam[index + 3];

            int spriteRow = ly - (sprite.yPos - 16);
            if (spriteRow < 0 || spriteRow >= spriteHeight)
                continue;

            // Insertion sort
            int insertionIndex = scanlineSprites.count;
            while (insertionIndex > 0 && scanlineSprites.sprites[insertionIndex - 1].xPos > sprite.xPos)
            {
                if (insertionIndex < 10)
                {
                    scanlineSprites.sprites[insertionIndex] = scanlineSprites.sprites[insertionIndex - 1];
                }
                insertionIndex--;
            }
            if (insertionIndex < 10)
            {
                scanlineSprites.sprites[insertionIndex] = sprite;
            }
            if (scanlineSprites.count < 10)
            {
                scanlineSprites.count++;
            }
        }

        // Extract color buffer output from sprites
        for (int i = scanlineSprites.count - 1; i >= 0; --i)
        {
            OAMObject sprite = scanlineSprites.sprites[i];
            int spriteRow = ly - (sprite.yPos - 16);

            bool yFlip = sprite.attributes & 0x40;
            bool xFlip = sprite.attributes & 0x20;

            // If Y-Flipped, read from the bottom up
            if (yFlip)
            {
                spriteRow = spriteHeight - 1 - spriteRow;
            }

            if (use8x16)
            {
                sprite.tileIndex &= 0xFE;
            }

            // Fetch Tile Data Address
            // Sprites ALWAYS use 0x8000 mode, regardless of LCDC Bit 4
            Address tileAddr = 0x8000 + (sprite.tileIndex * 16) + (spriteRow * 2);

            uint8_t byte1 = vram[addrVRAM.GetOffset(tileAddr)];
            uint8_t byte2 = vram[addrVRAM.GetOffset(tileAddr + 1)];

            // Draw the 8 pixels of this row
            for (int x = 0; x < 8; x++)
            {
                // Calculate screen X coordinate
                int screenX = (sprite.xPos - 8) + x;

                // Boundary Check: Don't draw off-screen
                if (screenX < 0 || screenX >= 160) continue;

                // Handle X-Flip: Read bits backwards
                int bit = xFlip ? x : (7 - x);

                // Decode Color ID (2 bits)
                uint8_t lo = (byte1 >> bit) & 1;
                uint8_t hi = (byte2 >> bit) & 1;
                uint8_t colorId = (hi << 1) | lo;

                // TRANSPARENCY CHECK: Color 0 is ALWAYS transparent for sprites
                //if (colorId == 0) continue;

                // Determine Palette
                // Bit 4 of attributes: 0 = OBP0 (FF48), 1 = OBP1 (FF49)
                uint8_t paletteReg = (sprite.attributes & 0x10) ? obp1 : obp0;

                // Apply Palette to get the Shade (0-3)
                uint8_t paletteShift = colorId * 2;
                uint8_t shade = (paletteReg >> paletteShift) & 0x03;

                // If 0, Sprite is above BG.
                // If 1, Sprite is behind BG colors 1-3 (only visible over BG Color 0).
                bool priority = sprite.attributes & 0x80;

                spritePixels[screenX].colorIndex = colorId;
                spritePixels[screenX].priority = priority;
                spritePixels[screenX].finalColorIndex = shade;
            }
        }
    }

    // Extract color output from background
    if (bgEnabled)
    {
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
            bgColorIndex[x] = colorId;
        }

    }
    
    // Window
    if (windowEnabled && wx <= 166 && windowYCondition /* && wy <= 143 && wy <= ly */)
        {
            // Address Base of the tilemap to use
            uint16_t winMapBase = (lcdc & 0x40) ? 0x9C00 : 0x9800; // Bit 6

            bool signedAddressing = !(lcdc & 0x10); //  (Bit 4: 0=8800-97FF, 1=8000-8FFF)
            uint16_t dataBase = signedAddressing ? 0x9000 : 0x8000;

            // Window Row = LY(where hardware is drawing) - WY(where the game specifies the window position)
            //uint8_t winY = ly - wy; // Relative Y within the window map.
            //uint16_t tileRow = (winY / 8) * 32;
            uint16_t tileRow = (windowRowsDrawn / 8) * 32;
            uint8_t fineY = windowRowsDrawn % 8;

            for (int x = 0; x < 160; x++)
            {
                // WX is offset by 7. If WX=7, window starts at X=0.
                // If x < (wx - 7), we are to the left of the window.
                int winX = x - (wx - 7);

                if (winX < 0) continue; // Window hasn't started on this line yet

                uint16_t tileCol = winX / 8;
                Address tileAddress = winMapBase + tileRow + tileCol;

                uint8_t tileIndex = vram[addrVRAM.GetOffset(tileAddress)];

                Address tileDataAddr;
                if (signedAddressing)
                    tileDataAddr = 0x9000 + ((int8_t)tileIndex * 16);
                else
                    tileDataAddr = 0x8000 + (tileIndex * 16);

                uint8_t byte1 = vram[addrVRAM.GetOffset(tileDataAddr + (fineY * 2))];
                uint8_t byte2 = vram[addrVRAM.GetOffset(tileDataAddr + (fineY * 2) + 1)];

                int bit = 7 - (winX % 8);
                uint8_t lo = (byte1 >> bit) & 1;
                uint8_t hi = (byte2 >> bit) & 1;
                uint8_t colorId = (hi << 1) | lo;

                // Window always above background
                bgColorIndex[x] = colorId;
                windowDrawnThisRow = true;
            }
        }

    // Final Color resolution
    for (int i = 0; i < 160; ++i)
    {
        // Sprite color == 0, BG wins.
        // BG color == 0, sprite wins.
        // OBJ bit 7 == 0, BG wins, unless BG color == 0
        // Otherwise sprite wins
        // Use bgColorIndex as the final buffer for the screenBuffer
        
        int finalPixelColor = 0;

        int spriteRawColorIndex = spritePixels[i].colorIndex;

        int bgRawColorIndex = bgColorIndex[i];
        int bgFinalColor = (bgp >> (bgRawColorIndex * 2)) & 0x03;

        if (spriteRawColorIndex == 0)
            finalPixelColor = bgFinalColor;
        else if (bgRawColorIndex == 0)
            finalPixelColor = spritePixels[i].finalColorIndex;
        else if (spritePixels[i].priority != 0)
            if (bgRawColorIndex != 0)
                finalPixelColor = bgFinalColor;
            else
                finalPixelColor = spritePixels[i].finalColorIndex;
        else
            finalPixelColor = spritePixels[i].finalColorIndex;

        screenBuffer[ly * 160 + i] = paletteColors[finalPixelColor];
    }

    if (windowDrawnThisRow)
        ++windowRowsDrawn;
}


void PPU::UpdateSTATInterrupt()
{
    bool currentSignal = false;

    // LY=LYC Check
    // We update stat bit 2(LY=LYC) inside PPU::Tick()
    if ((stat & 0x04) && (stat & 0x40))
    {
        currentSignal = true;
    }

    // Mode Checks
    switch (mode)
    {
    case PPUMode::HBLANK: // Mode 0
        if (stat & 0x08) currentSignal = true;
        break;
    case PPUMode::VBLANK: // Mode 1
        if (stat & 0x10) currentSignal = true;
        break;
    case PPUMode::OAM_SCAN: // Mode 2
        if (stat & 0x20) currentSignal = true;
        break;
    case PPUMode::DRAWING: // Mode 3
        // Mode 3 never triggers a STAT interrupt
        break;
    }

    // Rising Edge Detector
    if (currentSignal && !prevStatInterruptSignal)
    {
        bus->RequestInterrupt(InterruptCode::LCD);
    }

    prevStatInterruptSignal = currentSignal;
}

void PPU::SaveState(std::ofstream& out)
{
    // Memory
    GBWriteArr(out, vram);
    GBWriteArr(out, oam);

    // Registers
    GBWrite(out, lcdc);
    GBWrite(out, stat);
    GBWrite(out, scy); GBWrite(out, scx);
    GBWrite(out, ly);  GBWrite(out, lyc);
    GBWrite(out, bgp); GBWrite(out, obp0); GBWrite(out, obp1);
    GBWrite(out, wy);  GBWrite(out, wx);

    // Internal State
    GBWrite(out, mode);
    GBWrite(out, prevStatInterruptSignal);
    GBWrite(out, dots);
}

void PPU::LoadState(std::ifstream& in)
{
    // Memory
    GBReadArr(in, vram);
    GBReadArr(in, oam);

    // Registers
    GBRead(in, lcdc);
    GBRead(in, stat);
    GBRead(in, scy); GBRead(in, scx);
    GBRead(in, ly);  GBRead(in, lyc);
    GBRead(in, bgp); GBRead(in, obp0); GBRead(in, obp1);
    GBRead(in, wy);  GBRead(in, wx);

    // Internal State
    GBRead(in, mode);
    GBRead(in, prevStatInterruptSignal);
    GBRead(in, dots);


}

