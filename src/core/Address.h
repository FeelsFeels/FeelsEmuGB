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
static constexpr AddressRange addrWRAMBank0{ 0xC000, 0xCFFF };
static constexpr AddressRange addrWRAMSwitchable { 0xD000, 0xDFFF };
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

// Thanks, Sameboy. 
enum IORegisters
{
    /* Joypad and Serial */
    GB_IO_JOYP = 0x00, // Joypad (R/W)
    GB_IO_SB = 0x01, // Serial transfer data (R/W)
    GB_IO_SC = 0x02, // Serial Transfer Control (R/W)

    /* Missing */

    /* Timers */
    GB_IO_DIV = 0x04, // Divider Register (R/W)
    GB_IO_TIMA = 0x05, // Timer counter (R/W)
    GB_IO_TMA = 0x06, // Timer Modulo (R/W)
    GB_IO_TAC = 0x07, // Timer Control (R/W)

    /* Missing */

    GB_IO_IF = 0x0F, // Interrupt Flag (R/W)

    /* Sound */
    GB_IO_NR10 = 0x10, // Channel 1 Sweep register (R/W)
    GB_IO_NR11 = 0x11, // Channel 1 Sound length/Wave pattern duty (R/W)
    GB_IO_NR12 = 0x12, // Channel 1 Volume Envelope (R/W)
    GB_IO_NR13 = 0x13, // Channel 1 Frequency lo (Write Only)
    GB_IO_NR14 = 0x14, // Channel 1 Frequency hi (R/W)
    /* NR20 does not exist */
    GB_IO_NR21 = 0x16, // Channel 2 Sound Length/Wave Pattern Duty (R/W)
    GB_IO_NR22 = 0x17, // Channel 2 Volume Envelope (R/W)
    GB_IO_NR23 = 0x18, // Channel 2 Frequency lo data (W)
    GB_IO_NR24 = 0x19, // Channel 2 Frequency hi data (R/W)
    GB_IO_NR30 = 0x1A, // Channel 3 Sound on/off (R/W)
    GB_IO_NR31 = 0x1B, // Channel 3 Sound Length
    GB_IO_NR32 = 0x1C, // Channel 3 Select output level (R/W)
    GB_IO_NR33 = 0x1D, // Channel 3 Frequency's lower data (W)
    GB_IO_NR34 = 0x1E, // Channel 3 Frequency's higher data (R/W)
    /* NR40 does not exist */
    GB_IO_NR41 = 0x20, // Channel 4 Sound Length (R/W)
    GB_IO_NR42 = 0x21, // Channel 4 Volume Envelope (R/W)
    GB_IO_NR43 = 0x22, // Channel 4 Polynomial Counter (R/W)
    GB_IO_NR44 = 0x23, // Channel 4 Counter/consecutive, Initial (R/W)
    GB_IO_NR50 = 0x24, // Channel control / ON-OFF / Volume (R/W)
    GB_IO_NR51 = 0x25, // Selection of Sound output terminal (R/W)
    GB_IO_NR52 = 0x26, // Sound on/off

    /* Missing */

    GB_IO_WAV_START = 0x30, // Wave pattern start
    GB_IO_WAV_END = 0x3F, // Wave pattern end

    /* Graphics */
    GB_IO_LCDC = 0x40, // LCD Control (R/W)
    GB_IO_STAT = 0x41, // LCDC Status (R/W)
    GB_IO_SCY = 0x42, // Scroll Y (R/W)
    GB_IO_SCX = 0x43, // Scroll X (R/W)
    GB_IO_LY = 0x44, // LCDC Y-Coordinate (R)
    GB_IO_LYC = 0x45, // LY Compare (R/W)
    GB_IO_DMA = 0x46, // DMA Transfer and Start Address (W)
    GB_IO_BGP = 0x47, // BG Palette Data (R/W) - Non CGB Mode Only
    GB_IO_OBP0 = 0x48, // Object Palette 0 Data (R/W) - Non CGB Mode Only
    GB_IO_OBP1 = 0x49, // Object Palette 1 Data (R/W) - Non CGB Mode Only
    GB_IO_WY = 0x4A, // Window Y Position (R/W)
    GB_IO_WX = 0x4B, // Window X Position minus 7 (R/W)
    // Controls DMG mode and PGB mode
    GB_IO_KEY0 = 0x4C,

    /* General CGB features */
    GB_IO_KEY1 = 0x4D, // CGB Mode Only - Prepare Speed Switch

    /* Missing */

    GB_IO_VBK = 0x4F, // CGB Mode Only - VRAM Bank
    GB_IO_BANK = 0x50, // Write to disable the boot ROM mapping

    /* CGB DMA */
    GB_IO_HDMA1 = 0x51, // CGB Mode Only - New DMA Source, High
    GB_IO_HDMA2 = 0x52, // CGB Mode Only - New DMA Source, Low
    GB_IO_HDMA3 = 0x53, // CGB Mode Only - New DMA Destination, High
    GB_IO_HDMA4 = 0x54, // CGB Mode Only - New DMA Destination, Low
    GB_IO_HDMA5 = 0x55, // CGB Mode Only - New DMA Length/Mode/Start

    /* IR */
    GB_IO_RP = 0x56, // CGB Mode Only - Infrared Communications Port

    /* Missing */

    /* CGB Palettes */
    GB_IO_BGPI = 0x68, // CGB Mode Only - Background Palette Index
    GB_IO_BGPD = 0x69, // CGB Mode Only - Background Palette Data
    GB_IO_OBPI = 0x6A, // CGB Mode Only - Object Palette Index
    GB_IO_OBPD = 0x6B, // CGB Mode Only - Object Palette Data
    GB_IO_OPRI = 0x6C, // Affects object priority (X based or index based)

    /* Missing */

    GB_IO_SVBK = 0x70, // CGB Mode Only - WRAM Bank
    GB_IO_PSM = 0x71, // Palette Selection Mode, controls the PSW and key combo
    GB_IO_PSWX = 0x72, // X position of the palette switching window
    GB_IO_PSWY = 0x73, // Y position of the palette switching window
    GB_IO_PSW = 0x74, // Key combo to trigger the palette switching window
    GB_IO_PGB = 0x75, // Bits 0-2 control PHI, A15 and ¬CS, respectively.  Bits 4-6 control the I/O directions of bits 0-2 (0 is R, 1 is W)
    GB_IO_PCM12 = 0x76, // Channels 1 and 2 amplitudes
    GB_IO_PCM34 = 0x77, // Channels 3 and 4 amplitudes
};





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
static constexpr AddressRange addrCgbPalettes{ 0xFF68, 0xFF6B };
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

// MBC3
static constexpr AddressRange addrRamBankSelector	   { 0x4000, 0x5FFF };
static constexpr AddressRange addrLatchClockData	   { 0x6000, 0x7FFF };

// MBC5
static constexpr AddressRange addrRomBankSelectorLSB   { 0x2000, 0x2FFF };
static constexpr AddressRange addrRomBankSelectorMSB   { 0x3000, 0x3FFF };
