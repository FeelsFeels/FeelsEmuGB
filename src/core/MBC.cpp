#include "MBC.h"

//TODO: MBC1 - proper ROM Banking

InvalidMBC::InvalidMBC(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}
InvalidMBC::~InvalidMBC()
{}

uint8_t InvalidMBC::Read(Address address)
{
	return 0xFF;
}

void InvalidMBC::Write(Address address, uint8_t val)
{
	
}



NoMBC::NoMBC(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}

NoMBC::~NoMBC()
{}

uint8_t NoMBC::Read(Address address)
{
	return rom[address];
}

void NoMBC::Write(Address address, uint8_t val)
{
	// No op
}



MBC1::MBC1(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{
	romBankMask = this->info.romBanks - 1;
}

MBC1::~MBC1()
{}

uint8_t MBC1::Read(Address address)
{
	if (addrROMBank0.Contains(address))
	{
		if (bankingMode == 0)
		{
			return rom[address];
		}
		else
		{
			// Bank 0 address being remapped.
			uint8_t targetBank = (bankHigh << 5);
			targetBank &= romBankMask;
			uint32_t offset = (targetBank * info.romBankSize) + address;

			return (offset < rom.size()) ? rom[offset] : 0xFF;
		}
	}
	else if (addrROMBankSwitchable.Contains(address))
	{
		uint8_t bankLowTranslation = bankLow;
		bankLowTranslation += bankLowTranslation == 0 ? 1 : 0; // QUIRK: Bank 0 becomes Bank 1

		// Offset = (Bank Number * Bank size) + (Address - start of addr region)
		uint8_t targetBank = (bankHigh << 5) | bankLowTranslation;
		targetBank &= romBankMask;
		uint32_t offset = (targetBank * info.romBankSize) + (address - addrROMBankSwitchable.start);

		return (offset < rom.size()) ? rom[offset] : 0xFF;
	}
	else if (addrExtRAM.Contains(address))
	{
		if (!ramEnabled) return 0xFF;

		uint8_t targetRamBank = 0;
		if (bankingMode == 1)
		{
			targetRamBank = bankHigh & 0x03;
		}
		else
		{
			targetRamBank = 0;
		}

		uint32_t offset = (targetRamBank * info.ramBankSize) + (address - addrExtRAM.start);
		return (offset < ram.size()) ? ram[offset] : 0xFF;
	}

}

void MBC1::Write(Address address, uint8_t val)
{
	if (addrRAMEnable.Contains(address))
	{
		ramEnabled = ((val & 0x0F) == 0x0A);
	}
	else if (addrBankSelector.Contains(address))
	{
		// Primary bank selector. only lower 5 bits matter.
		bankLow = val & 0x1F;
		//if (bankLow == 0) bankLow = 1; // QUIRK: Bank 0 becomes Bank 1
	}
	else if (addrBankSelector2.Contains(address))
	{
		// Secondary bank selector.
		// Banking mode 0 - Regular rom banking mode.
		// Banking mode 1 - Ram mode. Rom Bank 0 can now be switched. Ram Banks can be switched.
		bankHigh = val & 0x03;
	}
	else if (addrBankingMode.Contains(address))
	{
		bankingMode = val & 0x01;
	}
	else if (addrExtRAM.Contains(address))
	{
		if (ramEnabled)
		{
			uint8_t targetRamBank = (bankingMode == 1) ? bankHigh : 0;
			uint32_t offset = (targetRamBank * info.ramBankSize) + (address - addrExtRAM.start);
			if (offset < ram.size())
			{
				ram[offset] = val;
			}
		}
	}
}



MBC2::MBC2(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}

MBC2::~MBC2()
{}

uint8_t MBC2::Read(Address address)
{
	return rom[address];
}

void MBC2::Write(Address address, uint8_t val)
{

}



MBC3::MBC3(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}

MBC3::~MBC3()
{}

uint8_t MBC3::Read(Address address)
{
	return rom[address];
}

void MBC3::Write(Address address, uint8_t val)
{

}



MBC5::MBC5(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}

MBC5::~MBC5()
{}

uint8_t MBC5::Read(Address address)
{
	return rom[address];
}

void MBC5::Write(Address address, uint8_t val)
{

}
