#pragma once
#include <cstdint>


using Address = uint16_t;

struct AddressRange
{
	Address Size() const
	{
		return end - start + 1;
	}

	bool Contains(Address addr) const
	{
		return addr >= start && addr <= end;
	}

	static bool InRange(Address low, Address high, Address addr)
	{
		return addr >= low && addr <= high;
	}

	Address GetOffset(Address addr) const
	{
		ASSERT(Contains(addr), "Address %04X out of range [%04X, %04X]", addr, start, end);
		return addr - start;
	}

	Address start, end;
};

//Main Mapping
static constexpr AddressRange addrCart{ 0x0000, 0x7FFF };
static constexpr AddressRange addrVRAM{ 0x8000, 0x9FFF };
static constexpr AddressRange addrExtRAM{ 0xA000, 0xBFFF };
static constexpr AddressRange addrWRAM{ 0xC000, 0xDFFF };
static constexpr AddressRange addrEchoRAM{ 0xE000, 0xFDFF };	// Mirror to C000-DDFF
static constexpr AddressRange addrOAM{ 0xFE00, 0xFE9F };
static constexpr AddressRange addrUnused{ 0xFEA0, 0xFEFF };
static constexpr AddressRange addrIO{ 0xFF00, 0xFF7F };
static constexpr AddressRange addrHRAM{ 0xFF80, 0xFFFE };
static constexpr AddressRange addrIE{ 0xFFFF, 0xFFFF };


// Extra specific ranges/addresses within the main memory map
static constexpr AddressRange addrIO_LCD_Control { 0xFF40, 0xFF4B };
