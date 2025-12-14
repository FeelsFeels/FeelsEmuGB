#pragma once
#include <cstdint>


using Address = uint16_t;

struct AddressRange
{
	AddressRange(Address start, Address end)
		: start{ start }, end{ end }
	{};

	Address size() const
	{
		return end - start + 1;
	}

	Address start, end;
};

//struct MemoryRegion
//{
//	Address address;
//	uint8_t* data;
//
//	bool contains(uint16_t addr) const
//	{
//		return addr >= address.start && addr <= address.end;
//	}
//
//	size_t offset(uint16_t addr) const
//	{
//		return addr - address.start;
//	}
//};