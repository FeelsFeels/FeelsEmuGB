#pragma once
#ifdef __EMSCRIPTEN__
//io.IniFilename = nullptr; // disable auto save/load on web

inline void LoadDefaultWebLayout()
{
    ImGui::LoadIniSettingsFromMemory(
        "[Window][Debug##Default]\n"
        "Pos=60,60\n"
        "Size=400,400\n"
        "Collapsed=0\n"

        "[Window][Emulator Controls]\n"
        "Pos=563,41\n"
        "Size=576,352\n"
        "Collapsed=0\n"
        "DockId=0x00000001,0\n"

        "[Window][Emulator Information]\n"
        "Pos=563,41\n"
        "Size=576,352\n"
        "Collapsed=0\n"
        "DockId=0x00000001,1\n"

        "[Window][Rom Info]\n"
        "Pos=963,463\n"
        "Size=309,263\n"
        "Collapsed=0\n"
        "DockId=0x00000003,0\n"

        "[Window][CPU Info]\n"
        "Pos=960,22\n"
        "Size=312,436\n"
        "Collapsed=0\n"
        "DockId=0x00000001,0\n"

        "[Window][Game Viewport]\n"
        "Pos=3,20\n"
        "Size=428,414\n"
        "Collapsed=0\n"

        "[Window][VRAM]\n"
        "Pos=960,22\n"
        "Size=312,436\n"
        "Collapsed=0\n"
        "DockId=0x00000001,1\n"

        "[Window][Load ROM]\n"
        "Pos=963,463\n"
        "Size=309,263\n"
        "Collapsed=0\n"
        "DockId=0x00000003,1\n"

        "[Window][Settings Panel]\n"
        "Pos=4,426\n"
        "Size=421,270\n"
        "Collapsed=0\n"
        "DockId=0x00000002,0\n"

        "[Window][Control Panel]\n"
        "Pos=8,443\n"
        "Size=424,270\n"
        "Collapsed=0\n"
        "DockId=0x00000002,0\n"

        "[Window][Audio Mixer]\n"
        "Pos=8,443\n"
        "Size=424,270\n"
        "Collapsed=0\n"
        "DockId=0x00000002,1\n"

        "[Window][Tilemap Viewport]\n"
        "Pos=433,20\n"
        "Size=528,574\n"
        "Collapsed=0\n"

        "[Docking][Data]\n"
        "DockNode  ID=0x00000001 Pos=960,22 Size=312,436 Selected=0x8EC440C3\n"
        "DockNode  ID=0x00000002 Pos=8,443 Size=424,270 Selected=0x7188CDAB\n"
        "DockNode  ID=0x00000003 Pos=963,463 Size=309,263 Selected=0x09E987CB\n"
	);	
}


#endif