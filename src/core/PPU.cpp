#include "PPU.h"
#include "Bus.h"

//TODO: For now, we draw the 160 horizontal pixels at the end of every scanline.
//      This will not be perfectly accurate, since the gameboy's ppu outputs one pixel to the screen per dot,
//      with penalties like for background scrolling, bg fetcher, and objects.

void PPU::ResetRegisters()
{
    if (cgbMode)
    {
        lcdc = 0x91;
        stat = 0x85;
        scy = 0x00;
        scx = 0x00;
        ly = 0x90;
        lyc = 0x00;
        bgp = 0xFC;
        obp0 = 0x00;
        obp1 = 0x00;
        wy = 0x00;
        wx = 0x00;
        bcps = 0x00;
        bcpd = 0x00;
        ocps = 0x00;
        ocpd = 0x00;
        opri = 0x00;
        std::fill(vram1.begin(), vram1.end(), 0);
        std::fill(cgbBgPaletteData.begin(), cgbBgPaletteData.end(), 0);
        std::fill(cgbObjPaletteData.begin(), cgbObjPaletteData.end(), 0);
    }
    else
    {
        lcdc = 0x91;
        stat = 0x85;
        scy = 0x00;
        scx = 0x00;
        ly = 0x90;
        lyc = 0x00;
        bgp = 0xFC;
        obp0 = 0x00; // NOTE: actual behaviour for obp is left uninitialized, but most often 0x00 or 0xFF.
        obp1 = 0x00; //       set as 00 since the emulator supports switching roms during runtime.
        wy = 0x00;
        wx = 0x00;
    }

    std::fill(vram.begin(), vram.end(), 0);
    std::fill(oam.begin(), oam.end(), 0);
    std::fill(screenBuffer.begin(), screenBuffer.end(), 0);
}

void PPU::Tick(int cycles)
{
    dots += cycles;

    // We assume a consistent amount of time for each scanline.
    // 80 for OAM, the shortest length of time for Drawing Pixels (172 dots),
    // and the longest time for HBlank (204 dots)

    if (ly < 144)
    {
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
                bus->HBlankTransfer();
            }
        }
    }

    if (dots >= 456)
    {
        dots -= 456;
        ly++;

        if (ly == lyc)
            stat |= 0x04;
        else
            stat &= ~0x04;
        UpdateSTATInterrupt();

        if (ly == 144)
        {
            windowRowsDrawn = 0;
            windowYCondition = false;
            mode = PPUMode::VBLANK;
            UpdateSTATInterrupt();
            bus->RequestInterrupt(InterruptCode::VBLANK);
        }
        else if (ly > 153)
        {
            ly = 0;
            if (ly == lyc)
                stat |= 0x04;
            else
                stat &= ~0x04;
            mode = PPUMode::OAM_SCAN;
            UpdateSTATInterrupt();
        }
    }
}

uint8_t PPU::Read(Address addr)
{
    if (addrVRAM.Contains(addr))
    {
        return selectedVramBank1 ? vram1[addrVRAM.GetOffset(addr)] : vram[addrVRAM.GetOffset(addr)];
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

    case 0xFF68: return bcps;
    case 0xFF69:
    {
        uint8_t offset = bcps & 0x3F;
        return cgbBgPaletteData[offset];
    }
    case 0xFF6A: return ocps;
    case 0xFF6B:
    {
        uint8_t offset = ocps & 0x3F;
        return cgbObjPaletteData[offset];
    }
    case 0xFF6C:
        return opri;
    default: return 0xFF;
    }
}

void PPU::Write(Address addr, uint8_t data)
{
    if (addrVRAM.Contains(addr))
    {
        if(selectedVramBank1)
            vram1[addrVRAM.GetOffset(addr)] = data;
        else
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

    case 0xFF68: bcps = data; break;
    case 0xFF69:
    {
        bool increment = bcps & 0x80;
        uint8_t offset = bcps & 0x3F;
        cgbBgPaletteData[offset] = data;
        if (increment) bcps = (bcps & 0x80) | ((offset + 1) & 0x3F);

        break;
    }
    case 0xFF6A: ocps = data; break;
    case 0xFF6B:
    {
        bool increment = ocps & 0x80;
        uint8_t offset = ocps & 0x3F;
        cgbObjPaletteData[offset] = data;
        if (increment) ocps = (ocps & 0x80) | ((offset + 1) & 0x3F);
        break;
    }
    case 0xFF6C:
        opri = data & 0xFF;
    }
}

void PPU::RenderScanlineToBuffer()
{
    const uint32_t paletteColors[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };
    const std::array<uint8_t, 8192>& vramToUse = selectedVramBank1 ? vram1 : vram;

    bgColorIndex.fill(0);
    bgPriority.fill(0);
    spritePixels.fill(OAMPixel{});
    scanlineSprites.count = 0;

    bool windowEnabled = (lcdc & 0x20); // bit 5
    bool lcdEnabled = (lcdc & 0x80); // bit 7
    bool objEnabled = (lcdc & 0x02);
    
    bool bgEnabled = (lcdc & 0x01); // On CGB, this becomes master BG priority flag, not an enable/disable.
                                    // bgEnabled == false on CGB means OBJ always on top, not that BG is disabled.

    bool masterBGPriority = cgbMode ? bgEnabled : true;


    if (!lcdEnabled) return; // Screen is off

    bool windowDrawnThisRow = false;

    if (!windowYCondition && windowEnabled && ly == wy
        && wx <= 166 && wy <= 143)
        windowYCondition = true;


    if (objEnabled)
    {
        bool use8x16 = (lcdc & 0x04);
        int spriteHeight = use8x16 ? 16 : 8;

        // "OAM SCAN" is actually here
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


            if (scanlineSprites.count >= 10)
                continue; // CGB: OAM priority just takes first 10 in OAM order

            if (!cgbMode || (opri & 0x01))
            {
                // DMG mode sprite priority: insertion sort by lowest X
                int insertionIndex = scanlineSprites.count;
                while (insertionIndex > 0 && scanlineSprites.sprites[insertionIndex - 1].xPos > sprite.xPos)
                {
                    scanlineSprites.sprites[insertionIndex] = scanlineSprites.sprites[insertionIndex - 1];
                    insertionIndex--;
                }
                scanlineSprites.sprites[insertionIndex] = sprite;
            }
            else
            {
                scanlineSprites.sprites[scanlineSprites.count] = sprite;
            }

            scanlineSprites.count++;
        }

        // Extract color buffer output from sprites
        for (int i = scanlineSprites.count - 1; i >= 0; --i)
        {
            OAMObject sprite = scanlineSprites.sprites[i];
            int spriteRow = ly - (sprite.yPos - 16);

            bool yFlip = sprite.attributes & 0x40;
            bool xFlip = sprite.attributes & 0x20;

            if (yFlip)
                spriteRow = spriteHeight - 1 - spriteRow;

            if (use8x16)
                sprite.tileIndex &= 0xFE;

            uint8_t sprVramBank = cgbMode ? ((sprite.attributes >> 3) & 0x01) : 0;
            const auto& sprVram = (sprVramBank == 1) ? vram1 : vram;

            Address tileAddr = 0x8000 + (sprite.tileIndex * 16) + (spriteRow * 2);

            uint8_t byte1 = sprVram[addrVRAM.GetOffset(tileAddr)];
            uint8_t byte2 = sprVram[addrVRAM.GetOffset(tileAddr + 1)];

            for (int x = 0; x < 8; x++)
            {
                int screenX = (sprite.xPos - 8) + x;
                if (screenX < 0 || screenX >= 160) continue;

                int bit = xFlip ? x : (7 - x);

                uint8_t lo = (byte1 >> bit) & 1;
                uint8_t hi = (byte2 >> bit) & 1;
                uint8_t colorId = (hi << 1) | lo;

                if (colorId == 0) continue; // transparent

                spritePixels[screenX].occupied = true;

                bool priority = sprite.attributes & 0x80;

                // DMG palette
                uint8_t paletteReg = (sprite.attributes & 0x10) ? obp1 : obp0;
                uint8_t shade = (paletteReg >> (colorId * 2)) & 0x03;

                // CGB palette index (bits 2-0)
                uint8_t cgbPalette = cgbMode ? (sprite.attributes & 0x07) : 0;

                spritePixels[screenX].colorIndex = colorId;
                spritePixels[screenX].priority = priority;
                spritePixels[screenX].finalColorIndex = shade;      // DMG shade, ignored on CGB
                spritePixels[screenX].cgbPaletteIndex = cgbPalette;
                spritePixels[screenX].vramBank = sprVramBank;
            }
        }
    }


    if (bgEnabled || cgbMode) // on CGB, BG/Window always enabled regardless of LCDC bit 0
    {
        uint8_t tileMap = (lcdc & 0x08);
        uint16_t mapBase = tileMap ? 0x9C00 : 0x9800;
        bool signedAddressing = !(lcdc & 0x10);

        uint8_t yPos = scy + ly;
        uint16_t tileRow = (yPos / 8) * 32;
        uint8_t fineY = yPos % 8;

        for (int x = 0; x < 160; x++)
        {
            uint8_t xPos = scx + x;
            uint16_t tileCol = xPos / 8;
            Address tileAddress = mapBase + tileRow + tileCol;

            uint8_t tileIndex = vram[addrVRAM.GetOffset(tileAddress)];
            uint8_t attr = cgbMode ? vram1[addrVRAM.GetOffset(tileAddress)] : 0;

            bool hFlip = cgbMode && (attr & 0x20);
            bool vFlip = cgbMode && (attr & 0x40);

            uint8_t vramBank = cgbMode ? ((attr >> 3) & 0x01) : 0;
            uint8_t paletteIndex = cgbMode ? (attr & 0x07) : 0;
            bool bgPrio = cgbMode && (attr & 0x80);

            // Select correct vram bank for tile data
            const auto& tileDataVram = (vramBank == 1) ? vram1 : vram;

            Address tileDataAddr;
            if (signedAddressing)
                tileDataAddr = 0x9000 + ((int8_t)tileIndex * 16);
            else
                tileDataAddr = 0x8000 + (tileIndex * 16);

            uint8_t effectiveFineY = vFlip ? (7 - fineY) : fineY;

            uint8_t byte1 = tileDataVram[addrVRAM.GetOffset(tileDataAddr + (effectiveFineY * 2))];
            uint8_t byte2 = tileDataVram[addrVRAM.GetOffset(tileDataAddr + (effectiveFineY * 2) + 1)];

            int bit = hFlip ? (xPos % 8) : (7 - (xPos % 8));

            uint8_t lo = (byte1 >> bit) & 1;
            uint8_t hi = (byte2 >> bit) & 1;
            uint8_t colorId = (hi << 1) | lo;

            bgPixels[x].colorIndex = colorId;
            bgPixels[x].paletteIndex = paletteIndex;
            bgPixels[x].vramBank = vramBank;
            bgPixels[x].hFlip = hFlip;
            bgPixels[x].vFlip = vFlip;
            bgPixels[x].priority = bgPrio;
        }
    }

    if (windowEnabled && wx <= 166 && windowYCondition)
    {
        uint16_t winMapBase = (lcdc & 0x40) ? 0x9C00 : 0x9800;
        bool signedAddressing = !(lcdc & 0x10);

        uint16_t tileRow = (windowRowsDrawn / 8) * 32;
        uint8_t fineY = windowRowsDrawn % 8;

        for (int x = 0; x < 160; x++)
        {
            int winX = x - (wx - 7);
            if (winX < 0) continue;

            uint16_t tileCol = winX / 8;
            Address tileAddress = winMapBase + tileRow + tileCol;

            uint8_t tileIndex = vram[addrVRAM.GetOffset(tileAddress)];

            uint8_t attr = cgbMode ? vram1[addrVRAM.GetOffset(tileAddress)] : 0;

            bool hFlip = cgbMode && (attr & 0x20);
            bool vFlip = cgbMode && (attr & 0x40);

            uint8_t vramBank = cgbMode ? ((attr >> 3) & 0x01) : 0;
            uint8_t paletteIndex = cgbMode ? (attr & 0x07) : 0;
            bool bgPrio = cgbMode && (attr & 0x80);

            const auto& tileDataVram = (vramBank == 1) ? vram1 : vram;

            Address tileDataAddr;
            if (signedAddressing)
                tileDataAddr = 0x9000 + ((int8_t)tileIndex * 16);
            else
                tileDataAddr = 0x8000 + (tileIndex * 16);

            uint8_t effectiveFineY = vFlip ? (7 - fineY) : fineY;

            uint8_t byte1 = tileDataVram[addrVRAM.GetOffset(tileDataAddr + (effectiveFineY * 2))];
            uint8_t byte2 = tileDataVram[addrVRAM.GetOffset(tileDataAddr + (effectiveFineY * 2) + 1)];

            int bit = hFlip ? (winX % 8) : (7 - (winX % 8));

            uint8_t lo = (byte1 >> bit) & 1;
            uint8_t hi = (byte2 >> bit) & 1;
            uint8_t colorId = (hi << 1) | lo;

            // Window overwrites BG
            bgPixels[x].colorIndex = colorId;
            bgPixels[x].paletteIndex = paletteIndex;
            bgPixels[x].vramBank = vramBank;
            bgPixels[x].hFlip = hFlip;
            bgPixels[x].vFlip = vFlip;
            bgPixels[x].priority = bgPrio;
            windowDrawnThisRow = true;
        }
    }

    // Final Color resolution
    for (int i = 0; i < 160; ++i)
    {
        uint8_t bgColorId = bgPixels[i].colorIndex;
        uint8_t sprColorId = spritePixels[i].colorIndex;

        if (cgbMode)
        {
            uint32_t bgColor = CGBColorToARGB(bgPixels[i].paletteIndex, bgColorId, false);
            uint32_t sprColor = CGBColorToARGB(spritePixels[i].cgbPaletteIndex, sprColorId, true);

            // Priority resolution
            // No sprite at this pixel
            if (!spritePixels[i].occupied)
            {
                screenBuffer[ly * 160 + i] = bgColor;
                continue;
            }

            // if not Master BG priority: OBJ always on top
            // BG tile priority bit set AND BG color is not 0: BG wins
            // OBJ attribute priority bit set AND BG color is not 0: BG wins
            // Otherwise sprite wins

            if (!masterBGPriority)
            {
                screenBuffer[ly * 160 + i] = sprColor;
                continue;
            }

            if (bgPixels[i].priority && bgColorId != 0)
            {
                screenBuffer[ly * 160 + i] = bgColor;
                continue;
            }

            if (spritePixels[i].priority && bgColorId != 0)
            {
                screenBuffer[ly * 160 + i] = bgColor;
                continue;
            }

            screenBuffer[ly * 160 + i] = sprColor;
        }
        else
        {
            // DMG priority resolution
            int bgFinalColor = (bgp >> (bgColorId * 2)) & 0x03;

            int finalPixelColor = 0;
            if (!spritePixels[i].occupied)
                finalPixelColor = bgFinalColor;
            else if (bgColorId == 0)
                finalPixelColor = spritePixels[i].finalColorIndex;
            else if (spritePixels[i].priority != 0)
                finalPixelColor = bgFinalColor;
            else
                finalPixelColor = spritePixels[i].finalColorIndex;

            screenBuffer[ly * 160 + i] = paletteColors[finalPixelColor];
        }
    }


    if (windowDrawnThisRow)
        ++windowRowsDrawn;
}


void PPU::RenderScanlineToBuffer2()
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
        
    const std::array<uint8_t, 8192>& vramToUse = selectedVramBank1 ? vram1 : vram;

    const auto& tileMapBank = vram;
    const auto& attrMapBank = vram1;

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

            uint8_t byte1 = vramToUse[addrVRAM.GetOffset(tileAddr)];
            uint8_t byte2 = vramToUse[addrVRAM.GetOffset(tileAddr + 1)];

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
            uint8_t tileIndex = vramToUse[addrVRAM.GetOffset(tileAddress)];

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
            uint8_t byte1 = vramToUse[addrVRAM.GetOffset(tileDataAddr + (fineY * 2))];
            uint8_t byte2 = vramToUse[addrVRAM.GetOffset(tileDataAddr + (fineY * 2) + 1)];

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

                uint8_t tileIndex = vramToUse[addrVRAM.GetOffset(tileAddress)];

                Address tileDataAddr;
                if (signedAddressing)
                    tileDataAddr = 0x9000 + ((int8_t)tileIndex * 16);
                else
                    tileDataAddr = 0x8000 + (tileIndex * 16);

                uint8_t byte1 = vramToUse[addrVRAM.GetOffset(tileDataAddr + (fineY * 2))];
                uint8_t byte2 = vramToUse[addrVRAM.GetOffset(tileDataAddr + (fineY * 2) + 1)];

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






uint32_t PPU::CGBColorToARGB(uint8_t paletteIndex, uint8_t colorIndex, bool isObj)
{
    const auto& paletteData = isObj ? cgbObjPaletteData : cgbBgPaletteData;
    int byteIndex = (paletteIndex * 4 * 2) + (colorIndex * 2);
    uint16_t color = paletteData[byteIndex] | (paletteData[byteIndex + 1] << 8);
    uint8_t r = (color >> 0) & 0x1F;
    uint8_t g = (color >> 5) & 0x1F;
    uint8_t b = (color >> 10) & 0x1F;
    
    r = (r << 3) | (r >> 2);
    g = (g << 3) | (g >> 2);
    b = (b << 3) | (b >> 2);
    //return 0xFF000000 | (r << 16) | (g << 8) | b;
    return 0xFF000000 | (b << 16) | (g << 8) | r; // BGR order
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

