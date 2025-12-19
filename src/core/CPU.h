#pragma once
#include "Register.h"
#include "Address.h"

#include <vector>

class Bus; //Forward decl

class CPU
{
public:
	CPU();

	void AttachBus(Bus* p) { bus = p; }
	void ResetRegisters();
	int Step();

private:
	//enum OperandType { REG_A, REG_B, IMM8, ADDR_HL, ... };
	//struct Instruction
	//{
	//	void (CPU::* op_func)(OperandType, OperandType);
	//	OperandType dst;
	//	OperandType src;
	//};
	friend class DebugInfo;

	struct Instruction
	{
		void (CPU::*execute)();
		std::string name;
		uint8_t cycles;
	};
	struct CBInstruction
	{
		void (CPU::* execute)();
		std::string name;
		uint8_t cycles;
	};
	void RegisterInstructions();
	std::vector<Instruction> instructions;
	std::vector<CBInstruction> cbInstructions;

	int totalCyclesForInstruction;
	uint8_t lastInstruction;	// honestly just for debug purposes;

	Bus* bus;
	Registers reg;

	// Internal CPU state
	bool ime = true;	  // Interrupt Master Enable. true being "Enables Interrupt Handling"
	bool imeNext = false; // When enabling ime (ei instruction), CPU waits until next cycle to enable.
	bool halted = false;  // If true, don't fetch opcodes. Wakes on any interrupt
	bool stopped = false; // Wakes on joypad interrupt.

	// Helpers for instructions
	uint8_t  FetchByte();	// from pc
	uint16_t FetchWord();
	uint8_t  PopByte();		// pop/push from stack (sp)
	uint16_t PopWord();
	void PushByte(uint8_t val);
	void PushWord(uint16_t val);
	
#pragma region Instructions
	void NOP();
	void INVALID();

	// 8 bit loads
	void LD_r8_n8(ByteRegister& dst, uint8_t val);
	void LD_r8_r8(ByteRegister& dst, const ByteRegister& val);
	void LD_r8_addr(ByteRegister& dst, const WordRegister& addr);
	void LD_addr_r8(const WordRegister& addr, const ByteRegister& val);	// Write to address
	void LD_addr_SP(const WordRegister& addr);

	// 16 bit load
	void LD_r16_n16(WordRegister& dst, uint16_t value);
	void LD_HL_SP_e8(int8_t val);

	// Arithmetic
	void ADD(uint8_t val);
	void ADDC(uint8_t val);	//Add with Carry
	void SUB(uint8_t val);
	void SUBC(uint8_t val);
	void ADD_HL(uint16_t val);
	void ADD_SP_e8(int8_t val);

	void CMP(uint8_t val);

	void INC_r8(ByteRegister& dst);
	void INC_r16(WordRegister& dst);
	void DEC_r8(ByteRegister& dst);
	void DEC_r16(WordRegister& dst);

	// Bitwise
	void AND(uint8_t val);
	void OR(uint8_t val);
	void XOR(uint8_t val);
	void CPL();	// ComPLement. only can operate on A register

	// Carry Flag instructions
	void CCF();	// Complement carry flag
	void SCF();	// Set carry flag


	// Rotates and Shifts
	void RL(ByteRegister& dst);		// Rotate bits in r8 left, through the carry flag.
	void RLC(ByteRegister& dst);	// Rotate left, circular
	void RR(ByteRegister& dst);
	void RRC(ByteRegister& dst);
	void SLA(ByteRegister& dst);	// Shift Left Arithmetic - Sign bit stays same
	void SRA(ByteRegister& dst);
	void SRL(ByteRegister& dst);	// Shift Right Logical
	void SWAP(ByteRegister& dst);

	// Bit flags
	void BIT(uint8_t bit, const ByteRegister& value);
	void BIT_addr(uint8_t bit, const WordRegister& value);
	void SET(uint8_t bit, ByteRegister& reg);
	void RES(uint8_t bit, ByteRegister& reg);

	// Control flow
	void JP(const WordRegister& addr);
	void JR(int8_t addr);
	void CALL(Address addr);
	void RET();
	void RST(Address vector);

	// Interrupts
	void DI();	 //Disable
	void EI();	 //Enable
	void HALT();

	// Miscellaneous
	void DAA();	 //Decimal Adjust Accumulator
	void STOP();

#pragma endregion


// Note: Opcode 0xE8 is one of the special ones.
#pragma region Opcode Hell

#pragma region OP_00_0F
	void OP_00(); void OP_01(); void OP_02(); void OP_03();
	void OP_04(); void OP_05(); void OP_06(); void OP_07();
	void OP_08(); void OP_09(); void OP_0A(); void OP_0B();
	void OP_0C(); void OP_0D(); void OP_0E(); void OP_0F();
#pragma endregion

#pragma region OP_10_1F
	void OP_10(); void OP_11(); void OP_12(); void OP_13();
	void OP_14(); void OP_15(); void OP_16(); void OP_17();
	void OP_18(); void OP_19(); void OP_1A(); void OP_1B();
	void OP_1C(); void OP_1D(); void OP_1E(); void OP_1F();
#pragma endregion

#pragma region OP_20_2F
	void OP_20(); void OP_21(); void OP_22(); void OP_23();
	void OP_24(); void OP_25(); void OP_26(); void OP_27();
	void OP_28(); void OP_29(); void OP_2A(); void OP_2B();
	void OP_2C(); void OP_2D(); void OP_2E(); void OP_2F();
#pragma endregion

#pragma region OP_30_3F
	void OP_30(); void OP_31(); void OP_32(); void OP_33();
	void OP_34(); void OP_35(); void OP_36(); void OP_37();
	void OP_38(); void OP_39(); void OP_3A(); void OP_3B();
	void OP_3C(); void OP_3D(); void OP_3E(); void OP_3F();
#pragma endregion

#pragma region OP_40_4F
	void OP_40(); void OP_41(); void OP_42(); void OP_43();
	void OP_44(); void OP_45(); void OP_46(); void OP_47();
	void OP_48(); void OP_49(); void OP_4A(); void OP_4B();
	void OP_4C(); void OP_4D(); void OP_4E(); void OP_4F();
#pragma endregion

#pragma region OP_50_5F
	void OP_50(); void OP_51(); void OP_52(); void OP_53();
	void OP_54(); void OP_55(); void OP_56(); void OP_57();
	void OP_58(); void OP_59(); void OP_5A(); void OP_5B();
	void OP_5C(); void OP_5D(); void OP_5E(); void OP_5F();
#pragma endregion

#pragma region OP_60_6F
	void OP_60(); void OP_61(); void OP_62(); void OP_63();
	void OP_64(); void OP_65(); void OP_66(); void OP_67();
	void OP_68(); void OP_69(); void OP_6A(); void OP_6B();
	void OP_6C(); void OP_6D(); void OP_6E(); void OP_6F();
#pragma endregion

#pragma region OP_70_7F
	void OP_70(); void OP_71(); void OP_72(); void OP_73();
	void OP_74(); void OP_75(); void OP_76(); void OP_77();
	void OP_78(); void OP_79(); void OP_7A(); void OP_7B();
	void OP_7C(); void OP_7D(); void OP_7E(); void OP_7F();
#pragma endregion

#pragma region OP_80_8F
	void OP_80(); void OP_81(); void OP_82(); void OP_83();
	void OP_84(); void OP_85(); void OP_86(); void OP_87();
	void OP_88(); void OP_89(); void OP_8A(); void OP_8B();
	void OP_8C(); void OP_8D(); void OP_8E(); void OP_8F();
#pragma endregion

#pragma region OP_90_9F
	void OP_90(); void OP_91(); void OP_92(); void OP_93();
	void OP_94(); void OP_95(); void OP_96(); void OP_97();
	void OP_98(); void OP_99(); void OP_9A(); void OP_9B();
	void OP_9C(); void OP_9D(); void OP_9E(); void OP_9F();
#pragma endregion

#pragma region OP_A0_AF
	void OP_A0(); void OP_A1(); void OP_A2(); void OP_A3();
	void OP_A4(); void OP_A5(); void OP_A6(); void OP_A7();
	void OP_A8(); void OP_A9(); void OP_AA(); void OP_AB();
	void OP_AC(); void OP_AD(); void OP_AE(); void OP_AF();
#pragma endregion

#pragma region OP_B0_BF
	void OP_B0(); void OP_B1(); void OP_B2(); void OP_B3();
	void OP_B4(); void OP_B5(); void OP_B6(); void OP_B7();
	void OP_B8(); void OP_B9(); void OP_BA(); void OP_BB();
	void OP_BC(); void OP_BD(); void OP_BE(); void OP_BF();
#pragma endregion

#pragma region OP_C0_CF
	void OP_C0(); void OP_C1(); void OP_C2(); void OP_C3();
	void OP_C4(); void OP_C5(); void OP_C6(); void OP_C7();
	void OP_C8(); void OP_C9(); void OP_CA(); void OP_CB();
	void OP_CC(); void OP_CD(); void OP_CE(); void OP_CF();
#pragma endregion

#pragma region OP_D0_DF
	void OP_D0(); void OP_D1(); void OP_D2(); void OP_D3();
	void OP_D4(); void OP_D5(); void OP_D6(); void OP_D7();
	void OP_D8(); void OP_D9(); void OP_DA(); void OP_DB();
	void OP_DC(); void OP_DD(); void OP_DE(); void OP_DF();
#pragma endregion

#pragma region OP_E0_EF
	void OP_E0(); void OP_E1(); void OP_E2(); void OP_E3();
	void OP_E4(); void OP_E5(); void OP_E6(); void OP_E7();
	void OP_E8(); void OP_E9(); void OP_EA(); void OP_EB();
	void OP_EC(); void OP_ED(); void OP_EE(); void OP_EF();
#pragma endregion

#pragma region OP_F0_FF
	void OP_F0(); void OP_F1(); void OP_F2(); void OP_F3();
	void OP_F4(); void OP_F5(); void OP_F6(); void OP_F7();
	void OP_F8(); void OP_F9(); void OP_FA(); void OP_FB();
	void OP_FC(); void OP_FD(); void OP_FE(); void OP_FF();
#pragma endregion

#pragma endregion


#pragma region CB Opcode Hell

#pragma region OP_CB_00_0F
	void OP_CB_00(); void OP_CB_01(); void OP_CB_02(); void OP_CB_03();
	void OP_CB_04(); void OP_CB_05(); void OP_CB_06(); void OP_CB_07();
	void OP_CB_08(); void OP_CB_09(); void OP_CB_0A(); void OP_CB_0B();
	void OP_CB_0C(); void OP_CB_0D(); void OP_CB_0E(); void OP_CB_0F();
#pragma endregion

#pragma region OP_CB_10_1F
	void OP_CB_10(); void OP_CB_11(); void OP_CB_12(); void OP_CB_13();
	void OP_CB_14(); void OP_CB_15(); void OP_CB_16(); void OP_CB_17();
	void OP_CB_18(); void OP_CB_19(); void OP_CB_1A(); void OP_CB_1B();
	void OP_CB_1C(); void OP_CB_1D(); void OP_CB_1E(); void OP_CB_1F();
#pragma endregion

#pragma region OP_CB_20_2F
	void OP_CB_20(); void OP_CB_21(); void OP_CB_22(); void OP_CB_23();
	void OP_CB_24(); void OP_CB_25(); void OP_CB_26(); void OP_CB_27();
	void OP_CB_28(); void OP_CB_29(); void OP_CB_2A(); void OP_CB_2B();
	void OP_CB_2C(); void OP_CB_2D(); void OP_CB_2E(); void OP_CB_2F();
#pragma endregion

#pragma region OP_CB_30_3F
	void OP_CB_30(); void OP_CB_31(); void OP_CB_32(); void OP_CB_33();
	void OP_CB_34(); void OP_CB_35(); void OP_CB_36(); void OP_CB_37();
	void OP_CB_38(); void OP_CB_39(); void OP_CB_3A(); void OP_CB_3B();
	void OP_CB_3C(); void OP_CB_3D(); void OP_CB_3E(); void OP_CB_3F();
#pragma endregion

#pragma region OP_CB_40_4F
	void OP_CB_40(); void OP_CB_41(); void OP_CB_42(); void OP_CB_43();
	void OP_CB_44(); void OP_CB_45(); void OP_CB_46(); void OP_CB_47();
	void OP_CB_48(); void OP_CB_49(); void OP_CB_4A(); void OP_CB_4B();
	void OP_CB_4C(); void OP_CB_4D(); void OP_CB_4E(); void OP_CB_4F();
#pragma endregion

#pragma region OP_CB_50_5F
	void OP_CB_50(); void OP_CB_51(); void OP_CB_52(); void OP_CB_53();
	void OP_CB_54(); void OP_CB_55(); void OP_CB_56(); void OP_CB_57();
	void OP_CB_58(); void OP_CB_59(); void OP_CB_5A(); void OP_CB_5B();
	void OP_CB_5C(); void OP_CB_5D(); void OP_CB_5E(); void OP_CB_5F();
#pragma endregion

#pragma region OP_CB_60_6F
	void OP_CB_60(); void OP_CB_61(); void OP_CB_62(); void OP_CB_63();
	void OP_CB_64(); void OP_CB_65(); void OP_CB_66(); void OP_CB_67();
	void OP_CB_68(); void OP_CB_69(); void OP_CB_6A(); void OP_CB_6B();
	void OP_CB_6C(); void OP_CB_6D(); void OP_CB_6E(); void OP_CB_6F();
#pragma endregion

#pragma region OP_CB_70_7F
	void OP_CB_70(); void OP_CB_71(); void OP_CB_72(); void OP_CB_73();
	void OP_CB_74(); void OP_CB_75(); void OP_CB_76(); void OP_CB_77();
	void OP_CB_78(); void OP_CB_79(); void OP_CB_7A(); void OP_CB_7B();
	void OP_CB_7C(); void OP_CB_7D(); void OP_CB_7E(); void OP_CB_7F();
#pragma endregion

#pragma region OP_CB_80_8F
	void OP_CB_80(); void OP_CB_81(); void OP_CB_82(); void OP_CB_83();
	void OP_CB_84(); void OP_CB_85(); void OP_CB_86(); void OP_CB_87();
	void OP_CB_88(); void OP_CB_89(); void OP_CB_8A(); void OP_CB_8B();
	void OP_CB_8C(); void OP_CB_8D(); void OP_CB_8E(); void OP_CB_8F();
#pragma endregion

#pragma region OP_CB_90_9F
	void OP_CB_90(); void OP_CB_91(); void OP_CB_92(); void OP_CB_93();
	void OP_CB_94(); void OP_CB_95(); void OP_CB_96(); void OP_CB_97();
	void OP_CB_98(); void OP_CB_99(); void OP_CB_9A(); void OP_CB_9B();
	void OP_CB_9C(); void OP_CB_9D(); void OP_CB_9E(); void OP_CB_9F();
#pragma endregion

#pragma region OP_CB_A0_AF
	void OP_CB_A0(); void OP_CB_A1(); void OP_CB_A2(); void OP_CB_A3();
	void OP_CB_A4(); void OP_CB_A5(); void OP_CB_A6(); void OP_CB_A7();
	void OP_CB_A8(); void OP_CB_A9(); void OP_CB_AA(); void OP_CB_AB();
	void OP_CB_AC(); void OP_CB_AD(); void OP_CB_AE(); void OP_CB_AF();
#pragma endregion

#pragma region OP_CB_B0_BF
	void OP_CB_B0(); void OP_CB_B1(); void OP_CB_B2(); void OP_CB_B3();
	void OP_CB_B4(); void OP_CB_B5(); void OP_CB_B6(); void OP_CB_B7();
	void OP_CB_B8(); void OP_CB_B9(); void OP_CB_BA(); void OP_CB_BB();
	void OP_CB_BC(); void OP_CB_BD(); void OP_CB_BE(); void OP_CB_BF();
#pragma endregion

#pragma region OP_CB_C0_CF
	void OP_CB_C0(); void OP_CB_C1(); void OP_CB_C2(); void OP_CB_C3();
	void OP_CB_C4(); void OP_CB_C5(); void OP_CB_C6(); void OP_CB_C7();
	void OP_CB_C8(); void OP_CB_C9(); void OP_CB_CA(); void OP_CB_CB();
	void OP_CB_CC(); void OP_CB_CD(); void OP_CB_CE(); void OP_CB_CF();
#pragma endregion

#pragma region OP_CB_D0_DF
	void OP_CB_D0(); void OP_CB_D1(); void OP_CB_D2(); void OP_CB_D3();
	void OP_CB_D4(); void OP_CB_D5(); void OP_CB_D6(); void OP_CB_D7();
	void OP_CB_D8(); void OP_CB_D9(); void OP_CB_DA(); void OP_CB_DB();
	void OP_CB_DC(); void OP_CB_DD(); void OP_CB_DE(); void OP_CB_DF();
#pragma endregion

#pragma region OP_CB_E0_EF
	void OP_CB_E0(); void OP_CB_E1(); void OP_CB_E2(); void OP_CB_E3();
	void OP_CB_E4(); void OP_CB_E5(); void OP_CB_E6(); void OP_CB_E7();
	void OP_CB_E8(); void OP_CB_E9(); void OP_CB_EA(); void OP_CB_EB();
	void OP_CB_EC(); void OP_CB_ED(); void OP_CB_EE(); void OP_CB_EF();
#pragma endregion

#pragma region OP_CB_F0_FF
	void OP_CB_F0(); void OP_CB_F1(); void OP_CB_F2(); void OP_CB_F3();
	void OP_CB_F4(); void OP_CB_F5(); void OP_CB_F6(); void OP_CB_F7();
	void OP_CB_F8(); void OP_CB_F9(); void OP_CB_FA(); void OP_CB_FB();
	void OP_CB_FC(); void OP_CB_FD(); void OP_CB_FE(); void OP_CB_FF();
#pragma endregion

#pragma endregion
};