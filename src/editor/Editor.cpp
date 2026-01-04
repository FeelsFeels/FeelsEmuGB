#include "Editor.h"
#include "../utils/VFS/VFS.h"
#include "../GameBoySettings.h"


void VRAMBrowser::Init(Renderer* p)
{
    renderer = p;
    textureID = renderer->CreateTexture(WIDTH, HEIGHT);
    pixelBuffer.resize(WIDTH * HEIGHT);
}

void VRAMBrowser::UpdateBuffer(GameBoy& gb)
{
    const auto& vram = gb.ppu.GetVRAM();

    // Loop through all 384 Tiles
    for (int tileIndex = 0; tileIndex < 384; tileIndex++)
    {
        // Calculate where to draw this tile in our big 128x192 texture
        int tileX = tileIndex % 16;
        int tileY = tileIndex / 16;

        // VRAM Address for this tile (16 bytes per tile)
        int tileAddress = tileIndex * 16;

        // Loop through 8 rows of the tile
        for (int row = 0; row < 8; row++)
        {
            // Read the two bytes that define this row
            // Byte 1 (Low Bits) is at offset: row * 2
            // Byte 2 (High Bits) is at offset: row * 2 + 1
            uint8_t byte1 = vram[tileAddress + (row * 2)];
            uint8_t byte2 = vram[tileAddress + (row * 2) + 1];

            // Loop through 8 pixels in the row (Bit 7 down to 0)
            for (int bit = 7; bit >= 0; bit--)
            {
                // Extract the bits
                uint8_t lo = (byte1 >> bit) & 1;
                uint8_t hi = (byte2 >> bit) & 1;

                // Combine: (Hi << 1) | Lo  -> Results in 0, 1, 2, or 3
                int colorIndex = (hi << 1) | lo;
                uint32_t color = palette[colorIndex];

                // Plot to our big pixel buffer
                // Buffer Index = (GlobalY * Width) + GlobalX
                // GlobalX = (TileX * 8) + (7 - bit)  <-- '7-bit' because bit 7 is pixel 0 (leftmost)
                // GlobalY = (TileY * 8) + row
                int x = (tileX * 8) + (7 - bit);
                int y = (tileY * 8) + row;

                pixelBuffer[y * WIDTH + x] = color;
            }
        }
    }

    // Upload to GPU
    renderer->UpdateTexture(textureID, WIDTH, HEIGHT, pixelBuffer.data());
}

void VRAMBrowser::Draw(GameBoy& gb, ImGuiIO& io)
{
    if (!isVisible) return;

    UpdateBuffer(gb);

    if (ImGui::Begin("VRAM", &isVisible))
    {
        float scale = 2.0f;
        ImVec2 imageSize(WIDTH * scale, HEIGHT * scale);

        ImVec2 uv0(0, 0);
        ImVec2 uv1(1, 1);
        // Store cursor position BEFORE drawing so we know where top-left is
        ImVec2 pMin = ImGui::GetCursorScreenPos();
        ImVec2 pMax = ImVec2(pMin.x + imageSize.x, pMin.y + imageSize.y);

        ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);

        // Draw Grid Overlay
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Vertical Lines
        for (int x = 0; x <= 16; x++) // 16 columns
        {
            float xPos = pMin.x + (x * 8 * scale);
            drawList->AddLine(ImVec2(xPos, pMin.y), ImVec2(xPos, pMax.y), IM_COL32(150, 150, 150, 150));
        }
        // Horizontal Lines
        for (int y = 0; y <= 24; y++) // 24 rows
        {
            float yPos = pMin.y + (y * 8 * scale);
            drawList->AddLine(ImVec2(pMin.x, yPos), ImVec2(pMax.x, yPos), IM_COL32(150, 150, 150, 150));
        }

        // Mouse Over Logic
        if (ImGui::IsItemHovered())
        {
            ImGuiIO& io = ImGui::GetIO();
            float mouseLocalX = io.MousePos.x - pMin.x;
            float mouseLocalY = io.MousePos.y - pMin.y;

            int tileX = (int)(mouseLocalX / (8 * scale));
            int tileY = (int)(mouseLocalY / (8 * scale));

            // Safety Clamp
            if (tileX >= 0 && tileX < 16 && tileY >= 0 && tileY < 24)
            {
                int tileIndex = (tileY * 16) + tileX;
                int address = 0x8000 + (tileIndex * 16);

                // Popup
                ImGui::BeginTooltip();
                ImGui::Text("Tile Index: %d (0x%02X)", tileIndex, tileIndex);
                ImGui::Text("Memory Address: 0x%04X", address);
                ImGui::Text("Grid Pos: (%d, %d)", tileX, tileY);
                // Bonus: Show the tile enlarged?
                ImGui::EndTooltip();
            }
        }
    }
    ImGui::End();

}


void TileMapBrowser::Init(Renderer* p)
{
    renderer = p;
    textureID = renderer->CreateTexture(WIDTH, HEIGHT);
    pixelBuffer.resize(WIDTH * HEIGHT);
}

void TileMapBrowser::UpdateBuffer(GameBoy& gb)
{
    const auto& vram = gb.ppu.GetVRAM();

    uint16_t mapBase = showTilemap2 ? 0x9C00 : 0x9800;

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            Address mapAddr = mapBase + (y * 32) + x;
            uint8_t tileIndex = vram[addrVRAM.GetOffset(mapAddr)];

            // Get the Tile Data Address
            bool signedMode = !(gb.ppu.Read(0xFF40) & 0x10);
            Address tileDataAddr;

            // Controls which section of tile data is being read from
            if (signedMode)
                tileDataAddr = 0x9000 + (static_cast<int8_t>(tileIndex) * 16);
            else
                tileDataAddr = 0x8000 + (tileIndex * 16);

            // Draw the 8x8 pixels of this tile into the buffer
            // Calculate where this tile sits in the 256x256 buffer
            int bufferStartX = x * 8;
            int bufferStartY = y * 8;

            // Loop 8 rows(pixels) of the tile
            for (int row = 0; row < 8; row++)
            {
                uint8_t byte1 = vram[addrVRAM.GetOffset(tileDataAddr + (row * 2))];
                uint8_t byte2 = vram[addrVRAM.GetOffset(tileDataAddr + (row * 2) + 1)];

                for (int bit = 0; bit < 8; bit++)
                {
                    // Extract the bits
                    uint8_t lo = (byte1 >> bit) & 0x01;
                    uint8_t hi = (byte2 >> bit) & 0x01;

                    // Combine: (Hi << 1) | Lo  -> Results in 0, 1, 2, or 3
                    int colorIndex = (hi << 1) | lo;
                    uint32_t color = palette[colorIndex];

                    int finalX = bufferStartX + (7 - bit);
                    int finalY = bufferStartY + row;

                    pixelBuffer[finalY * 256 + finalX] = color;
                }
            }
        }
    }
    // Upload to GPU
    renderer->UpdateTexture(textureID, WIDTH, HEIGHT, pixelBuffer.data());
}

void TileMapBrowser::Draw(GameBoy& gb, ImGuiIO& io)
{
    if (!isVisible) return;

    UpdateBuffer(gb);

    if (ImGui::Begin("Tilemap Viewport", &isVisible))
    {
        ImGui::Checkbox("Draw Viewport Box", &drawViewportBox); 
        ImGui::SameLine();
        ImGui::Checkbox("Show Tilemap 2", &showTilemap2);

        ImGui::Separator();

        float scale = 2.0f;
        ImVec2 imageSize(WIDTH * scale, HEIGHT * scale);

        ImVec2 uv0(0, 0);
        ImVec2 uv1(1, 1);

        ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);

        if (drawViewportBox)
        {
            // Viewport position
            uint8_t scy = gb.ppu.Read(0xFF42);
            uint8_t scx = gb.ppu.Read(0xFF43);

            // Setup Dimensions
            // We need the screen position of the image we just drew to calculate offsets
            ImVec2 pMin = ImGui::GetItemRectMin();
            int viewW = 160;
            int viewH = 144;

            // Calculate Segments (Handle Wrapping)
            // A "Segment" defines a Start coordinate and a Length
            struct Segment { int start; int length; };

            // X Axis Logic
            std::vector<Segment> xSegs;
            if (scx + viewW <= 256)
            {
                // No X-wrap
                xSegs.push_back({ (int)scx, viewW });
            }
            else
            {
                // X-wrap: Split into [Right Side] and [Left Side]
                int rightSide = 256 - scx;
                xSegs.push_back({ (int)scx, rightSide });       // e.g., 200 -> 255
                xSegs.push_back({ 0, viewW - rightSide });      // e.g., 0 -> 103
            }

            // Y Axis Logic
            std::vector<Segment> ySegs;
            if (scy + viewH <= 256)
            {
                // No Y-wrap
                ySegs.push_back({ (int)scy, viewH });
            }
            else
            {
                // Y-wrap: Split into [Bottom Side] and [Top Side]
                int bottomSide = 256 - scy;
                ySegs.push_back({ (int)scy, bottomSide });
                ySegs.push_back({ 0, viewH - bottomSide });
            }

            // 4. Draw Rectangles
            // If we wrap on both axes, this loop runs 4 times (drawing 4 corners).
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            for (const auto& xs : xSegs)
            {
                for (const auto& ys : ySegs)
                {
                    // Convert Map Coordinates -> Screen Coordinates
                    float x1 = pMin.x + (xs.start * scale);
                    float y1 = pMin.y + (ys.start * scale);
                    float x2 = x1 + (xs.length * scale);
                    float y2 = y1 + (ys.length * scale);

                    // Draw Yellow Box with 2.0f thickness
                    drawList->AddRect(
                        ImVec2(x1, y1),
                        ImVec2(x2, y2),
                        IM_COL32(255, 255, 0, 255),
                        0.0f, 0, 2.0f
                    );

                    // Optional: Add a faint fill to make it easier to see
                    drawList->AddRectFilled(
                        ImVec2(x1, y1),
                        ImVec2(x2, y2),
                        IM_COL32(255, 255, 0, 50)
                    );
                }
            }
        }

        // Draw Grid Overlay
        ImDrawList* drawList = ImGui::GetWindowDrawList();
    }
    ImGui::End();

}


bool RomBrowser::Draw(std::string* outPath)
{
    if (!isVisible) return false;
    bool fileSelected = false;

    if (ImGui::Begin("Load ROM", &isVisible))
    {
        // Navigation Bar
        if (!currentPath.empty())
        {
            if (ImGui::Button(".."))
            {
                currentPath = VFS::GetParentPath(currentPath);
            }
            ImGui::SameLine();
        }
        ImGui::Text("VFS://%s", currentPath.empty() ? "Root" : currentPath.c_str());
        ImGui::Separator();

        // File List
        float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("Files", ImVec2(0, -footerHeight)))
        {
            if (currentPath != lastPath)
            {
                cachedEntries = VFS::ListDirectory(currentPath);
                lastPath = currentPath;
            }

            // --- DIRECTORIES FIRST ---
            for (const auto& entryName : cachedEntries)
            {
                std::string fullPath = VFS::JoinPath(currentPath, entryName);

                if (VFS::IsDirectory(fullPath))
                {
                    std::string label = "[DIR] " + entryName;
                    if (ImGui::Selectable(label.c_str()))
                    {
                        currentPath = fullPath;
                    }
                }
            }

            // --- FILES SECOND ---
            for (const auto& entryName : cachedEntries)
            {
                std::string fullPath = VFS::JoinPath(currentPath, entryName);

                // Skip directories in this pass
                if (VFS::IsDirectory(fullPath)) continue;

                // Filter Extensions
                std::string ext = VFS::GetExtension(entryName);
                if (ext == ".gb" || ext == ".gbc" || ext == ".rom")
                {
                    bool isSelected = (selectedFile == fullPath);
                    if (ImGui::Selectable(entryName.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        selectedFile = fullPath;
                        if (ImGui::IsMouseDoubleClicked(0))
                        {
                            *outPath = selectedFile;
                            fileSelected = true;
                            //isVisible = false;
                        }
                    }
                }
            }
        }
        ImGui::EndChild();

        // Load Button
        ImGui::Separator();
        if (ImGui::Button("Load Selected") && !selectedFile.empty())
        {
            *outPath = selectedFile;
            fileSelected = true;
            //isVisible = false;
        }
    }
    ImGui::End();

    return fileSelected;
}


void CartInfo::Draw(GameBoy& gb, ImGuiIO& io)
{
    if (!isVisible) return;

    if (ImGui::Begin("Rom Info", &isVisible))
    {
        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::Text("Cartridge: %s", gb.GetCartPath().c_str());
        auto info = gb.GetCartInfo();
        ImGui::Text("MBC Type: %s", info.cartTypeString.c_str());
        ImGui::Text("Rom Size: %u", info.romSizeBytes);
        ImGui::Text("Rom Banks: %u", info.romBanks);
        ImGui::Text("Ram Size: %u", info.ramSizeBytes);
        ImGui::Text("Ram Banks: %u", info.ramBanks);
        ImGui::Text("Has Ram: %s", info.hasRam ? "TRUE" : "FALSE");
        ImGui::Text("Has Battery: %s", info.hasBattery ? "TRUE" : "FALSE");
        ImGui::Text("Has Timer: %s", info.hasTimer ? "TRUE" : "FALSE");

        ImGui::Separator();

        ImGui::Text("CGB Flag: %s", info.cgbFlag ? "TRUE" : "FALSE");

    }
    ImGui::End();

}


void DebugInfo::Draw(GameBoy& gb, ImGuiIO& io)
{
    if (!isVisible) return;

    if (ImGui::Begin("CPU Info", &isVisible))
    {
        const Registers& r = gb.cpu.reg;

        // --- Program state ---
        ImGui::Text("PC: %04X", r.pc);
        ImGui::Text("SP: %04X", r.sp);
        ImGui::Text("Last OP: %02X", static_cast<int>(gb.cpu.lastInstruction));

        ImGui::Separator();

        // --- 16-bit register pairs ---
        ImGui::Text("AF: %04X", r.af);
        ImGui::Text("BC: %04X", r.bc);
        ImGui::Text("DE: %04X", r.de);
        ImGui::Text("HL: %04X", r.hl);

        ImGui::Separator();

        // --- 8-bit registers ---
        ImGui::Text("A: %02X   F: %02X",
            static_cast<int>(r.a),
            static_cast<int>(r.f));
        ImGui::Text("B: %02X   C: %02X",
            static_cast<int>(r.b),
            static_cast<int>(r.c));
        ImGui::Text("D: %02X   E: %02X",
            static_cast<int>(r.d),
            static_cast<int>(r.e));
        ImGui::Text("H: %02X   L: %02X",
            static_cast<int>(r.h),
            static_cast<int>(r.l));

        ImGui::Separator();

        // --- Flags ---
        ImGui::Text("Flags (F): %02X", static_cast<int>(r.f));
        ImGui::Text("Z: %d   N: %d   H: %d   C: %d",
            r.GetZ(),
            r.GetN(),
            r.GetH(),
            r.GetC());


        ImGui::Separator();

        // --- Interrupts ---
        ImGui::Text("IME: %s", gb.cpu.ime ? "ENABLED" : "DISABLED");

        ImGui::Separator();

        const uint8_t IF = gb.cpu.GetIF();
        const uint8_t IE = gb.cpu.GetIE();

        ImGui::Text("Interrupt Registers");
        ImGui::Text("IF (FF0F): %02X", IF);
        ImGui::Text("IE (FFFF): %02X", IE);

        ImGui::Separator();

        ImGui::Text("Interrupt Flags (IF)");
        ImGui::Text("VBL: %d  LCD: %d  TIM: %d  SER: %d  JOY: %d",
            (IF >> 0) & 1,
            (IF >> 1) & 1,
            (IF >> 2) & 1,
            (IF >> 3) & 1,
            (IF >> 4) & 1
        );

        ImGui::Separator();

        ImGui::Text("Interrupt Enables (IE)");
        ImGui::Text("VBL: %d  LCD: %d  TIM: %d  SER: %d  JOY: %d",
            (IE >> 0) & 1,
            (IE >> 1) & 1,
            (IE >> 2) & 1,
            (IE >> 3) & 1,
            (IE >> 4) & 1
        );
        
        ImGui::Separator();

        const uint8_t pending = IF & IE;
        ImGui::Text("Pending (IF & IE): %02X", pending);
    }

    ImGui::End();
}


void ControlPanel::Draw(GameBoy& gb, ImGuiIO& io)
{
    if (!isVisible) return;

    if (ImGui::Begin("Control Panel", &isVisible))
    {
        ImGui::Text("Performance");
        ImGui::Separator();

        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Frame Time: %.2f ms", 1000.0f / io.Framerate);


        ImGui::Spacing();
        ImGui::Text("Emulation Speed");
        ImGui::Separator();

        float& speed = GBSettings::RUNTIME_SPEED;

        // Slider range:
        // -1.0  -> uncapped
        // 0.5x  -> minimum capped
        // 4.0x  -> maximum capped
        const float minSpeed = -1.0f;
        const float maxSpeed = 4.0f;
        if (ImGui::SliderFloat("Runtime Speed", &speed, minSpeed, maxSpeed, "%.2f"))
        {
            // Round to to 0.5 steps
            speed = std::round(speed * 4.0f) / 4.0f;

            if (speed > maxSpeed) speed = maxSpeed;
            if (speed < minSpeed) speed = minSpeed;
        }

        if (speed < 0.0f)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Speed: UNCAPPED");
        }
        else
        {
            ImGui::Text("Speed: %.2fx", speed);
        }

        // Save states
        ImGui::Separator();

        if (ImGui::Button("Save State"))
        {
            SaveState(gb, io);
        }
        if (ImGui::Button("Load State"))
        {
            LoadState(gb, io);
        }

    }
    ImGui::End();
}

void ControlPanel::SaveState(GameBoy& gb, ImGuiIO& io)
{
    gb.SaveState();
}

void ControlPanel::LoadState(GameBoy & gb, ImGuiIO & io)
{
    gb.LoadState();
}


void Editor::Init(Renderer* p)
{
    renderer = p; 
    vramBrowser.Init(renderer);
    tileMapBrowser.Init(renderer);
}

void Editor::Render(GameBoy& gb)
{
    ImGuiIO& io = ImGui::GetIO();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open ROM..."))
			{
				romBrowser.isVisible = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	std::string selectedRomPath;
	if (romBrowser.Draw(&selectedRomPath))
	{
		std::cout << "Loading ROM from: " << selectedRomPath << std::endl;
		gb.InsertCartridge(selectedRomPath);
	}

    debugInfo.Draw(gb, io);
    cartInfo.Draw(gb, io);
    controlPanel.Draw(gb, io);
    vramBrowser.Draw(gb, io);
    tileMapBrowser.Draw(gb, io);
}





// ImGui Helpers
ImVec2 GetWindowChromeSize()
{
    ImGuiStyle& style = ImGui::GetStyle();
    float titleBar = ImGui::GetFontSize() + style.FramePadding.y * 2.0f;

    return ImVec2(
        style.WindowPadding.x * 2.0f,
        style.WindowPadding.y * 2.0f + titleBar
    );
}

void CustomConstraintsCallBack(ImGuiSizeCallbackData* data)
{
    float aspect = *(float*)data->UserData;

    ImVec2 size = data->CurrentSize;
    size.y = size.x / aspect;

    data->DesiredSize = size;
}
