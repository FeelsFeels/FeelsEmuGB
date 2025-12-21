#include "Gameboy.h"

GameBoy::GameBoy()
{
	cpu.AttachBus(&bus);
	bus.AttachPPU(&ppu);

}

GameBoy::~GameBoy()
{}

void GameBoy::InsertCartridge(std::string filepath)
{
	// Clear previous info first
	bus.RemoveCartridge();
	cart.reset();
	pathToCartridge.clear();

	std::vector<uint8_t> romData;
	if (!VFS::ReadFile(filepath, romData))
	{
		// Warn
		std::cout << "Insert Cartridge: Bad path: " << filepath << "\n";
		return;
	}
	
	//std::cout << "Loaded Rom: " << filepath << "\n";
	pathToCartridge = filepath;
	cart = Cartridge::CreateCartridge(std::move(romData));
	bus.AttachCartridge(cart.get());
	bus.RunBootRom();
	cpu.ResetRegisters();
}

const CartridgeInfo& GameBoy::GetCartInfo() const
{
	return cart->info;
}

void GameBoy::Update()
{
	if (!cart)
	{
		return;
	}

	int cycles = cpu.Step();
	ppu.Tick(cycles);

}
