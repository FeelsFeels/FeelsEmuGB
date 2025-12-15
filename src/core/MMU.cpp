#include "MMU.h"

void MMU::AttachCartridge(Cartridge* cart)
{
	cartridge = cart;
}

void MMU::RemoveCartridge()
{
	cartridge = nullptr;
}
