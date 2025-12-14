#include "MBC.h"


InvalidMBC::InvalidMBC(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}
InvalidMBC::~InvalidMBC()
{}

void InvalidMBC::Read(Address address)
{}

void InvalidMBC::Write(Address address)
{}



NoMBC::NoMBC(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}

NoMBC::~NoMBC()
{}

void NoMBC::Read(Address address)
{}

void NoMBC::Write(Address address)
{}



MBC1::MBC1(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}
MBC1::~MBC1()
{}
void MBC1::Read(Address address)
{}

void MBC1::Write(Address address)
{}



MBC2::MBC2(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}

MBC2::~MBC2()
{}

void MBC2::Read(Address address)
{}

void MBC2::Write(Address address)
{}



MBC3::MBC3(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}

MBC3::~MBC3()
{}

void MBC3::Read(Address address)
{}

void MBC3::Write(Address address)
{}



MBC5::MBC5(CartridgeInfo&& info, std::vector<uint8_t>&& romData)
	: Cartridge(std::move(info), std::move(romData))
{}

MBC5::~MBC5()
{}

void MBC5::Read(Address address)
{}

void MBC5::Write(Address address)
{}
