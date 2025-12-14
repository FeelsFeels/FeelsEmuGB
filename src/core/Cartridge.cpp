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
	CartridgeInfo cartInfo;

	// Entry point 0100-0103
	// Nintendo Logo 0104-0133
	
	// These bytes contain the title of the game in upper case ASCII. If the title is less than 16 characters long, the remaining bytes should be padded with $00s.
	auto titleBegin = reinterpret_cast<const char*>(romData.data() + CartridgeHeaderAddresses::TITLE.start);
	auto titleEnd = std::find(titleBegin, titleBegin + CartridgeHeaderAddresses::TITLE.size(), '\0');
	cartInfo.title.assign(titleBegin, titleEnd);
	
	cartInfo.type = static_cast<CartridgeType>(romData[CartridgeHeaderAddresses::CARTRIDGE_TYPE.start]);
	if (CartridgeTypeToString(cartInfo.type) == "UNKNOWN")
	{
		// warn or something
	}


	return cartInfo;
}


