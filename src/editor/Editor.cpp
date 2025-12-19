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
        ImGui::Text("PC: %04X", gb.cpu.reg.pc);
        ImGui::Text("SP: %04X", gb.cpu.reg.sp);
        ImGui::Text("Last OP: %02X", gb.cpu.lastInstruction);
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
