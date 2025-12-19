#include "Editor.h"
#include "../utils/VFS/VFS.h"


bool RomBrowser::Draw(std::string* outPath)
{
    if (!isVisible) return false;
    bool fileSelected = false;

    if (ImGui::Begin("Load ROM (VFS)", &isVisible))
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
                            isVisible = false;
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
            isVisible = false;
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
    }

    ImGui::End();
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


    ImGui::Render();
}
