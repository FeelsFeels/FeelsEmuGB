#pragma once
#include "Address.h"

class Bus;

class Timer
{
public:

	void Tick(int cycles);

	uint8_t Read(Address addr);
	void Write(Address addr, uint8_t val);

	void AttachBus(Bus* p) { bus = p; }

private:
	Bus* bus;

	uint16_t div { 0 };	// Divider. Always increments at a rate of rate of 16384Hz
	uint8_t tima{ 0 };	// Timer counter. Increments at TAC frequency
	uint8_t tma { 0 };	// 
	uint8_t tac { 0 };

	bool previousTargetBitState{ false };

	// use tac & 0x03 to index into this
	static constexpr int targetTimerBit[] = { 9,3,5,7 };
};

// TAC Modes and the position the bit has to tick for tima to be incremented.
//        0 |3 2 |1 
// 0000|0000|0000|0000

//switch (clockSelect)
//{
//case 0b00:
//	return 4194; // 256 m cycles (or x4 for t-cycles)
//case 0b01:
//	return 268400; // 4 m cycles
//case 0b10:
//	return 67110; // 16 m cycles
//case 0b11:
//	return 16780; // 64 m cycles
//}