#include "Cartridge.h"
#include "MBC.h"

Cartridge::Cartridge(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: info {std::move(info)}, rom {std::move(romData)}
{
	ram.resize(this->info.ramSizeBytes);
	if (this->info.hasBattery)
	{
		// Find .sav file where the info->filepath is located
		std::string saveFilepath = VFS::GetStem(this->info.filepath) + ".sav";
		saveFilepath = VFS::JoinPath(VFS::GetParentPath(this->info.filepath), saveFilepath);
		if(VFS::FileExists(saveFilepath))
		{
			std::cout << "Save file found: " << saveFilepath << "\n";
			VFS::ReadFile(saveFilepath, ram);
		}
		this->info.saveFilepath = saveFilepath;
	}
}

Cartridge::~Cartridge()
{
	if (info.hasBattery && ramDirty)
	{
		DumpRAMToFile();
	}
}

void Cartridge::LoadRAM(std::vector<uint8_t> ramData)
{
	ram = ramData;
}

std::unique_ptr<Cartridge> Cartridge::CreateCartridge(std::vector<uint8_t>&& romData, std::string filepath)
{
	CartridgeInfo cartInfo = ParseCartridgeHeader(romData);
	cartInfo.filepath = filepath;
	
	switch (cartInfo.type)
	{
	case CartridgeType::ROM_ONLY:
		return std::make_unique<NoMBC>(std::move(cartInfo), std::move(romData));

	case CartridgeType::MBC1:
	case CartridgeType::MBC1_RAM:
	case CartridgeType::MBC1_RAM_BATTERY:
		return std::make_unique<MBC1>(std::move(cartInfo), std::move(romData));

	case CartridgeType::MBC2:
	case CartridgeType::MBC2_BATTERY:
		return std::make_unique<MBC2>(std::move(cartInfo), std::move(romData));

	case CartridgeType::MBC3:
	case CartridgeType::MBC3_TIMER_BATTERY:
	case CartridgeType::MBC3_TIMER_RAM_BATTERY:
	case CartridgeType::MBC3_RAM:
	case CartridgeType::MBC3_RAM_BATTERY:
		return std::make_unique<MBC3>(std::move(cartInfo), std::move(romData));

	case CartridgeType::MBC5:
	case CartridgeType::MBC5_RAM:
	case CartridgeType::MBC5_RAM_BATTERY:
	case CartridgeType::MBC5_RUMBLE:
	case CartridgeType::MBC5_RUMBLE_RAM_BATTERY:
		return std::make_unique<MBC5>(std::move(cartInfo), std::move(romData));


	// Not supported, yet:
	// MMM01 family
	// MBC6/7/POCKET CAMERA, BANDAI, HUCx
	// ROM_RAM family undefined behaviour. Skipping.

	default:
		return std::make_unique<InvalidMBC>(std::move(cartInfo), std::move(romData));
		break;
	}

}

CartridgeInfo Cartridge::ParseCartridgeHeader(const std::vector<uint8_t>& romData)
{
	CartridgeInfo cartInfo;

	// Entry point 0100-0103
	// Nintendo Logo 0104-0133
	
	// Title
	// These bytes contain the title of the game in upper case ASCII. If the title is less than 16 characters long, the remaining bytes should be padded with $00s.
	const char* rawTitle = reinterpret_cast<const char*>(romData.data() + CartridgeHeaderAddresses::TITLE.start);
	std::string tempTitle(rawTitle, strnlen(rawTitle, CartridgeHeaderAddresses::TITLE.Size()));
	for (auto& c : tempTitle)
	{
		if (!std::isprint(static_cast<unsigned char>(c)))
		{
			c = '\0'; // Treat garbage as an early terminator
			break;
		}
	}
	while (!tempTitle.empty() && (tempTitle.back() == '\0' || tempTitle.back() == ' ')) // Some devs pad title with spaces instead of nulls
	{
		tempTitle.pop_back();
	}
	if (tempTitle.empty())
	{
		cartInfo.title = "[UNTITLED]"; // Fallback for test ROMs / no name ROMs
	}
	else
	{
		cartInfo.title = tempTitle;
	}
	// More title edge cases:
	// Check CGB flag to redetermine title length. CGB games have the title shrunk to 11 characters to account for the Manufacturer Code

	

	cartInfo.type = static_cast<CartridgeType>(romData[CartridgeHeaderAddresses::CARTRIDGE_TYPE.start]);
	cartInfo.cartTypeString = CartridgeTypeToString(cartInfo.type);
	if (cartInfo.cartTypeString == "UNKNOWN")
	{
		std::cout << "Bad Cartridge\n";
		// warn or something
	}

	cartInfo.romSizeType = static_cast<RomSize>(romData[CartridgeHeaderAddresses::ROM_SIZE.start]);
	cartInfo.romSizeBytes = GetRomSizeBytes(cartInfo.romSizeType);
	cartInfo.romBanks = GetRomBanks(cartInfo.romSizeType);
	if(cartInfo.romBanks > 0) cartInfo.romBankSize = cartInfo.romSizeBytes / cartInfo.romBanks;
	cartInfo.ramSizeType = static_cast<RamSize>(romData[CartridgeHeaderAddresses::RAM_SIZE.start]);
	cartInfo.ramSizeBytes = GetRamSizeBytes(cartInfo.ramSizeType);
	cartInfo.ramBanks = GetRamBanks(cartInfo.ramSizeType);
	if(cartInfo.ramBanks > 0) cartInfo.ramBankSize = cartInfo.ramSizeBytes / cartInfo.ramBanks;

	cartInfo.hasRam = CartridgeHasRam(cartInfo.type);
	cartInfo.hasBattery = CartridgeHasBattery(cartInfo.type);
	cartInfo.hasTimer = CartridgeHasTimer(cartInfo.type);

	uint8_t cgb = romData[CartridgeHeaderAddresses::CGB_FLAG.start];
	cartInfo.cgbFlag = (cgb == 0x80 || cgb == 0xC0) ? true : false;


	// Compute and verify checksum
	cartInfo.headerChecksum = romData[CartridgeHeaderAddresses::HEADER_CHECKSUM.start];
	uint8_t checksum = 0;
	for (uint16_t address = 0x0134; address <= 0x014C; address++)
	{
		checksum = checksum - romData[address] - 1;
	}

	if (checksum != cartInfo.headerChecksum)
	{
		std::cout << "[WARNING] Cartridge Header Checksum does not match!\n";
		cartInfo.headerChecksumValid = false;
	}
	else
	{
		cartInfo.headerChecksumValid = true;
	}
	return cartInfo;
}


void Cartridge::SaveState(std::ofstream& out)
{
	GBWriteVec(out, ram);
	GBWrite(out, ramDirty);
}

void Cartridge::LoadState(std::ifstream& in)
{
	GBReadVec(in, ram);
	GBRead(in, ramDirty);
}

void Cartridge::DumpRAMToFile()
{
	VFS::WriteFile(info.saveFilepath, ram);
}


