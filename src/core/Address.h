#pragma once
#include <cstdint>

struct Address
{
	Address(uint16_t start, uint16_t end)
		: start{ start }, end{ end }
	{};

	uint16_t size() const
	{
		return end - start + 1;
	}

	uint16_t start, end;
};

struct MemoryRegion
{
	Address address;
	uint8_t* data;

	bool contains(uint16_t addr) const
	{
		return addr >= address.start && addr <= address.end;
	}

	size_t offset(uint16_t addr) const
	{
		return addr - address.start;
	}
};