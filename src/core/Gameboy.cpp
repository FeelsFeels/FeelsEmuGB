#include "Gameboy.h"

GameBoy::GameBoy()
{
	cpu.AttachBus(&bus);

	ppu.AttachBus(&bus);

	timer.AttachBus(&bus);

	bus.AttachCPU(&cpu);
	bus.AttachPPU(&ppu);
	bus.AttachTimer(&timer);
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

	int cycles = cpu.HandleInterrupts();

	if (cycles == 0)
	{
		cycles = cpu.Tick();
	}

	ppu.Tick(cycles);
}
