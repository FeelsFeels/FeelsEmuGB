#pragma once
#include "Register.h"


class Bus; //Forward decl

class CPU
{
public:
	void AttachBus(Bus* p) { bus = p; }

private:
	struct Instruction
	{
		void (CPU::*execute)();
		std::string name;
		uint8_t cycles;
	};

	Bus* bus;
	Registers reg;

	void ADD(uint8_t val);
	void SUB(uint8_t val);
	void ADD_HL(uint16_t val);




};