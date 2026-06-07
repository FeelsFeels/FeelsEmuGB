#include "Bus.h"
#include "Timer.h"
#include "CPU.h"
#include "PPU.h"
#include "APU.h"
#include "Joypad.h"


// TODO: Eliminate the if/else hell. Simplify it. Carve out ranges, then switch statement.

void Bus::RunBootRom()
{
	// Read boot rom, execute instructions
	// Init graphics and sound, scroll logo, compare logo, etc
	// Compare checksum...

	// Initialise hardware register values at 0x100
	// Give control back

	// For now, fake the boot rom. Just skip and set PC to 0x100.

	// Set hardware state
	//Write(0xFF05, 0x00); // TIMA
	//Write(0xFF06, 0x00); // TMA
	//Write(0xFF07, 0x00); // TAC
	//Write(0xFF10, 0x80); // NR10 (Audio)
	//Write(0xFF11, 0xBF); // NR11
	//Write(0xFF12, 0xF3); // NR12
	//Write(0xFF14, 0xBF); // NR14
	//Write(0xFF16, 0x3F); // NR21
	//Write(0xFF17, 0x00); // NR22
	//Write(0xFF19, 0xBF); // NR24
	//Write(0xFF1A, 0x7F); // NR30
	//Write(0xFF1B, 0xFF); // NR31
	//Write(0xFF1C, 0x9F); // NR32
	//Write(0xFF1E, 0xBF); // NR33
	//Write(0xFF20, 0xFF); // NR41
	//Write(0xFF21, 0x00); // NR42
	//Write(0xFF22, 0x00); // NR43
	//Write(0xFF23, 0xBF); // NR30
	//Write(0xFF24, 0x77); // NR50
	//Write(0xFF25, 0xF3); // NR51
	//Write(0xFF26, 0xF1); // NR52 (Audio On)
	//Write(0xFF40, 0x91); // LCDC (Screen On, BG On)
	//Write(0xFF42, 0x00); // SCY
	//Write(0xFF43, 0x00); // SCX
	//Write(0xFF45, 0x00); // LYC
	//Write(0xFF47, 0xFC); // BGP (Background Palette)
	//Write(0xFF48, 0xFF); // OBP0
	//Write(0xFF49, 0xFF); // OBP1
	//Write(0xFF4A, 0x00); // WY
	//Write(0xFF4B, 0x00); // WX
	//Write(0xFFFF, 0x00); // IE
	
	wram.fill(0);
	hram.fill(0);
	io.fill(0);

	io[0x4F] = 0xFE;
	io[0x70] = 0x01;
	

	hdmaActive = false;
	hdmaHblankActive = false;
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
		return ppu->Read(addr);
		return 0xFF;
	}
	else if (addrExtRAM.Contains(addr))
	{
		return cartridge->Read(addr); // MBC inside cartridge handles read
	}
	else if (addrWRAMBank0.Contains(addr))
	{
		return wram[addrWRAM.GetOffset(addr)];
	}
	else if (addrWRAMSwitchable.Contains(addr))
	{
		if (cgbMode)
		{
			uint8_t selectedBank = io[0x70] & 0x07;
			if (selectedBank == 0)
				selectedBank = 1;

			Address offset = (selectedBank * 4096) + (addr - 0xD000);
			return wram[offset];
		}
		else
		{
			return wram[addrWRAM.GetOffset(addr)];
		}
	}
	else if (addrEchoRAM.Contains(addr))
	{
		// 0xE000, 0xFDFF mirrors to C000-DDFF
		//ASSERT(false, "Who is the game dev? Go Fuck yourself. Correction: 'Nintendo prohibits devs from using this memory range', so why the fuck does zelda read here?");
		return Read(addr - 0x2000);
		
	}
	else if (addrOAM.Contains(addr))
	{
		return ppu->Read(addr);
	}
	else if (addrUnused.Contains(addr))
	{
		return 0xFF;
	}
	else if (addrIO.Contains(addr))
	{
		return ReadIO(addr);
	}
	else if (addrHRAM.Contains(addr))
	{
		return hram[addrHRAM.GetOffset(addr)];
	}
	else if (addrIE.Contains(addr))
	{
		return cpu->GetIE();
	}

	ASSERT(false, "Unimplemented address range: %04X", addr);
	return 0xFF;
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
		ppu->Write(addr, data);
	}
	else if (addrExtRAM.Contains(addr))
	{
		cartridge->Write(addr, data);
	}
	else if (addrWRAMBank0.Contains(addr))
	{
		// Bank 0 same for both CGB and DMG
		wram[addrWRAM.GetOffset(addr)] = data;
	}
	else if (addrWRAMSwitchable.Contains(addr))
	{
		if (cgbMode)
		{
			uint8_t selectedBank = io[0x70] & 0x07;
			if (selectedBank == 0)
				selectedBank = 1;

			Address offset = (selectedBank * 4096) + (addr - 0xD000);
			wram[offset] = data;
		}
		else
		{
			wram[addrWRAM.GetOffset(addr)] = data;
		}
	}
	else if (addrEchoRAM.Contains(addr))
	{
		Write(addr - 0x2000, data);
	}
	else if (addrOAM.Contains(addr))
	{
		ppu->Write(addr, data);
	}
	else if (addrUnused.Contains(addr))
	{
		// Do no thing, or crash, haha
	}
	else if (addrIO.Contains(addr))
	{
		WriteIO(addr, data);
	}
	else if (addrHRAM.Contains(addr))
	{
		hram[addrHRAM.GetOffset(addr)] = data;
	}
	else if (addrIE.Contains(addr))
	{
		cpu->SetIE(data);
	}
}

uint8_t Bus::ReadIO(Address addr)
{
	uint8_t offset = addr & 0xFF;
	switch (offset)
	{
	case GB_IO_JOYP:
		return joypad->GetInput();

	case GB_IO_SB:
		return io[offset];
	case GB_IO_SC:
		// TODO: Serial transfer
		return io[offset] | 0xFF;
		return io[offset] | 0x7C;
	
	//0xFF03
	
	case GB_IO_DIV:
	case GB_IO_TIMA:
	case GB_IO_TMA:
	case GB_IO_TAC:
		return timer->Read(addr);
	
	case GB_IO_IF:
		return cpu->GetIF();

	// Audio register read code in default case

	case GB_IO_DMA:
		return io[offset];

	case GB_IO_LCDC:
	case GB_IO_STAT:
	case GB_IO_SCY:
	case GB_IO_SCX:
	case GB_IO_LY:
	case GB_IO_LYC:
	case GB_IO_BGP:
	case GB_IO_OBP0:
	case GB_IO_OBP1:
	case GB_IO_WY:
	case GB_IO_WX:
		return ppu->Read(addr);

	case GB_IO_KEY0:
		// TODO: KEY0 CPU mode select
		//https://gbdev.io/pandocs/CGB_Registers.html#ff4c--key0sys-cgb-mode-only-cpu-mode-select
		return 0xFF;
	case GB_IO_KEY1:
		if (cgbMode)
			return (((uint8_t)cpu->doubleSpeedMode << 7) | (uint8_t)cpu->preparingSpeedSwitch);
		else
			return 0xFF;

	case GB_IO_VBK:
		if (cgbMode)
			return io[GB_IO_VBK] | 0xFE;
		else
			return 0xFF;

	case GB_IO_BANK:
		// TODO: Boot rom 
		// return 0xFE | gb->boot_rom_finished;
		//return !bootRomEnabled | 0xFE;
		return !bootRomEnabled | 0xFF;

	// HDMA1234 Write only
	case GB_IO_HDMA5:
		if (cgbMode)
			return io[offset];
		else
			return 0xFF;

	case GB_IO_RP:
		// TODO: Infrared communications port
		if (cgbMode)
			return io[offset];
		else
			return 0xFF;

	case GB_IO_BGPI:
	case GB_IO_BGPD:
	case GB_IO_OBPI:
	case GB_IO_OBPD:
	case GB_IO_OPRI:
		if (cgbMode)
			return ppu->Read(addr);
		else
			return 0xFF;

	case GB_IO_SVBK:
	case GB_IO_PSM:
	case GB_IO_PSWX:
	case GB_IO_PSWY:
	case GB_IO_PSW:
	case GB_IO_PGB:
	case GB_IO_PCM12:
	case GB_IO_PCM34:
		// Have zero clue what these "undocumented" registers are
		return 0xFF;
		//return io[offset];

	default:
		// Audio
		if (addrAudioRegisters.Contains(addr))
		{
			return apu->Read(addr);
		}
		return 0xFF;
	}

	return 0xFF;
}

void Bus::WriteIO(Address addr, uint8_t data)
{
	uint8_t offset = addr & 0xFF;

	switch (offset)
	{
	case GB_IO_JOYP:
		joypad->Write(data);
		break;

	case GB_IO_SB:
		io[offset] = data;
		break;

	case GB_IO_SC:
		std::cout << "Writing to serial transfer 0xFF02: " << std::hex << (int)data << "\n";
		io[offset] = data;
		break;

	case GB_IO_DIV:
	case GB_IO_TIMA:
	case GB_IO_TMA:
	case GB_IO_TAC:
		timer->Write(addr, data);
		break;

	case GB_IO_IF:
		cpu->SetIF(data);
		break;

		// Audio registers handled in default

	case GB_IO_LCDC:
	case GB_IO_STAT:
	case GB_IO_SCY:
	case GB_IO_SCX:
	case GB_IO_LY:
	case GB_IO_LYC:
	case GB_IO_BGP:
	case GB_IO_OBP0:
	case GB_IO_OBP1:
	case GB_IO_WY:
	case GB_IO_WX:
		ppu->Write(addr, data);
		break;

	case GB_IO_DMA:
		DMATransfer(data);
		break;

	case GB_IO_KEY0:
		// TODO: KEY0 CPU mode select
		io[offset] = data;
		break;

	case GB_IO_KEY1:
	{
		uint8_t armed = data & 0x01;
		io[offset] |= armed;
		cpu->PrepareSpeedSwitch(armed);
		break;
	}

	case GB_IO_VBK:
		if (cgbMode)
			ppu->ChangeVramBank(data & 0x01);
	
		io[offset] = data;
		break;

	case GB_IO_BANK:
		// TODO: Boot rom 
		//bootRomEnabled |= data & 1;
		bootRomEnabled = false;
		break;

	case GB_IO_HDMA1:
	case GB_IO_HDMA2:
	case GB_IO_HDMA3:
	case GB_IO_HDMA4:
		io[offset] = data;
		break;

	case GB_IO_HDMA5:
		io[offset] = data;
		if (cgbMode)
		{
			if (hdmaHblankActive && !(data & 0x80))
				hdmaHblankActive = false;
			else
				HDMATransfer(data);
		}
		break;

	case GB_IO_RP:
		// TODO: Infrared communications port
		io[offset] = data;
		break;

	case GB_IO_BGPI:
	case GB_IO_BGPD:
	case GB_IO_OBPI:
	case GB_IO_OBPD:
	case GB_IO_OPRI:
		ppu->Write(addr, data);
		break;

	case GB_IO_SVBK:
		io[0x70] = data & 0xFF;
		break;

	case GB_IO_PSM:
	case GB_IO_PSWX:
	case GB_IO_PSWY:
	case GB_IO_PSW:
	case GB_IO_PGB:
	case GB_IO_PCM12:
	case GB_IO_PCM34:
		io[offset] = data;
		break;

	default:
		if (addrAudioRegisters.Contains(addr))
			apu->Write(addr, data);
		else
			io[offset] = data;
		break;
	}
}


void Bus::DMATransfer(uint8_t data)
{
	// Source: $XX00-$XX9F   ;XX = $00 to $DF
	// Destination: $FE00-$FE9F
	// In other words, 0x100 bytes from source to dest. 

	// TODO: Burn 160 M-cycles. BUT in this period, CPU only can read from HRAM.
	
	uint16_t address = data << 8;	// Divided by 0x100

	for (int i = 0; i < 160; i++)
	{
		uint8_t val = Read(address + i);
		ppu->Write(0xFE00 + i, val);
	}
}

void Bus::HDMATransfer(uint8_t data)
{
	hdmaSrc = io[0x51] << 8 | (io[0x52] & 0xF0);
	hdmaDst = 0x8000 | ((io[0x53] & 0x1F) << 8) | (io[0x54] & 0xF0);
	hdmaLength = (io[0x55] & 0x7F) + 1;

	if (data & 0x80)
	{
		// Hblank dma
		hdmaHblankActive = true;
	}
	else
	{
		// General Purpose DMA
		// TODO: Burn cycles instead of copying all at once. Disable CPU but not the others.
		int bytesToCopy = (hdmaLength) * 0x10;
		for (int i = 0; i < bytesToCopy; ++i)
			Write(hdmaDst + i, Read(hdmaSrc + i));
		io[0x55] = 0xFF;

		// 2 Bytes per 4 T Cycles
		cpu->AddStalledCycles(bytesToCopy * 2);
	}
}


// Triggered inside PPU
void Bus::HBlankTransfer()
{
	if (!hdmaHblankActive)
		return;

	// Copy 0x10 bytes of data each Hblank
	for (int i = 0; i < 16; ++i)
		Write(hdmaDst + i, Read(hdmaSrc + i));

	hdmaSrc += 16;
	hdmaDst += 16;
	--hdmaLength;

	// Reading from Register FF55 returns the remianing length (divided by 0x10, minus 1)
	io[0x55] = hdmaLength & 0x7F;
	if (hdmaLength <= 0)
	{
		hdmaHblankActive = false;
		io[0x55] = 0xFF;
	}
	cpu->AddStalledCycles(32);
}


void Bus::RequestInterrupt(InterruptCode bit)
{
	cpu->RequestInterrupt(bit);
}

void Bus::SaveState(std::ofstream& out)
{
	GBWrite(out, wram);
	GBWrite(out, hram);
	GBWrite(out, io);
}

void Bus::LoadState(std::ifstream& in)
{
	GBRead(in, wram);
	GBRead(in, hram);
	GBRead(in, io);
}

/*

	if (addr == 0xFF02)
	{
		io[0x02] = data;
	}
	else if (addr == addrJoypad)
	{
		joypad->Write(data);
	}
	else if (addr == addrInterruptFlag)
	{
		cpu->SetIF(data);
		return;
	}
	else if (addrTimer.Contains(addr))
	{
		timer->Write(addr, data);
		return;
	}
	else if (addr == addrDMATransfer)
	{
		DMATransfer(data);
	}
	else if (addrIO_LCD_Control.Contains(addr))
	{
		ppu->Write(addr, data);
	}
	else if (addrAudioRegisters.Contains(addr))
	{
		apu->Write(addr, data);
	}
	else if (addr == addrBootRomEnable)
	{
		bootRomEnabled = false;
		return;
	}

	// CGB Stuff, I guess
	else if (addr == 0xFF4D)
	{
		uint8_t armed = data & 0x01;
		io[0x4D] |= armed;

		cpu->PrepareSpeedSwitch(armed);

	}
	else if (addr == 0xFF4F)
	{
		ppu->ChangeVramBank(data & 0x01);
		io[0x4F] = data & 0x01;
	}
	else if (addr == 0xFF55)
	{
		io[0x55] = data;
		if (cgbMode)
		{
			if (hdmaHblankActive && !(data & 0x80))
			{
				// Cancel HDMATransfer
				hdmaHblankActive = false;
			}
			else
				HDMATransfer(data);
		}
	}
	else if (addrCgbPalettes.Contains(addr))
	{
		ppu->Write(addr, data);
	}
	else if (addr == 0xFF6C)
	{
		ppu->Write(addr, data);
	}
	else if (addr == 0xFF70)
	{
		// WRAM bank
		io[0x70] = data & 0xFF;
	}
	else // Fallback for unimplemented io in this range
	{
		io[addr & 0x7F] = data;
	}

*/