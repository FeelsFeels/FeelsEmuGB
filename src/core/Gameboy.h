#pragma once
#include "MMU.h"
#include "Cartridge.h"

#include <memory>

class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void InsertCartridge(std::string filepath);

private:
	std::shared_ptr<Cartridge> cart;

	MMU bus;
};
