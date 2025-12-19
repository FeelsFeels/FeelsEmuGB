#include "Bus.h"

void Bus::AttachCartridge(Cartridge* cart)
{
	cartridge = cart;
}

void Bus::RemoveCartridge()
{
	cartridge = nullptr;
}

void Bus::RunBootRom()
{
	// Read boot rom, execute instructions
	// Init graphics and sound, scroll logo, compare logo, etc
	// Compare checksum...

	// Initialise hardware register values at 0x100
	// Give control back

	// For now, fake the boot rom. Just skip and set PC to 0x100.

	// Set hardware state
	Write(0xFF05, 0x00); // TIMA
	Write(0xFF06, 0x00); // TMA
	Write(0xFF07, 0x00); // TAC
	Write(0xFF10, 0x80); // NR10 (Audio)
	Write(0xFF11, 0xBF); // NR11
	Write(0xFF12, 0xF3); // NR12
	Write(0xFF14, 0xBF); // NR14
	Write(0xFF16, 0x3F); // NR21
	Write(0xFF17, 0x00); // NR22
	Write(0xFF19, 0xBF); // NR24
	Write(0xFF1A, 0x7F); // NR30
	Write(0xFF1B, 0xFF); // NR31
	Write(0xFF1C, 0x9F); // NR32
	Write(0xFF1E, 0xBF); // NR33
	Write(0xFF20, 0xFF); // NR41
	Write(0xFF21, 0x00); // NR42
	Write(0xFF22, 0x00); // NR43
	Write(0xFF23, 0xBF); // NR30
	Write(0xFF24, 0x77); // NR50
	Write(0xFF25, 0xF3); // NR51
	Write(0xFF26, 0xF1); // NR52 (Audio On)
	Write(0xFF40, 0x91); // LCDC (Screen On, BG On)
	Write(0xFF42, 0x00); // SCY
	Write(0xFF43, 0x00); // SCX
	Write(0xFF45, 0x00); // LYC
	Write(0xFF47, 0xFC); // BGP (Background Palette)
	Write(0xFF48, 0xFF); // OBP0
	Write(0xFF49, 0xFF); // OBP1
	Write(0xFF4A, 0x00); // WY
	Write(0xFF4B, 0x00); // WX
	Write(0xFFFF, 0x00); // IE
}

uint8_t Bus::Read(Address addr)
{
	ASSERT(cartridge, "No cartridge in MMU!");

	if (addrCart.Contains(addr))
	{
		return cartridge->Read(addr);
	}
	else if (addrVRAM.Contains(addr))
	{
		// return ppu.ReadVRAM(addr); // TODO: Connect PPU later
		return 0xFF;
	}
	else if (addrExtRAM.Contains(addr))
	{
		return cartridge->Read(addr); // MBC inside cartridge handles read
	}
	else if (addrWRAM.Contains(addr))
	{
		if (cgbMode && AddressRange::InRange(0xD000, 0xDFFF, addr))
		{
			// Switchable bank 1-7
			ASSERT(false, "CGB unsupported")
		}
		return wram[addr & 0x1FFF];
	}
	else if (addrEchoRAM.Contains(addr))
	{
		return wram[addr & 0x1FFF];
	}
	else if (addrOAM.Contains(addr))
	{
		// return ppu.ReadOAM(addr); // TODO: Connect PPU later
		return 0xFF;
	}
	else if (addrUnused.Contains(addr))
	{
		return 0xFF;
	}
	else if (addrIO.Contains(addr))
	{
		// TODO: Redirect to Timer, Audio, PPU, etc.
		return io[addr & 0x7F];
	}
	else if (addrHRAM.Contains(addr))
	{
		return hram[addr & 0x7F];
	}
	else if (addrIE.Contains(addr))
	{
		return io[0x7F];
	}

	return 0xFF;

	// ASSERT(false, "Unimplemented address range: " + std::to_string(addr));
}

void Bus::Write(Address addr, uint8_t data)
{
	ASSERT(cartridge, "No cartridge in MMU!");


	if (addrCart.Contains(addr))
	{
		cartridge->Write(addr, data);
	}
	else if (addrVRAM.Contains(addr))
	{
		// ppu.WriteVRAM(addr, data);
	}
	else if (addrExtRAM.Contains(addr))
	{
		cartridge->Write(addr, data);
	}
	else if (addrWRAM.Contains(addr))
	{
		wram[addr & 0x1FFF] = data;
	}
	else if (addrEchoRAM.Contains(addr))
	{
		wram[addr & 0x1FFF] = data;
	}
	else if (addrOAM.Contains(addr))
	{
		// ppu.WriteOAM(addr, data);
	}
	else if (addrUnused.Contains(addr))
	{
		// Do no thing
	}
	else if (addrIO.Contains(addr))
	{
		// --- TEST ROM OUTPUT STUB ---
		// Blargg's test ROMs write characters to 0xFF01
		if (addr == 0xFF01)
		{
			printf("%c", data);
		}
		// ----------------------------

		io[addr & 0x7F] = data;
		if (addr == 0xFF50)
		{
			bootRomEnabled = false;
			return;
		}
	}
	else if (addrHRAM.Contains(addr))
	{
		hram[addr & 0x7F] = data;
	}
	else if (addrIE.Contains(addr))
	{
		io[0x7F] = data;
	}


}

