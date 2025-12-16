#pragma once
#include "Cartridge.h"
#include "PPU.h"
#include "CPU.h"

#include <memory>
#include <string>


// Think of this as the motherboard
class Bus
{
public:
	Bus() = default;
	~Bus() = default;

	uint8_t Read(Address addr);
	void Write(Address addr, uint8_t data);
	
	void AttachCartridge(Cartridge* cart);
	void RemoveCartridge();

	void AttachCPU(CPU* p) { cpu = p; };
	void AttachPPU(PPU* p) { ppu = p; };

	void RunBootRom();

private:
	Cartridge* cartridge;
	CPU* cpu;
	PPU* ppu;

	bool cgbMode = false;
	bool bootRomEnabled = true;

	std::array<uint8_t, 8192> wram;


	static constexpr AddressRange addrCart		  { 0x0000, 0x7FFF };
	static constexpr AddressRange addrVRAM		  { 0x8000, 0x9FFF };
	static constexpr AddressRange addrExtRAM	  { 0xA000, 0xBFFF };
	static constexpr AddressRange addrWRAM		  { 0xC000, 0xDFFF };
	static constexpr AddressRange addrEchoRAM	  { 0xE000, 0xFDFF };	// Mirror to C000-DDFF
	static constexpr AddressRange addrOAM		  { 0xFE00, 0xFE9F };
	static constexpr AddressRange addrUnused	  { 0xFEA0, 0xFEFF };
	static constexpr AddressRange addrIO		  { 0xFF00, 0xFF7F };
	static constexpr AddressRange addrHRAM		  { 0xFF80, 0xFFFE };
	static constexpr AddressRange addrIE		  { 0xFFFF, 0xFFFF };
};