#include "Gameboy.h"

GameBoy::GameBoy()
{
	cpu.AttachBus(&bus);

	ppu.AttachBus(&bus);

	timer.AttachBus(&bus);

	joypad.Reset();

	bus.AttachCPU(&cpu);
	bus.AttachPPU(&ppu);
	bus.AttachTimer(&timer);
	bus.AttachJoypad(&joypad);
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
	cart = Cartridge::CreateCartridge(std::move(romData), filepath);
	bus.AttachCartridge(cart.get());
	bus.RunBootRom();
	cpu.ResetRegisters();
	ppu.ResetRegisters();
}

const CartridgeInfo& GameBoy::GetCartInfo() const
{
	return cart->GetInfo();
}

int GameBoy::Update()
{
	if (!cart)
	{
		return 0;
	}

	int cycles = cpu.Tick();
	timer.Tick(cycles);
	ppu.Tick(cycles);

	return cycles;
}


void GameBoy::UpdateInput(std::unordered_map<SDL_Scancode, ButtonState>& keyboard)
{
	// D-pad
	if (keyboard[SDL_SCANCODE_W].down)
		joypad.ButtonDown(Buttons::UP);
	else
		joypad.ButtonUp(Buttons::UP);

	if (keyboard[SDL_SCANCODE_S].down)
		joypad.ButtonDown(Buttons::DOWN);
	else
		joypad.ButtonUp(Buttons::DOWN);

	if (keyboard[SDL_SCANCODE_A].down)
		joypad.ButtonDown(Buttons::LEFT);
	else
		joypad.ButtonUp(Buttons::LEFT);

	if (keyboard[SDL_SCANCODE_D].down)
		joypad.ButtonDown(Buttons::RIGHT);
	else
		joypad.ButtonUp(Buttons::RIGHT);

	//Buttons
	if (keyboard[SDL_SCANCODE_J].down)
		joypad.ButtonDown(Buttons::B);
	else
		joypad.ButtonUp(Buttons::B);

	if (keyboard[SDL_SCANCODE_K].down)
		joypad.ButtonDown(Buttons::A);
	else
		joypad.ButtonUp(Buttons::A);

	if (keyboard[SDL_SCANCODE_Z].down)
		joypad.ButtonDown(Buttons::SELECT);
	else
		joypad.ButtonUp(Buttons::SELECT);

	if (keyboard[SDL_SCANCODE_X].down)
		joypad.ButtonDown(Buttons::START);
	else
		joypad.ButtonUp(Buttons::START);

}


void GameBoy::SaveState()
{
	std::string filepath = VFS::ConvertVirtualToPhysical(cart->GetInfo().filepath);
	std::string saveStateFilepath = VFS::GetStem(filepath) + ".state";
	saveStateFilepath = VFS::JoinPath(VFS::GetParentPath(filepath), saveStateFilepath);

	std::ofstream out(saveStateFilepath, std::ios::binary);

	SaveStateHeader header;
	header.romChecksum = cart->GetInfo().headerChecksum;

	// Write components
	GBWrite(out, header);
	cpu.SaveState(out);
	bus.SaveState(out);
	ppu.SaveState(out);
	cart->SaveState(out);
	timer.SaveState(out);
}

void GameBoy::LoadState()
{
	std::string filepath = VFS::ConvertVirtualToPhysical(cart->GetInfo().filepath);
	std::string saveStateFilepath = VFS::GetStem(filepath) + ".state";
	saveStateFilepath = VFS::JoinPath(VFS::GetParentPath(filepath), saveStateFilepath);

	std::ifstream in(saveStateFilepath, std::ios::binary);
	if (!in)
	{
		std::cout << "No Savestates found!\n";
		return;
	}

	SaveStateHeader header;
	GBRead(in, header);

	if (header.magic != SAVESTATE_MAGIC)
	{
		printf("Error: Not a valid save state.\n");
		return;
	}
	if (header.version != SAVESTATE_VERSION)
	{
		printf("Error: Save state version mismatch.\n");
		return;
	}

	cpu.LoadState(in);
	bus.LoadState(in);
	ppu.LoadState(in);
	cart->LoadState(in);
	timer.LoadState(in);
}