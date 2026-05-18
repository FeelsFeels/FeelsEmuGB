#include "Timer.h"
#include "Bus.h"

void Timer::ResetRegisters()
{
	div = 0xAB00;
	tima = 0x00;
	tma = 0x00;
	tac = 0xF8;

	overflowDelay = 0;
}

void Timer::Tick(int cycles)
{
	bool timerEnabled = tac & 0x04;
	uint8_t clockSelect = tac & 0x03;
	int targetBit = targetTimerBit[clockSelect];

	while (cycles > 0)
	{
		//--cycles;
		//++div;
		cycles -= 4;
		div += 4;
		
		// Overflow delay BEFORE edge detection
		// This gives the interrupt the documented 4 T cycle delay.
		if (overflowDelay > 0)
		{
			overflowDelay -= 4;
			if (overflowDelay <= 0)
			{
				overflowDelay = 0;
				tima = tma;
				bus->RequestInterrupt(InterruptCode::TIMER);
			}
		}

		bool currentTargetBitState = (div & (1 << targetBit));

		// Gameboy timer bug: the falling edge detector is dependant on both timerEnabled and the actual timer bit
		bool fallingEdgeDetector = timerEnabled && currentTargetBitState;

		// ticks on falling edge. that is, previous state is 1, current state is 0.
		if (previousTargetBitState && !fallingEdgeDetector)
		{
			++tima;
			if (tima == 0)
			{
				overflowDelay = 4;
				//bus->RequestInterrupt(InterruptCode::TIMER);
			}
		}
		previousTargetBitState = fallingEdgeDetector;
	}
}

uint8_t Timer::Read(Address addr)
{
	switch (addr)
	{
	case 0xFF04: return div >> 8; break;
	case 0xFF05: return tima; break;
	case 0xFF06: return tma; break;
	case 0xFF07: return tac; break;
	default: return 0xFF; break;
	}

}

void Timer::Write(Address addr, uint8_t val)
{
	switch (addr)
	{
	case 0xFF04:
		div = 0;	// "Reset when executing stop instruction" handled in the STOP instruction in CPU
		break;
	case 0xFF05: 
		if (overflowDelay > 0) overflowDelay = 0;
		tima = val;
		break;
	case 0xFF06: 
		tma = val;
		break;
	case 0xFF07: 
		tac = val;
		std::cout << "Writing to TAC: " << (int)val << "\n";
		break;
	}
}


void Timer::SaveState(std::ofstream& out)
{
	GBWrite(out, div);
	GBWrite(out, tima);
	GBWrite(out, tma);
	GBWrite(out, tac);
	GBWrite(out, previousTargetBitState);
}

void Timer::LoadState(std::ifstream& in)
{
	GBRead(in, div);
	GBRead(in, tima);
	GBRead(in, tma);
	GBRead(in, tac);
	GBRead(in, previousTargetBitState);
}
