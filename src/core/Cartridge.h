#pragma once
#include <filesystem>
#include "CartridgeHeader.h"

// 0100-0103 — Entry point
// 0104-0133 — Nintendo logo
// 




class Cartridge
{
public:
	Cartridge();
	~Cartridge();

	void LoadROM(std::string pathToROM);


private:

};