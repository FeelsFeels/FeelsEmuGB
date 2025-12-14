#include "Cartridge.h"


class NoMBC : public Cartridge
{
public:
	NoMBC(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~NoMBC();

	void Read(Address address) override;
	void Write(Address address) override;
};

class MBC1 : public Cartridge
{
public:
	MBC1(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~MBC1();

	void Read(Address address) override;
	void Write(Address address) override;
};

class MBC2 : public Cartridge
{
public:
	MBC2(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~MBC2();

	void Read(Address address) override;
	void Write(Address address) override;
};

class MBC3 : public Cartridge
{
public:
	MBC3(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~MBC3();

	void Read(Address address) override;
	void Write(Address address) override;
};

class MBC5 : public Cartridge
{
public:
	MBC5(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~MBC5();

	void Read(Address address) override;
	void Write(Address address) override;
};



class InvalidMBC : public Cartridge
{
public:
	InvalidMBC(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~InvalidMBC();

	void Read(Address address) override;
	void Write(Address address) override;
};