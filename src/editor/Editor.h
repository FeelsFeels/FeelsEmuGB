#pragma once
#include "../core/Gameboy.h"

#include <filesystem>
#include <vector>
#include <string>

#include "imgui.h"


struct RomBrowser
{
	std::string currentPath = "";
	std::string lastPath = "miku_migu_oo_ee_oo";
	std::string selectedFile = "";
	std::vector<std::string> cachedEntries;

    bool isVisible = true;

	bool Draw(std::string* outPath);
};

struct CartInfo
{
	bool isVisible = true;
	void Draw(GameBoy& gb, ImGuiIO& io);
};

struct DebugInfo
{
	bool isVisible = true;
	void Draw(GameBoy& gb, ImGuiIO& io);
};

class Editor
{
public:
	Editor() = default;
	~Editor() = default;

	void Render(GameBoy& gb);


private:
	DebugInfo debugInfo;
	CartInfo cartInfo;
	RomBrowser romBrowser;
};