#include "Cartridge.h"


class NoMBC : public Cartridge
{
public:
	NoMBC(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~NoMBC();

	uint8_t Read(Address address) override;
	void Write(Address address, uint8_t val) override;
};

class MBC1 : public Cartridge
{
public:
	MBC1(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~MBC1();

	uint8_t Read(Address address) override;
	void Write(Address address, uint8_t val) override;

private:
	bool romEnabled = false;


};

class MBC2 : public Cartridge
{
public:
	MBC2(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~MBC2();

	uint8_t Read(Address address) override;
	void Write(Address address, uint8_t val) override;
};

class MBC3 : public Cartridge
{
public:
	MBC3(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~MBC3();

	uint8_t Read(Address address) override;
	void Write(Address address, uint8_t val) override;
};

class MBC5 : public Cartridge
{
public:
	MBC5(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~MBC5();

	uint8_t Read(Address address) override;
	void Write(Address address, uint8_t val) override;
};



class InvalidMBC : public Cartridge
{
public:
	InvalidMBC(CartridgeInfo&& info, std::vector<uint8_t>&& romData);
	~InvalidMBC();

	uint8_t Read(Address address) override;
	void Write(Address address, uint8_t val) override;
};