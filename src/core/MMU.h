#pragma once
#include "Cartridge.h"
#include <memory>
#include <string>



class MMU
{
public:
	MMU() = default;
	~MMU() = default;

	void Read();
	void Write();
	
	void AttachCartridge(Cartridge* cart);
	void RemoveCartridge();

private:
	Cartridge* cartridge;



};