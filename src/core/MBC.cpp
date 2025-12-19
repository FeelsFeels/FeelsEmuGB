#include "MBC.h"


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
{}
MBC1::~MBC1()
{}
uint8_t MBC1::Read(Address address)
{
	if (address < 0x4000) return rom[address];
	if (address < 0x8000) return rom[address];
}

void MBC1::Write(Address address, uint8_t val)
{
	if (address < 0x8000)
	{

		return;  // Silently ignore
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
