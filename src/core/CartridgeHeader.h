#pragma once
#include "Address.h"
#include <array>

namespace CartridgeHeaderAddresses
{
	const AddressRange ENTRY_POINT	   {0x0100, 0x0103};
	const AddressRange NINTENDO_LOGO   {0x0104, 0x0133};
	const AddressRange TITLE		   {0x0134, 0x0143};
	const AddressRange MANUFACTURER	   {0x013F, 0x0142}; // note overlap with TITLE
	const AddressRange CGB_FLAG		   {0x0143, 0x0143};	// same as above
	const AddressRange NEW_LICENSEE    {0x0144, 0x0145};
	const AddressRange SGB_FLAG		   {0x0146, 0x0146};
	const AddressRange CARTRIDGE_TYPE  {0x0147, 0x0147};
	const AddressRange ROM_SIZE		   {0x0148, 0x0148};
	const AddressRange RAM_SIZE		   {0x0149, 0x0149};
	const AddressRange DESTINATION	   {0x014A, 0x014A};
	const AddressRange OLD_LICENSEE	   {0x014B, 0x014B};
	const AddressRange MASK_ROM		   {0x014C, 0x014C};
	const AddressRange HEADER_CHECKSUM {0x014D, 0x014D};
	const AddressRange GLOBAL_CHECKSUM {0x014E, 0x014F};
}


#pragma region Cartridge
// Format:
// 1. Enum Name
// 2. Hex Value
// 3. Display String
// 4. Has External RAM?
// 5. Has Battery? (Need to create a .sav file)
// 6. Has Timer? (RTC support needed)

#define CARTRIDGE_TYPE_LIST \
    X(ROM_ONLY,                0x00, "ROM Only",                   false, false, false) \
    X(MBC1,                    0x01, "MBC1",                       false, false, false) \
    X(MBC1_RAM,                0x02, "MBC1 + RAM",                 true,  false, false) \
    X(MBC1_RAM_BATTERY,        0x03, "MBC1 + RAM + Battery",       true,  true,  false) \
    X(MBC2,                    0x05, "MBC2",                       true,  false, false) /* MBC2 includes built-in RAM */ \
    X(MBC2_BATTERY,            0x06, "MBC2 + Battery",             true,  true,  false) \
    X(ROM_RAM,                 0x08, "ROM + RAM",                  true,  false, false) \
    X(ROM_RAM_BATTERY,         0x09, "ROM + RAM + Battery",        true,  true,  false) \
    X(MMM01,                   0x0B, "MMM01",                      false, false, false) \
    X(MMM01_RAM,               0x0C, "MMM01 + RAM",                true,  false, false) \
    X(MMM01_RAM_BATTERY,       0x0D, "MMM01 + RAM + Battery",      true,  true,  false) \
    X(MBC3_TIMER_BATTERY,      0x0F, "MBC3 + Timer + Battery",     false, true,  true)  \
    X(MBC3_TIMER_RAM_BATTERY,  0x10, "MBC3 + Timer + RAM + Battery", true, true, true)  \
    X(MBC3,                    0x11, "MBC3",                       false, false, false) \
    X(MBC3_RAM,                0x12, "MBC3 + RAM",                 true,  false, false) \
    X(MBC3_RAM_BATTERY,        0x13, "MBC3 + RAM + Battery",       true,  true,  false) \
    X(MBC5,                    0x19, "MBC5",                       false, false, false) \
    X(MBC5_RAM,                0x1A, "MBC5 + RAM",                 true,  false, false) \
    X(MBC5_RAM_BATTERY,        0x1B, "MBC5 + RAM + Battery",       true,  true,  false) \
    X(MBC5_RUMBLE,             0x1C, "MBC5 + Rumble",              false, false, false) \
    X(MBC5_RUMBLE_RAM,         0x1D, "MBC5 + Rumble + RAM",        true,  false, false) \
    X(MBC5_RUMBLE_RAM_BATTERY, 0x1E, "MBC5 + Rumble + RAM + Battery", true, true, false) \
    X(MBC6,                    0x20, "MBC6",                       true,  true,  false) \
    X(MBC7_SENSOR_RUMBLE,      0x22, "MBC7 + Sensor + Rumble",     true,  true,  false) \
    X(POCKET_CAMERA,           0xFC, "Pocket Camera",              false, false, false) \
    X(BANDAI_TAMA5,            0xFD, "Bandai TAMA5",               false, false, false) \
    X(HUC3,                    0xFE, "HuC3",                       true,  true,  true)  \
    X(HUC1_RAM_BATTERY,        0xFF, "HuC1 + RAM + Battery",       true,  true,  false)

// -----------------------------------------------------------------------------
// Enum Class Generation
// -----------------------------------------------------------------------------
enum class CartridgeType : uint8_t
{
#define X(name, val, str, ram, bat, timer) name = val,
    CARTRIDGE_TYPE_LIST
#undef X
};

// -----------------------------------------------------------------------------
// Lookup Functions
// -----------------------------------------------------------------------------

constexpr const char* CartridgeTypeToString(CartridgeType type)
{
    switch (type)
    {
#define X(name, val, str, ram, bat, timer) case CartridgeType::name: return str;
        CARTRIDGE_TYPE_LIST
#undef X
    default: return "UNKNOWN";
    }
}

constexpr bool CartridgeHasRam(CartridgeType type)
{
    switch (type)
    {
#define X(name, val, str, ram, bat, timer) case CartridgeType::name: return ram;
        CARTRIDGE_TYPE_LIST
#undef X
    default: return false;
    }
}

constexpr bool CartridgeHasBattery(CartridgeType type)
{
    switch (type)
    {
#define X(name, val, str, ram, bat, timer) case CartridgeType::name: return bat;
        CARTRIDGE_TYPE_LIST
#undef X
    default: return false;
    }
}

constexpr bool CartridgeHasTimer(CartridgeType type)
{
    switch (type)
    {
#define X(name, val, str, ram, bat, timer) case CartridgeType::name: return timer;
        CARTRIDGE_TYPE_LIST
#undef X
    default: return false;
    }
}

//constexpr bool IsValidCartridgeType(CartridgeType type)
//{
//    switch (type)
//    {
//#define X(name, val, ...) case CartridgeType::name: return true;
//        CARTRIDGE_TYPE_LIST
//#undef X
//    default: return false;
//    }
//}
#pragma endregion

#pragma region Destination
#define DESTINATION_ENUM \
X(JAPAN,    "Japan") \
X(OVERSEAS, "Overseas")

enum Destination
{
#define X(name, str) name,
	DESTINATION_ENUM
#undef X
	DESTINATION_TOTAL
};

constexpr const char* DestinationTypeToString(Destination type)
{
    switch (type)
    {
#define X(name, str) case Destination::name: return str;
        DESTINATION_ENUM
#undef X
    default: return "UNKNOWN";
    }
}
#pragma endregion

#pragma region ROM Size
// Format: X(EnumName, HexValue, SizeInBytes, BankCount, Label)
#define ROM_SIZE_LIST \
    X(ROM_32KB,  0x00, 32768,   2,   "32 KB") \
    X(ROM_64KB,  0x01, 65536,   4,   "64 KB") \
    X(ROM_128KB, 0x02, 131072,  8,   "128 KB") \
    X(ROM_256KB, 0x03, 262144,  16,  "256 KB") \
    X(ROM_512KB, 0x04, 524288,  32,  "512 KB") \
    X(ROM_1MB,   0x05, 1048576, 64,  "1 MB") \
    X(ROM_2MB,   0x06, 2097152, 128, "2 MB") \
    X(ROM_4MB,   0x07, 4194304, 256, "4 MB") \
    X(ROM_8MB,   0x08, 8388608, 512, "8 MB") \
    X(ROM_1_1MB, 0x52, 1179648, 72,  "1.1 MB") \
    X(ROM_1_2MB, 0x53, 1310720, 80,  "1.2 MB") \
    X(ROM_1_5MB, 0x54, 1572864, 96,  "1.5 MB")

enum class RomSize : uint8_t
{
#define X(name, val, size, banks, str) name = val,
    ROM_SIZE_LIST
#undef X
};

// Helper: Get Byte Count
constexpr uint32_t GetRomSizeBytes(RomSize size)
{
    switch (size)
    {
#define X(name, val, size, banks, str) case RomSize::name: return size;
        ROM_SIZE_LIST
#undef X
    default: return 32768; // Default to 32KB
    }
}

// Helper: Get Bank Count
constexpr uint16_t GetRomBanks(RomSize size)
{
    switch (size)
    {
#define X(name, val, size, banks, str) case RomSize::name: return banks;
        ROM_SIZE_LIST
#undef X
    default: return 2;
    }
}

// Helper: Get String
constexpr const char* GetRomSizeString(RomSize size)
{
    switch (size)
    {
#define X(name, val, size, banks, str) case RomSize::name: return str;
        ROM_SIZE_LIST
#undef X
    default: return "Unknown";
    }
}
#pragma endregion

#pragma region RAM Size
// Format: X(EnumName, HexValue, SizeInBytes, BankCount, Label)
#define RAM_SIZE_LIST \
    X(NO_RAM,    0x00, 0,      0, "None") \
    X(RAM_2KB,   0x01, 2048,   1, "2 KB") \
    X(RAM_8KB,   0x02, 8192,   1, "8 KB") \
    X(RAM_32KB,  0x03, 32768,  4, "32 KB") \
    X(RAM_128KB, 0x04, 131072, 16, "128 KB") \
    X(RAM_64KB,  0x05, 65536,  8, "64 KB")

enum class RamSize : uint8_t
{
#define X(name, val, size, banks, str) name = val,
    RAM_SIZE_LIST
#undef X
};

constexpr uint32_t GetRamSizeBytes(RamSize size)
{
    switch (size)
    {
#define X(name, val, size, banks, str) case RamSize::name: return size;
        RAM_SIZE_LIST
#undef X
    default: return 0;
    }
}

constexpr uint8_t GetRamBanks(RamSize size)
{
    switch (size)
    {
#define X(name, val, size, banks, str) case RamSize::name: return banks;
        RAM_SIZE_LIST
#undef X
    default: return 0;
    }
}

constexpr const char* GetRamSizeString(RamSize size)
{
    switch (size)
    {
#define X(name, val, size, banks, str) case RamSize::name: return str;
        RAM_SIZE_LIST
#undef X
    default: return "None";
    }
}
#pragma endregion



struct CartridgeInfo
{
    std::string title;            // 0x0134 - 0x0143
    CartridgeType type;           // 0x0147
    std::string cartTypeString;

    // ROM Logic
    RomSize romSizeType;          // 0x0148 (The Enum)
    uint32_t romSizeBytes;        // Calculated
    uint16_t romBanks;            // Calculated

    // RAM Logic
    RamSize ramSizeType;          // 0x0149 (The Enum)
    uint32_t ramSizeBytes;        // Calculated
    uint8_t ramBanks;             // Calculated

    // Feature Flags
    bool hasRam;
    bool hasBattery;
    bool hasTimer;
    //bool hasRumble;


    // Nice to have. Might support next time.
    /*
    bool cgbFlag;                 // 0x0143 (True if Color GB supported)
    bool sgbFlag;                 // 0x0146 (True if Super GB supported)
    Destination destination;      // 0x014A (Japan vs Overseas)
    uint8_t version;              // 0x014C (Mask ROM version)

    uint8_t headerChecksum;       // 0x014D
    bool headerChecksumValid;     // Calculated. If false, real GB locks up.

    std::string licensee;         // 0x0144/0x014B
    */
};
