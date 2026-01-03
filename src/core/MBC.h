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
	uint8_t bankLow = 1;       // Primary Rom Bank Selector - only bottom 5 bits matter
	uint8_t bankHigh = 0;      // Secondary Rom Bank Selector / Ram Selector - bottom 2 bits matter
	bool ramEnabled = false;  // Is RAM writable?
	uint8_t bankingMode = 0;  // 0 = ROM Mode, 1 = RAM Mode
	uint32_t romBankMask = 0; // See notes: if # banks needs less than 5 bits to represent, 5th bit is masked.
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