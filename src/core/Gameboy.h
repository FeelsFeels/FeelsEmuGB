#pragma once
#include "MMU.h"
#include "Cartridge.h"

#include <memory>

class GameBoy
{
public:
	GameBoy();
	~GameBoy();

private:

	std::unique_ptr<Cartridge> cart;

	MMU bus;
};
