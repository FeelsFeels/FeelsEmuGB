#include "Gameboy.h"
#include "../GameBoySettings.h"

GameBoy::GameBoy()
{
	cpu.AttachBus(&bus);

	ppu.AttachBus(&bus);

	timer.AttachBus(&bus);

	joypad.Reset();

	bus.AttachCPU(&cpu);
	bus.AttachPPU(&ppu);
	bus.AttachAPU(&apu);
	bus.AttachTimer(&timer);
	bus.AttachJoypad(&joypad);
}

GameBoy::~GameBoy()
{
	SaveGame();
}

void GameBoy::InsertCartridge(std::string filepath)
{
	// We save the game's ram first if we already have a game loaded
	SaveGame();

	// Clear previous info first
	bus.RemoveCartridge();
	cart.reset();
	pathToCartridge.clear();

	std::vector<uint8_t> romData;
	if (!VFS::ReadFile(filepath, romData))
	{
		std::cout << "Insert Cartridge: Bad path: " << filepath << "\n";
		return;
	}
	
	pathToCartridge = filepath;
	cart = std::move(Cartridge::CreateCartridge(std::move(romData), filepath));
	cart->LoadRAMFromFilepath(filepath);

	bus.AttachCartridge(cart.get());
	bus.RunBootRom();
	cpu.ResetRegisters();
	ppu.ResetRegisters();
	apu.ResetRegisters();
}

// This one is thinking ahead to if I want to deploy on web.
// Not sure what filesystem the web uses, but read into vectors and create the cart from there.
void GameBoy::InsertCartridge(std::vector<uint8_t>&& romData)
{
	bus.RemoveCartridge();
	cart.reset();
	pathToCartridge.clear();

	cart = std::move(Cartridge::CreateCartridge(std::move(romData)));

	bus.AttachCartridge(cart.get());
	bus.RunBootRom();
	cpu.ResetRegisters();
	ppu.ResetRegisters();
	apu.ResetRegisters();
}

void GameBoy::OpenSaveFile(std::vector<uint8_t>&& ramData)
{
	if (!cart)
		return;

	cart->LoadRAM(std::forward<std::vector<uint8_t>>(ramData));
}

const CartridgeInfo* GameBoy::GetCartInfo() const
{
	if (HasCartridge())
	{
		return cart->GetInfo();
	}
	return nullptr;
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
	apu.Tick(cycles);

	return cycles;
}


void GameBoy::UpdateInput(IInputProvider& input)
{
	if (!HasCartridge())
	{
		return;
	}

	const GameInput& inputState = input.GetGameInput();

	// D-pad
	if (inputState[static_cast<size_t>(Buttons::UP)].down)
		joypad.ButtonDown(Buttons::UP);
	else
		joypad.ButtonUp(Buttons::UP);

	if (inputState[static_cast<size_t>(Buttons::DOWN)].down)
		joypad.ButtonDown(Buttons::DOWN);
	else
		joypad.ButtonUp(Buttons::DOWN);

	if (inputState[static_cast<size_t>(Buttons::LEFT)].down)
		joypad.ButtonDown(Buttons::LEFT);
	else
		joypad.ButtonUp(Buttons::LEFT);

	if (inputState[static_cast<size_t>(Buttons::RIGHT)].down)
		joypad.ButtonDown(Buttons::RIGHT);
	else
		joypad.ButtonUp(Buttons::RIGHT);

	//Buttons
	if (inputState[static_cast<size_t>(Buttons::B)].down)
		joypad.ButtonDown(Buttons::B);
	else
		joypad.ButtonUp(Buttons::B);

	if (inputState[static_cast<size_t>(Buttons::A)].down)
		joypad.ButtonDown(Buttons::A);
	else
		joypad.ButtonUp(Buttons::A);

	if (inputState[static_cast<size_t>(Buttons::SELECT)].down)
		joypad.ButtonDown(Buttons::SELECT);
	else
		joypad.ButtonUp(Buttons::SELECT);

	if (inputState[static_cast<size_t>(Buttons::START)].down)
		joypad.ButtonDown(Buttons::START);
	else
		joypad.ButtonUp(Buttons::START);

}

void GameBoy::SetAudioSampleRate(float sampleRate)
{
	apu.SetSampleRate(sampleRate);
}

void GameBoy::GetChannelVolumes(float& ch1, float& ch2, float& ch3, float& ch4)
{
	apu.GetChannelVolumes(ch1, ch2, ch3, ch4);
}

void GameBoy::SaveGame()
{
	if (cart)
	{
		auto info = cart->GetInfo();
		if (info->hasBattery && cart->IsRAMDirty())
		{
			cart->DumpRAMToFile();
			// Or whatever my web saving method is
		}
	}
}

void GameBoy::SaveState()
{
	if (!HasCartridge()) return;

	std::string filepath = VFS::ConvertVirtualToPhysical(cart->GetInfo()->filepath);
	std::string saveStateFilepath = VFS::GetStem(filepath) + ".state";
	saveStateFilepath = VFS::JoinPath(VFS::GetParentPath(filepath), saveStateFilepath);

	std::ofstream out(saveStateFilepath, std::ios::binary);

	SaveStateHeader header;
	header.romChecksum = cart->GetInfo()->headerChecksum;

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
	if (!HasCartridge()) return;

	std::string filepath = VFS::ConvertVirtualToPhysical(cart->GetInfo()->filepath);
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