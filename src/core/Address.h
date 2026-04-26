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

// Main Mapping
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

// Addresses within the IO range
static constexpr Address addrJoypad{ 0xFF00 };
static constexpr Address addrInterruptFlag{ 0xFF0F };
static constexpr AddressRange addrTimer{ 0xFF04, 0xFF07 };
static constexpr Address addrDMATransfer{ 0xFF46 };
static constexpr Address addrBootRomEnable{ 0xFF50 };


// APU Addresses (within IO range)
#pragma region APU
// Square 1
static constexpr Address addrNR10{ 0xFF10 };
static constexpr Address addrNR11{ 0xFF11 };
static constexpr Address addrNR12{ 0xFF12 };
static constexpr Address addrNR13{ 0xFF13 };
static constexpr Address addrNR14{ 0xFF14 };
static constexpr AddressRange addrSquare1{ 0xFF10, 0xFF14 };

// Square 2
static constexpr Address addrNR21{ 0xFF16 };
static constexpr Address addrNR22{ 0xFF17 };
static constexpr Address addrNR23{ 0xFF18 };
static constexpr Address addrNR24{ 0xFF19 };
static constexpr AddressRange addrSquare2{ 0xFF16, 0xFF19 };

// Wave
static constexpr Address addrNR30{ 0xFF1A };
static constexpr Address addrNR31{ 0xFF1B };
static constexpr Address addrNR32{ 0xFF1C };
static constexpr Address addrNR33{ 0xFF1D };
static constexpr Address addrNR34{ 0xFF1E };
static constexpr AddressRange addrWave{ 0xFF1A, 0xFF1E };
static constexpr AddressRange addrWavePattern{ 0xFF30, 0xFF3F };

// Noise
static constexpr Address addrNR41{ 0xFF20 };
static constexpr Address addrNR42{ 0xFF21 };
static constexpr Address addrNR43{ 0xFF22 };
static constexpr Address addrNR44{ 0xFF23 };
static constexpr AddressRange addrNoise{ 0xFF20, 0xFF23 };

// Control/Status
static constexpr Address addrNR50{ 0xFF24 };
static constexpr Address addrNR51{ 0xFF25 };
static constexpr Address addrNR52{ 0xFF26 };
static constexpr AddressRange addrAudioControl{ 0xFF24, 0xFF26 };

// Wave Table
static constexpr AddressRange addrWaveTable{ 0xFF30, 0xFF3F };

static constexpr AddressRange addrAudioRegisters{ 0xFF10, 0xFF3F };

#pragma endregion


// PPU Addresses (within IO)
#pragma region PPU
// TODOOOOOOO: put it in!
static constexpr Address addrLCDC{ 0xFF40 };
static constexpr Address addrSTAT{ 0xFF41 };

// Scrolling and Window
static constexpr Address addrSCY{ 0xFF42 };
static constexpr Address addrSCX{ 0xFF43 };
static constexpr Address addrLY{ 0xFF44 }; // Current Scanline
static constexpr Address addrLYC{ 0xFF45 }; // LY Compare
static constexpr Address addrWY{ 0xFF4A };
static constexpr Address addrWX{ 0xFF4B };

// DMA Transfer
static constexpr Address addrDMA{ 0xFF46 };

// Palettes
static constexpr Address addrBGP{ 0xFF47 };
static constexpr Address addrOBP0{ 0xFF48 };
static constexpr Address addrOBP1{ 0xFF49 };
#pragma endregion


// Extra specific ranges/addresses within the main memory map
static constexpr AddressRange addrIO_LCD_Control { 0xFF40, 0xFF4B };

// MBC address ranges
static constexpr AddressRange addrROMBank0		       { 0x0000, 0x3FFF };
static constexpr AddressRange addrROMBankSwitchable    { 0x4000, 0x7FFF };

static constexpr AddressRange addrRAMEnable		       { 0x0000, 0x1FFF };
static constexpr AddressRange addrBankSelector		   { 0x2000, 0x3FFF };
static constexpr AddressRange addrBankSelector2		   { 0x4000, 0x5FFF };
static constexpr AddressRange addrBankingMode		   { 0x6000, 0x7FFF };

//MBC3
static constexpr AddressRange addrRamBankSelector	   { 0x4000, 0x5FFF };
static constexpr AddressRange addrLatchClockData	   { 0x6000, 0x7FFF };
//static constexpr AddressRange addrRtcRegisters		   { 0x08, 0x0C };