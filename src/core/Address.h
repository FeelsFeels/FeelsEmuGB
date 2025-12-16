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

	Address start, end;
};

