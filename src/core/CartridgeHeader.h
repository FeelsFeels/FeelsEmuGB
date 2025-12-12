#pragma once
#include "Address.h"
#include <array>

#if 0

#define MATERIAL_TEXTURE_INDEX_ENUM \
X(BASE_COLOR_TEXTURE, "Base Color") \
X(METALLIC_ROUGHNESS_TEXTURE, "Metallic Roughness") \
X(NORMAL_TEXTURE, "Normal Map") \
X(EMISSIVE_TEXTURE, "Emissive") \
X(OCCLUSION_TEXTURE, "Occlusion")

#define X(type, str) type,
enum class MATERIAL_TEXTURE_INDEX
{
	MATERIAL_TEXTURE_INDEX_ENUM
	TOTAL
};
#undef X

#define X(type, str) str,
std::array<const char*, +MATERIAL_TEXTURE_INDEX::TOTAL> textureSlotNames{
	MATERIAL_TEXTURE_INDEX_ENUM
};
#undef X

#endif


namespace CartridgeHeaderAddresses
{
	const Address ENTRY_POINT	  {0x0100, 0x0103};
	const Address NINTENDO_LOGO	  {0x0104, 0x0133};
	const Address TITLE			  {0x0134, 0x0143};
	const Address MANUFACTURER	  {0x013F, 0x0142}; // note overlap with TITLE
	const Address CGB_FLAG		  {0x0143, 0x0143};	// same as above
	const Address NEW_LICENSEE    {0x0144, 0x0145};
	const Address SGB_FLAG		  {0x0146, 0x0146};
	const Address CARTRIDGE_TYPE  {0x0147, 0x0147};
	const Address ROM_SIZE		  {0x0148, 0x0148};
	const Address RAM_SIZE		  {0x0149, 0x0149};
	const Address DESTINATION	  {0x014A, 0x014A};
	const Address OLD_LICENSEE	  {0x014B, 0x014B};
	const Address MASK_ROM		  {0x014C, 0x014C};
	const Address HEADER_CHECKSUM {0x014D, 0x014D};
	const Address GLOBAL_CHECKSUM {0x014E, 0x014F};
}

#define CARTRIDGE_TYPE_ENUM \
X(ROMOnly, "ROM Only") \
X(MBC1, "MBC1") \
X(MBC2, "MBC2") \
X(MBC3, "MBC3") \
X(MBC5, "MBC5") \
X(MBC6, "MBC6") \
X(MBC7, "MBC7") \
X(MMM01, "MMM01") \
X(M161, "M161") \
X(HuC1, "HuC-1") \
X(HuC_3, "HuC-3") \
X(UNKNOWN, "Unknown")

enum CartridgeType
{
#define X(name, str) name,
	CARTRIDGE_TYPE_ENUM
#undef X
	CARTRIDGE_TYPE_TOTAL
};

static constexpr std::array<const char*, +CartridgeType::CARTRIDGE_TYPE_TOTAL> CartridgeTypeNames[] =
{
#define X(name, str) str,
	CARTRIDGE_TYPE_ENUM
#undef X
};

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
static constexpr std::array<const char*, +Destination::DESTINATION_TOTAL> DestinationNames[] = {
#define X(name, str) str,
	DESTINATION_ENUM
#undef X
};


struct CartridgeInfo
{
	std::string title;

	CartridgeType type;
	Destination destination;

	// rom size
	// ram size
	// license code
	// header checksum
	// global checksum
	// mask rom seems useless
	// cgb flag
	// sgb flag
};
