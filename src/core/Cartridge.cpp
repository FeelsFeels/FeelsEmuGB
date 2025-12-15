#include "Cartridge.h"
#include "MBC.h"

Cartridge::Cartridge(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: info {std::move(info)}, rom {std::move(romData)}
{}

Cartridge::~Cartridge()
{}

void Cartridge::LoadRAM(std::vector<uint8_t> ramData)
{
	ram = ramData;
}

std::unique_ptr<Cartridge> Cartridge::CreateCartridge(std::vector<uint8_t>&& romData)
{
	CartridgeInfo cartInfo = ParseCartridgeHeader(romData);
	
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
	std::cout << "Parsing... Raw ROM size: " << romData.size() << "\n";

	CartridgeInfo cartInfo;

	// Entry point 0100-0103
	// Nintendo Logo 0104-0133
	
	// Title
	// These bytes contain the title of the game in upper case ASCII. If the title is less than 16 characters long, the remaining bytes should be padded with $00s.
	const char* rawTitle = reinterpret_cast<const char*>(romData.data() + CartridgeHeaderAddresses::TITLE.start);
	std::string tempTitle(rawTitle, strnlen(rawTitle, CartridgeHeaderAddresses::TITLE.size()));
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
	cartInfo.ramSizeType = static_cast<RamSize>(romData[CartridgeHeaderAddresses::RAM_SIZE.start]);
	cartInfo.ramSizeBytes = GetRamSizeBytes(cartInfo.ramSizeType);
	cartInfo.ramBanks = GetRamBanks(cartInfo.ramSizeType);

	cartInfo.hasRam = CartridgeHasRam(cartInfo.type);
	cartInfo.hasBattery = CartridgeHasBattery(cartInfo.type);
	cartInfo.hasTimer = CartridgeHasTimer(cartInfo.type);

	return cartInfo;
}


