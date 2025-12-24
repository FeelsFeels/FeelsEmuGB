#pragma once
#include "../core/Gameboy.h"
#include "../graphics/Renderer.h"

#include <filesystem>
#include <vector>
#include <string>

#include "imgui.h"


struct VRAMBrowser
{
	void Init(Renderer* p); // Init OpenGL stuff
	void Draw(GameBoy& gb, ImGuiIO& io);

	unsigned int textureID = 0;
	static const int WIDTH = 128; // 16 tiles x 8 pixels = 128
	static const int HEIGHT = 192; // 24 rows x 8 pixels = 192, total 16 x 24 = 384 tiles
	std::vector<uint32_t> pixelBuffer;
	const uint32_t palette[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };

	bool isVisible = true;
	Renderer* renderer;

private:
	void UpdateBuffer(GameBoy& gb);
};

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
	void Init(Renderer* p) { renderer = p; vramBrowser.Init(renderer); }

private:
	Renderer* renderer;

	DebugInfo debugInfo;
	CartInfo cartInfo;
	RomBrowser romBrowser;
	VRAMBrowser vramBrowser;
};




// ImGui Helpers
ImVec2 GetWindowChromeSize();
void CustomConstraintsCallBack(ImGuiSizeCallbackData* data);