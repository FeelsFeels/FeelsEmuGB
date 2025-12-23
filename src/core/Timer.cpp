#include "Timer.h"
#include "Bus.h"

void Timer::Tick(int cycles)
{
	bool timerEnabled = tac & 0x04;
	uint8_t clockSelect = tac & 0x03;
	int targetBit = targetTimerBit[clockSelect];

	while (cycles > 0)
	{
		--cycles;
		++div;

		bool currentTargetBitState = (div & (1 << targetBit));

		// Gameboy timer bug: the falling edge detector is dependant on both timerEnabled and the actual timer bit
		bool fallingEdgeDetector = timerEnabled && currentTargetBitState;

		// ticks on falling edge. that is, previous state is 1, current state is 0.
		if (previousTargetBitState && !fallingEdgeDetector)
		{
			++tima;
			if (tima == 0)
			{
				tima = tma;
				bus->RequestInterrupt(InterruptCode::TIMER);
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
		tima = val;
		break;
	case 0xFF06: 
		tma = val;
		break;
	case 0xFF07: 
		tac = val;
		break;
	}
}
