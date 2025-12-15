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

	const CartridgeInfo& GetCartInfo() const;
	const std::string& GetCartPath() const { return pathToCartridge; };

private:
	std::string pathToCartridge;
	std::shared_ptr<Cartridge> cart;

	MMU bus;
};
