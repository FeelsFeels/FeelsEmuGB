#include "../CPU.h"
#include "../Bus.h"



void CPU::RegisterInstructions()
{
    instructions.resize(256);
    cbInstructions.resize(256);

    // --- MISC & 16-BIT (0x00 - 0x3F) ---
    instructions[0x00] = { &CPU::OP_00, "NOP", 4 };
    instructions[0x01] = { &CPU::OP_01, "LD BC, n16", 12 };
    instructions[0x02] = { &CPU::OP_02, "LD (BC), A", 8 };
    instructions[0x03] = { &CPU::OP_03, "INC BC", 8 };
    instructions[0x04] = { &CPU::OP_04, "INC B", 4 };
    instructions[0x05] = { &CPU::OP_05, "DEC B", 4 };
    instructions[0x06] = { &CPU::OP_06, "LD B, n8", 8 };
    instructions[0x07] = { &CPU::OP_07, "RLCA", 4 };

    instructions[0x08] = { &CPU::OP_08, "LD (a16), SP", 20 };
    instructions[0x09] = { &CPU::OP_09, "ADD HL, BC", 8 };
    instructions[0x0A] = { &CPU::OP_0A, "LD A, (BC)", 8 };
    instructions[0x0B] = { &CPU::OP_0B, "DEC BC", 8 };
    instructions[0x0C] = { &CPU::OP_0C, "INC C", 4 };
    instructions[0x0D] = { &CPU::OP_0D, "DEC C", 4 };
    instructions[0x0E] = { &CPU::OP_0E, "LD C, n8", 8 };
    instructions[0x0F] = { &CPU::OP_0F, "RRCA", 4 };

    instructions[0x10] = { &CPU::OP_10, "STOP", 4 };
    instructions[0x11] = { &CPU::OP_11, "LD DE, n16", 12 };
    instructions[0x12] = { &CPU::OP_12, "LD (DE), A", 8 };
    instructions[0x13] = { &CPU::OP_13, "INC DE", 8 };
    instructions[0x14] = { &CPU::OP_14, "INC D", 4 };
    instructions[0x15] = { &CPU::OP_15, "DEC D", 4 };
    instructions[0x16] = { &CPU::OP_16, "LD D, n8", 8 };
    instructions[0x17] = { &CPU::OP_17, "RLA", 4 };

    instructions[0x18] = { &CPU::OP_18, "JR n8", 12 };
    instructions[0x19] = { &CPU::OP_19, "ADD HL, DE", 8 };
    instructions[0x1A] = { &CPU::OP_1A, "LD A, (DE)", 8 };
    instructions[0x1B] = { &CPU::OP_1B, "DEC DE", 8 };
    instructions[0x1C] = { &CPU::OP_1C, "INC E", 4 };
    instructions[0x1D] = { &CPU::OP_1D, "DEC E", 4 };
    instructions[0x1E] = { &CPU::OP_1E, "LD E, n8", 8 };
    instructions[0x1F] = { &CPU::OP_1F, "RRA", 4 };

    instructions[0x20] = { &CPU::OP_20, "JR NZ, n8", 8 }; // +4 if taken
    instructions[0x21] = { &CPU::OP_21, "LD HL, n16", 12 };
    instructions[0x22] = { &CPU::OP_22, "LDI (HL), A", 8 };
    instructions[0x23] = { &CPU::OP_23, "INC HL", 8 };
    instructions[0x24] = { &CPU::OP_24, "INC H", 4 };
    instructions[0x25] = { &CPU::OP_25, "DEC H", 4 };
    instructions[0x26] = { &CPU::OP_26, "LD H, n8", 8 };
    instructions[0x27] = { &CPU::OP_27, "DAA", 4 };

    instructions[0x28] = { &CPU::OP_28, "JR Z, n8", 8 }; // +4 if taken
    instructions[0x29] = { &CPU::OP_29, "ADD HL, HL", 8 };
    instructions[0x2A] = { &CPU::OP_2A, "LDI A, (HL)", 8 };
    instructions[0x2B] = { &CPU::OP_2B, "DEC HL", 8 };
    instructions[0x2C] = { &CPU::OP_2C, "INC L", 4 };
    instructions[0x2D] = { &CPU::OP_2D, "DEC L", 4 };
    instructions[0x2E] = { &CPU::OP_2E, "LD L, n8", 8 };
    instructions[0x2F] = { &CPU::OP_2F, "CPL", 4 };

    instructions[0x30] = { &CPU::OP_30, "JR NC, n8", 8 }; // +4 if taken
    instructions[0x31] = { &CPU::OP_31, "LD SP, n16", 12 };
    instructions[0x32] = { &CPU::OP_32, "LDD (HL), A", 8 };
    instructions[0x33] = { &CPU::OP_33, "INC SP", 8 };
    instructions[0x34] = { &CPU::OP_34, "INC (HL)", 12 };
    instructions[0x35] = { &CPU::OP_35, "DEC (HL)", 12 };
    instructions[0x36] = { &CPU::OP_36, "LD (HL), n8", 12 };
    instructions[0x37] = { &CPU::OP_37, "SCF", 4 };

    instructions[0x38] = { &CPU::OP_38, "JR C, n8", 8 }; // +4 if taken
    instructions[0x39] = { &CPU::OP_39, "ADD HL, SP", 8 };
    instructions[0x3A] = { &CPU::OP_3A, "LDD A, (HL)", 8 };
    instructions[0x3B] = { &CPU::OP_3B, "DEC SP", 8 };
    instructions[0x3C] = { &CPU::OP_3C, "INC A", 4 };
    instructions[0x3D] = { &CPU::OP_3D, "DEC A", 4 };
    instructions[0x3E] = { &CPU::OP_3E, "LD A, n8", 8 };
    instructions[0x3F] = { &CPU::OP_3F, "CCF", 4 };

    // --- 8-BIT LOADS (0x40 - 0x7F) ---
    // Row 4
    instructions[0x40] = { &CPU::OP_40, "LD B, B", 4 };
    instructions[0x41] = { &CPU::OP_41, "LD B, C", 4 };
    instructions[0x42] = { &CPU::OP_42, "LD B, D", 4 };
    instructions[0x43] = { &CPU::OP_43, "LD B, E", 4 };
    instructions[0x44] = { &CPU::OP_44, "LD B, H", 4 };
    instructions[0x45] = { &CPU::OP_45, "LD B, L", 4 };
    instructions[0x46] = { &CPU::OP_46, "LD B, (HL)", 8 };
    instructions[0x47] = { &CPU::OP_47, "LD B, A", 4 };

    instructions[0x48] = { &CPU::OP_48, "LD C, B", 4 };
    instructions[0x49] = { &CPU::OP_49, "LD C, C", 4 };
    instructions[0x4A] = { &CPU::OP_4A, "LD C, D", 4 };
    instructions[0x4B] = { &CPU::OP_4B, "LD C, E", 4 };
    instructions[0x4C] = { &CPU::OP_4C, "LD C, H", 4 };
    instructions[0x4D] = { &CPU::OP_4D, "LD C, L", 4 };
    instructions[0x4E] = { &CPU::OP_4E, "LD C, (HL)", 8 };
    instructions[0x4F] = { &CPU::OP_4F, "LD C, A", 4 };

    // Row 5
    instructions[0x50] = { &CPU::OP_50, "LD D, B", 4 };
    instructions[0x51] = { &CPU::OP_51, "LD D, C", 4 };
    instructions[0x52] = { &CPU::OP_52, "LD D, D", 4 };
    instructions[0x53] = { &CPU::OP_53, "LD D, E", 4 };
    instructions[0x54] = { &CPU::OP_54, "LD D, H", 4 };
    instructions[0x55] = { &CPU::OP_55, "LD D, L", 4 };
    instructions[0x56] = { &CPU::OP_56, "LD D, (HL)", 8 };
    instructions[0x57] = { &CPU::OP_57, "LD D, A", 4 };

    instructions[0x58] = { &CPU::OP_58, "LD E, B", 4 };
    instructions[0x59] = { &CPU::OP_59, "LD E, C", 4 };
    instructions[0x5A] = { &CPU::OP_5A, "LD E, D", 4 };
    instructions[0x5B] = { &CPU::OP_5B, "LD E, E", 4 };
    instructions[0x5C] = { &CPU::OP_5C, "LD E, H", 4 };
    instructions[0x5D] = { &CPU::OP_5D, "LD E, L", 4 };
    instructions[0x5E] = { &CPU::OP_5E, "LD E, (HL)", 8 };
    instructions[0x5F] = { &CPU::OP_5F, "LD E, A", 4 };

    // Row 6
    instructions[0x60] = { &CPU::OP_60, "LD H, B", 4 };
    instructions[0x61] = { &CPU::OP_61, "LD H, C", 4 };
    instructions[0x62] = { &CPU::OP_62, "LD H, D", 4 };
    instructions[0x63] = { &CPU::OP_63, "LD H, E", 4 };
    instructions[0x64] = { &CPU::OP_64, "LD H, H", 4 };
    instructions[0x65] = { &CPU::OP_65, "LD H, L", 4 };
    instructions[0x66] = { &CPU::OP_66, "LD H, (HL)", 8 };
    instructions[0x67] = { &CPU::OP_67, "LD H, A", 4 };

    instructions[0x68] = { &CPU::OP_68, "LD L, B", 4 };
    instructions[0x69] = { &CPU::OP_69, "LD L, C", 4 };
    instructions[0x6A] = { &CPU::OP_6A, "LD L, D", 4 };
    instructions[0x6B] = { &CPU::OP_6B, "LD L, E", 4 };
    instructions[0x6C] = { &CPU::OP_6C, "LD L, H", 4 };
    instructions[0x6D] = { &CPU::OP_6D, "LD L, L", 4 };
    instructions[0x6E] = { &CPU::OP_6E, "LD L, (HL)", 8 };
    instructions[0x6F] = { &CPU::OP_6F, "LD L, A", 4 };

    // Row 7
    instructions[0x70] = { &CPU::OP_70, "LD (HL), B", 8 };
    instructions[0x71] = { &CPU::OP_71, "LD (HL), C", 8 };
    instructions[0x72] = { &CPU::OP_72, "LD (HL), D", 8 };
    instructions[0x73] = { &CPU::OP_73, "LD (HL), E", 8 };
    instructions[0x74] = { &CPU::OP_74, "LD (HL), H", 8 };
    instructions[0x75] = { &CPU::OP_75, "LD (HL), L", 8 };
    instructions[0x76] = { &CPU::OP_76, "HALT", 4 };
    instructions[0x77] = { &CPU::OP_77, "LD (HL), A", 8 };

    instructions[0x78] = { &CPU::OP_78, "LD A, B", 4 };
    instructions[0x79] = { &CPU::OP_79, "LD A, C", 4 };
    instructions[0x7A] = { &CPU::OP_7A, "LD A, D", 4 };
    instructions[0x7B] = { &CPU::OP_7B, "LD A, E", 4 };
    instructions[0x7C] = { &CPU::OP_7C, "LD A, H", 4 };
    instructions[0x7D] = { &CPU::OP_7D, "LD A, L", 4 };
    instructions[0x7E] = { &CPU::OP_7E, "LD A, (HL)", 8 };
    instructions[0x7F] = { &CPU::OP_7F, "LD A, A", 4 };

    // --- 8-BIT ALU (0x80 - 0xBF) ---
    // Row 8 (ADD / ADC)
    instructions[0x80] = { &CPU::OP_80, "ADD A, B", 4 };
    instructions[0x81] = { &CPU::OP_81, "ADD A, C", 4 };
    instructions[0x82] = { &CPU::OP_82, "ADD A, D", 4 };
    instructions[0x83] = { &CPU::OP_83, "ADD A, E", 4 };
    instructions[0x84] = { &CPU::OP_84, "ADD A, H", 4 };
    instructions[0x85] = { &CPU::OP_85, "ADD A, L", 4 };
    instructions[0x86] = { &CPU::OP_86, "ADD A, (HL)", 8 };
    instructions[0x87] = { &CPU::OP_87, "ADD A, A", 4 };

    instructions[0x88] = { &CPU::OP_88, "ADC A, B", 4 };
    instructions[0x89] = { &CPU::OP_89, "ADC A, C", 4 };
    instructions[0x8A] = { &CPU::OP_8A, "ADC A, D", 4 };
    instructions[0x8B] = { &CPU::OP_8B, "ADC A, E", 4 };
    instructions[0x8C] = { &CPU::OP_8C, "ADC A, H", 4 };
    instructions[0x8D] = { &CPU::OP_8D, "ADC A, L", 4 };
    instructions[0x8E] = { &CPU::OP_8E, "ADC A, (HL)", 8 };
    instructions[0x8F] = { &CPU::OP_8F, "ADC A, A", 4 };

    // Row 9 (SUB / SBC)
    instructions[0x90] = { &CPU::OP_90, "SUB B", 4 };
    instructions[0x91] = { &CPU::OP_91, "SUB C", 4 };
    instructions[0x92] = { &CPU::OP_92, "SUB D", 4 };
    instructions[0x93] = { &CPU::OP_93, "SUB E", 4 };
    instructions[0x94] = { &CPU::OP_94, "SUB H", 4 };
    instructions[0x95] = { &CPU::OP_95, "SUB L", 4 };
    instructions[0x96] = { &CPU::OP_96, "SUB (HL)", 8 };
    instructions[0x97] = { &CPU::OP_97, "SUB A", 4 };

    instructions[0x98] = { &CPU::OP_98, "SBC A, B", 4 };
    instructions[0x99] = { &CPU::OP_99, "SBC A, C", 4 };
    instructions[0x9A] = { &CPU::OP_9A, "SBC A, D", 4 };
    instructions[0x9B] = { &CPU::OP_9B, "SBC A, E", 4 };
    instructions[0x9C] = { &CPU::OP_9C, "SBC A, H", 4 };
    instructions[0x9D] = { &CPU::OP_9D, "SBC A, L", 4 };
    instructions[0x9E] = { &CPU::OP_9E, "SBC A, (HL)", 8 };
    instructions[0x9F] = { &CPU::OP_9F, "SBC A, A", 4 };

    // Row A (AND / XOR)
    instructions[0xA0] = { &CPU::OP_A0, "AND B", 4 };
    instructions[0xA1] = { &CPU::OP_A1, "AND C", 4 };
    instructions[0xA2] = { &CPU::OP_A2, "AND D", 4 };
    instructions[0xA3] = { &CPU::OP_A3, "AND E", 4 };
    instructions[0xA4] = { &CPU::OP_A4, "AND H", 4 };
    instructions[0xA5] = { &CPU::OP_A5, "AND L", 4 };
    instructions[0xA6] = { &CPU::OP_A6, "AND (HL)", 8 };
    instructions[0xA7] = { &CPU::OP_A7, "AND A", 4 };

    instructions[0xA8] = { &CPU::OP_A8, "XOR B", 4 };
    instructions[0xA9] = { &CPU::OP_A9, "XOR C", 4 };
    instructions[0xAA] = { &CPU::OP_AA, "XOR D", 4 };
    instructions[0xAB] = { &CPU::OP_AB, "XOR E", 4 };
    instructions[0xAC] = { &CPU::OP_AC, "XOR H", 4 };
    instructions[0xAD] = { &CPU::OP_AD, "XOR L", 4 };
    instructions[0xAE] = { &CPU::OP_AE, "XOR (HL)", 8 };
    instructions[0xAF] = { &CPU::OP_AF, "XOR A", 4 };

    // Row B (OR / CP)
    instructions[0xB0] = { &CPU::OP_B0, "OR B", 4 };
    instructions[0xB1] = { &CPU::OP_B1, "OR C", 4 };
    instructions[0xB2] = { &CPU::OP_B2, "OR D", 4 };
    instructions[0xB3] = { &CPU::OP_B3, "OR E", 4 };
    instructions[0xB4] = { &CPU::OP_B4, "OR H", 4 };
    instructions[0xB5] = { &CPU::OP_B5, "OR L", 4 };
    instructions[0xB6] = { &CPU::OP_B6, "OR (HL)", 8 };
    instructions[0xB7] = { &CPU::OP_B7, "OR A", 4 };

    instructions[0xB8] = { &CPU::OP_B8, "CP B", 4 };
    instructions[0xB9] = { &CPU::OP_B9, "CP C", 4 };
    instructions[0xBA] = { &CPU::OP_BA, "CP D", 4 };
    instructions[0xBB] = { &CPU::OP_BB, "CP E", 4 };
    instructions[0xBC] = { &CPU::OP_BC, "CP H", 4 };
    instructions[0xBD] = { &CPU::OP_BD, "CP L", 4 };
    instructions[0xBE] = { &CPU::OP_BE, "CP (HL)", 8 };
    instructions[0xBF] = { &CPU::OP_BF, "CP A", 4 };

    // --- CONTROL & I/O (0xC0 - 0xFF) ---
    instructions[0xC0] = { &CPU::OP_C0, "RET NZ", 8 };  // +12 if taken
    instructions[0xC1] = { &CPU::OP_C1, "POP BC", 12 };
    instructions[0xC2] = { &CPU::OP_C2, "JP NZ, a16", 12 }; // +4 if taken
    instructions[0xC3] = { &CPU::OP_C3, "JP a16", 16 };
    instructions[0xC4] = { &CPU::OP_C4, "CALL NZ, a16", 12 }; // +12 if taken
    instructions[0xC5] = { &CPU::OP_C5, "PUSH BC", 16 };
    instructions[0xC6] = { &CPU::OP_C6, "ADD A, n8", 8 };
    instructions[0xC7] = { &CPU::OP_C7, "RST 00H", 16 };

    instructions[0xC8] = { &CPU::OP_C8, "RET Z", 8 };
    instructions[0xC9] = { &CPU::OP_C9, "RET", 16 };
    instructions[0xCA] = { &CPU::OP_CA, "JP Z, a16", 12 };
    instructions[0xCB] = { &CPU::OP_CB, "PREFIX CB", 4 };
    instructions[0xCC] = { &CPU::OP_CC, "CALL Z, a16", 12 };
    instructions[0xCD] = { &CPU::OP_CD, "CALL a16", 24 };
    instructions[0xCE] = { &CPU::OP_CE, "ADC A, n8", 8 };
    instructions[0xCF] = { &CPU::OP_CF, "RST 08H", 16 };

    instructions[0xD0] = { &CPU::OP_D0, "RET NC", 8 };
    instructions[0xD1] = { &CPU::OP_D1, "POP DE", 12 };
    instructions[0xD2] = { &CPU::OP_D2, "JP NC, a16", 12 };
    // D3 unused
    instructions[0xD4] = { &CPU::OP_D4, "CALL NC, a16", 12 };
    instructions[0xD5] = { &CPU::OP_D5, "PUSH DE", 16 };
    instructions[0xD6] = { &CPU::OP_D6, "SUB n8", 8 };
    instructions[0xD7] = { &CPU::OP_D7, "RST 10H", 16 };

    instructions[0xD8] = { &CPU::OP_D8, "RET C", 8 };
    instructions[0xD9] = { &CPU::OP_D9, "RETI", 16 };
    instructions[0xDA] = { &CPU::OP_DA, "JP C, a16", 12 };
    // DB unused
    instructions[0xDC] = { &CPU::OP_DC, "CALL C, a16", 12 };
    // DD unused
    instructions[0xDE] = { &CPU::OP_DE, "SBC A, n8", 8 };
    instructions[0xDF] = { &CPU::OP_DF, "RST 18H", 16 };

    instructions[0xE0] = { &CPU::OP_E0, "LDH (n8), A", 12 };
    instructions[0xE1] = { &CPU::OP_E1, "POP HL", 12 };
    instructions[0xE2] = { &CPU::OP_E2, "LD (C), A", 8 };
    // E3, E4 unused
    instructions[0xE5] = { &CPU::OP_E5, "PUSH HL", 16 };
    instructions[0xE6] = { &CPU::OP_E6, "AND n8", 8 };
    instructions[0xE7] = { &CPU::OP_E7, "RST 20H", 16 };

    instructions[0xE8] = { &CPU::OP_E8, "ADD SP, e8", 16 };
    instructions[0xE9] = { &CPU::OP_E9, "JP (HL)", 4 };
    instructions[0xEA] = { &CPU::OP_EA, "LD (a16), A", 16 };
    // EB, EC, ED unused
    instructions[0xEE] = { &CPU::OP_EE, "XOR n8", 8 };
    instructions[0xEF] = { &CPU::OP_EF, "RST 28H", 16 };

    instructions[0xF0] = { &CPU::OP_F0, "LDH A, (n8)", 12 };
    instructions[0xF1] = { &CPU::OP_F1, "POP AF", 12 };
    instructions[0xF2] = { &CPU::OP_F2, "LD A, (C)", 8 };
    instructions[0xF3] = { &CPU::OP_F3, "DI", 4 };
    // F4 unused
    instructions[0xF5] = { &CPU::OP_F5, "PUSH AF", 16 };
    instructions[0xF6] = { &CPU::OP_F6, "OR n8", 8 };
    instructions[0xF7] = { &CPU::OP_F7, "RST 30H", 16 };

    instructions[0xF8] = { &CPU::OP_F8, "LD HL, SP+e8", 12 };
    instructions[0xF9] = { &CPU::OP_F9, "LD SP, HL", 8 };
    instructions[0xFA] = { &CPU::OP_FA, "LD A, (a16)", 16 };
    instructions[0xFB] = { &CPU::OP_FB, "EI", 4 };
    // FC, FD unused
    instructions[0xFE] = { &CPU::OP_FE, "CP n8", 8 };
    instructions[0xFF] = { &CPU::OP_FF, "RST 38H", 16 };


    // --- CB PREFIX INSTRUCTIONS ---
    // Pattern: 
    // x0-x7: Register B, C, D, E, H, L, (HL), A
    // Row 0: RLC, RRC, RL, RR, SLA, SRA, SWAP, SRL
    // Row 4-7: BIT
    // Row 8-B: RES
    // Row C-F: SET
    
    // --- CB PREFIX INSTRUCTIONS (Data Only) ---
    for (int i = 0; i < 256; i++)
    {
        uint8_t code = i;
        uint8_t reg_idx = code & 0x07; // 0=B, 1=C, 2=D, 3=E, 4=H, 5=L, 6=(HL), 7=A
        uint8_t op_idx = (code >> 3) & 0x1F;

        // Calculate Cycles
        // (HL) operations usually take 16, others 8. BIT (HL) takes 12.
        int cycles = (reg_idx == 6) ? 16 : 8;
        if (op_idx >= 8 && op_idx <= 15)
        { // BIT
            cycles = (reg_idx == 6) ? 12 : 8;
        }

        // Generate Debug Name (Optional, but helpful)
        std::string name = "CB " + std::to_string(i);

        // Fill table with nullptr for the function, but keep the cycles!
        cbInstructions[i] = { nullptr, "CB", (uint8_t)cycles };
    }


}



// ==========================================================
// MISC & 16-BIT INSTRUCTIONS (0x00 - 0x3F)
// ==========================================================
#define INC_16(op, reg_pair)     void CPU::op() { INC_r16(reg_pair); }
#define DEC_16(op, reg_pair)     void CPU::op() { DEC_r16(reg_pair); }
#define LD_16_N(op, reg_pair)    void CPU::op() { reg_pair = FetchWord(); }
#define ADD_HL_16(op, reg_pair)  void CPU::op() { ADD_HL(reg_pair); }

#define INC_8(op, dst)           void CPU::op() { INC_r8(dst); }
#define DEC_8(op, dst)           void CPU::op() { DEC_r8(dst); }
#define LD_8_N(op, dst)          void CPU::op() { LD_r8_n8(dst, FetchByte()); }
#define INC_MEM(op)              void CPU::op() { uint8_t val = bus->Read(reg.hl); val++; bus->Write(reg.hl, val); reg.SetZ(val==0); reg.SetN(0); reg.SetH((val&0xF)==0); }
#define DEC_MEM(op)              void CPU::op() { uint8_t val = bus->Read(reg.hl); val--; bus->Write(reg.hl, val); reg.SetZ(val==0); reg.SetN(1); reg.SetH((val&0xF)==0xF); }


// Row 0
void CPU::OP_00() { NOP(); }
LD_16_N(OP_01, reg.bc)
void CPU::OP_02() { LD_addr_r8(reg.bc, reg.a); } // LD (BC), A
INC_16(OP_03, reg.bc)
INC_8(OP_04, reg.b)
DEC_8(OP_05, reg.b)
LD_8_N(OP_06, reg.b)
void CPU::OP_07() { RLC(reg.a); reg.SetZ(false); } // RLCA (Z is always 0)

void CPU::OP_08() { LD_addr_SP(FetchWord()); } // LD (a16), SP
ADD_HL_16(OP_09, reg.bc)
void CPU::OP_0A() { LD_r8_addr(reg.a, reg.bc); } // LD A, (BC)
DEC_16(OP_0B, reg.bc)
INC_8(OP_0C, reg.c)
DEC_8(OP_0D, reg.c)
LD_8_N(OP_0E, reg.c)
void CPU::OP_0F() { RRC(reg.a); reg.SetZ(false); } // RRCA (Z is always 0)

// Row 1
void CPU::OP_10() { STOP(); } // Note: Technically consumes an extra 0x00 byte
LD_16_N(OP_11, reg.de)
void CPU::OP_12() { LD_addr_r8(reg.de, reg.a); } // LD (DE), A
INC_16(OP_13, reg.de)
INC_8(OP_14, reg.d)
DEC_8(OP_15, reg.d)
LD_8_N(OP_16, reg.d)
void CPU::OP_17() { RL(reg.a); reg.SetZ(false); } // RLA (Z is always 0)

void CPU::OP_18() { JR((int8_t)FetchByte()); } // JR r8
ADD_HL_16(OP_19, reg.de)
void CPU::OP_1A() { LD_r8_addr(reg.a, reg.de); } // LD A, (DE)
DEC_16(OP_1B, reg.de)
INC_8(OP_1C, reg.e)
DEC_8(OP_1D, reg.e)
LD_8_N(OP_1E, reg.e)
void CPU::OP_1F() { RR(reg.a); reg.SetZ(false); } // RRA (Z is always 0)

// Row 2
void CPU::OP_20() { int8_t off = (int8_t)FetchByte(); if (!reg.GetZ()) JR(off); } // JR NZ, r8
LD_16_N(OP_21, reg.hl)
void CPU::OP_22() { LD_addr_r8(reg.hl, reg.a); reg.hl++; } // LDI (HL), A
INC_16(OP_23, reg.hl)
INC_8(OP_24, reg.h)
DEC_8(OP_25, reg.h)
LD_8_N(OP_26, reg.h)
void CPU::OP_27() { DAA(); }

void CPU::OP_28() { int8_t off = (int8_t)FetchByte(); if (reg.GetZ()) JR(off); } // JR Z, r8
ADD_HL_16(OP_29, reg.hl)
void CPU::OP_2A() { LD_r8_addr(reg.a, reg.hl); reg.hl++; } // LDI A, (HL)
DEC_16(OP_2B, reg.hl)
INC_8(OP_2C, reg.l)
DEC_8(OP_2D, reg.l)
LD_8_N(OP_2E, reg.l)
void CPU::OP_2F() { CPL(); }

// Row 3
void CPU::OP_30() { int8_t off = (int8_t)FetchByte(); if (!reg.GetC()) JR(off); } // JR NC, r8
LD_16_N(OP_31, reg.sp)
void CPU::OP_32() { LD_addr_r8(reg.hl, reg.a); reg.hl--; } // LDD (HL), A
INC_16(OP_33, reg.sp)
INC_MEM(OP_34) // INC (HL) - Difficult one, handled by macro
DEC_MEM(OP_35) // DEC (HL)
void CPU::OP_36() { uint8_t val = FetchByte(); bus->Write(reg.hl, val); }
void CPU::OP_37() { SCF(); }

void CPU::OP_38() { int8_t off = (int8_t)FetchByte(); if (reg.GetC()) JR(off); } // JR C, r8
ADD_HL_16(OP_39, reg.sp)
void CPU::OP_3A() { LD_r8_addr(reg.a, reg.hl); reg.hl--; } // LDD A, (HL)
DEC_16(OP_3B, reg.sp)
INC_8(OP_3C, reg.a)
DEC_8(OP_3D, reg.a)
LD_8_N(OP_3E, reg.a)
void CPU::OP_3F() { CCF(); }

#undef INC_16
#undef DEC_16
#undef LD_16_N
#undef ADD_HL_16
#undef INC_8
#undef DEC_8
#undef LD_8_N
#undef INC_MEM
#undef DEC_MEM

// ==========================================================
// 8-BIT LOAD GROUP (0x40 - 0x7F)
// ==========================================================

// Helper Macros
#define LD_RR(op, dst, src) void CPU::op() { LD_r8_r8(dst, src); }
#define LD_HL(op, dst)      void CPU::op() { LD_r8_addr(dst, reg.hl); }
#define LD_TO_HL(op, src)   void CPU::op() { LD_addr_r8(reg.hl, src); }

// Row 4
LD_RR(OP_40, reg.b, reg.b) LD_RR(OP_41, reg.b, reg.c) LD_RR(OP_42, reg.b, reg.d) LD_RR(OP_43, reg.b, reg.e)
LD_RR(OP_44, reg.b, reg.h) LD_RR(OP_45, reg.b, reg.l) LD_HL(OP_46, reg.b)        LD_RR(OP_47, reg.b, reg.a)

// Row 5
LD_RR(OP_48, reg.c, reg.b) LD_RR(OP_49, reg.c, reg.c) LD_RR(OP_4A, reg.c, reg.d) LD_RR(OP_4B, reg.c, reg.e)
LD_RR(OP_4C, reg.c, reg.h) LD_RR(OP_4D, reg.c, reg.l) LD_HL(OP_4E, reg.c)        LD_RR(OP_4F, reg.c, reg.a)

// Row 6
LD_RR(OP_50, reg.d, reg.b) LD_RR(OP_51, reg.d, reg.c) LD_RR(OP_52, reg.d, reg.d) LD_RR(OP_53, reg.d, reg.e)
LD_RR(OP_54, reg.d, reg.h) LD_RR(OP_55, reg.d, reg.l) LD_HL(OP_56, reg.d)        LD_RR(OP_57, reg.d, reg.a)

LD_RR(OP_58, reg.e, reg.b) LD_RR(OP_59, reg.e, reg.c) LD_RR(OP_5A, reg.e, reg.d) LD_RR(OP_5B, reg.e, reg.e)
LD_RR(OP_5C, reg.e, reg.h) LD_RR(OP_5D, reg.e, reg.l) LD_HL(OP_5E, reg.e)        LD_RR(OP_5F, reg.e, reg.a)

// Row 6 (H and L)
LD_RR(OP_60, reg.h, reg.b) LD_RR(OP_61, reg.h, reg.c) LD_RR(OP_62, reg.h, reg.d) LD_RR(OP_63, reg.h, reg.e)
LD_RR(OP_64, reg.h, reg.h) LD_RR(OP_65, reg.h, reg.l) LD_HL(OP_66, reg.h)        LD_RR(OP_67, reg.h, reg.a)

LD_RR(OP_68, reg.l, reg.b) LD_RR(OP_69, reg.l, reg.c) LD_RR(OP_6A, reg.l, reg.d) LD_RR(OP_6B, reg.l, reg.e)
LD_RR(OP_6C, reg.l, reg.h) LD_RR(OP_6D, reg.l, reg.l) LD_HL(OP_6E, reg.l)        LD_RR(OP_6F, reg.l, reg.a)

// Row 7
LD_TO_HL(OP_70, reg.b) LD_TO_HL(OP_71, reg.c) LD_TO_HL(OP_72, reg.d) LD_TO_HL(OP_73, reg.e)
LD_TO_HL(OP_74, reg.h) LD_TO_HL(OP_75, reg.l) void CPU::OP_76() { HALT(); }  LD_TO_HL(OP_77, reg.a)

LD_RR(OP_78, reg.a, reg.b) LD_RR(OP_79, reg.a, reg.c) LD_RR(OP_7A, reg.a, reg.d) LD_RR(OP_7B, reg.a, reg.e)
LD_RR(OP_7C, reg.a, reg.h) LD_RR(OP_7D, reg.a, reg.l) LD_HL(OP_7E, reg.a)        LD_RR(OP_7F, reg.a, reg.a)

// Clean up
#undef LD_RR
#undef LD_HL
#undef LD_TO_HL


// ==========================================================
// 8-BIT ALU GROUP (0x80 - 0xBF)
// ==========================================================

#define ALU_OP(op, func, src) void CPU::op() { func(src); }
#define ALU_HL(op, func)      void CPU::op() { func(bus->Read(reg.hl)); }

// ADD
ALU_OP(OP_80, ADD, reg.b) ALU_OP(OP_81, ADD, reg.c) ALU_OP(OP_82, ADD, reg.d) ALU_OP(OP_83, ADD, reg.e)
ALU_OP(OP_84, ADD, reg.h) ALU_OP(OP_85, ADD, reg.l) ALU_HL(OP_86, ADD)        ALU_OP(OP_87, ADD, reg.a)

// ADC
ALU_OP(OP_88, ADDC, reg.b) ALU_OP(OP_89, ADDC, reg.c) ALU_OP(OP_8A, ADDC, reg.d) ALU_OP(OP_8B, ADDC, reg.e)
ALU_OP(OP_8C, ADDC, reg.h) ALU_OP(OP_8D, ADDC, reg.l) ALU_HL(OP_8E, ADDC)        ALU_OP(OP_8F, ADDC, reg.a)

// SUB
ALU_OP(OP_90, SUB, reg.b) ALU_OP(OP_91, SUB, reg.c) ALU_OP(OP_92, SUB, reg.d) ALU_OP(OP_93, SUB, reg.e)
ALU_OP(OP_94, SUB, reg.h) ALU_OP(OP_95, SUB, reg.l) ALU_HL(OP_96, SUB)        ALU_OP(OP_97, SUB, reg.a)

// SBC
ALU_OP(OP_98, SUBC, reg.b) ALU_OP(OP_99, SUBC, reg.c) ALU_OP(OP_9A, SUBC, reg.d) ALU_OP(OP_9B, SUBC, reg.e)
ALU_OP(OP_9C, SUBC, reg.h) ALU_OP(OP_9D, SUBC, reg.l) ALU_HL(OP_9E, SUBC)        ALU_OP(OP_9F, SUBC, reg.a)

// AND
ALU_OP(OP_A0, AND, reg.b) ALU_OP(OP_A1, AND, reg.c) ALU_OP(OP_A2, AND, reg.d) ALU_OP(OP_A3, AND, reg.e)
ALU_OP(OP_A4, AND, reg.h) ALU_OP(OP_A5, AND, reg.l) ALU_HL(OP_A6, AND)        ALU_OP(OP_A7, AND, reg.a)

// XOR
ALU_OP(OP_A8, XOR, reg.b) ALU_OP(OP_A9, XOR, reg.c) ALU_OP(OP_AA, XOR, reg.d) ALU_OP(OP_AB, XOR, reg.e)
ALU_OP(OP_AC, XOR, reg.h) ALU_OP(OP_AD, XOR, reg.l) ALU_HL(OP_AE, XOR)        ALU_OP(OP_AF, XOR, reg.a)

// OR
ALU_OP(OP_B0, OR, reg.b) ALU_OP(OP_B1, OR, reg.c) ALU_OP(OP_B2, OR, reg.d) ALU_OP(OP_B3, OR, reg.e)
ALU_OP(OP_B4, OR, reg.h) ALU_OP(OP_B5, OR, reg.l) ALU_HL(OP_B6, OR)        ALU_OP(OP_B7, OR, reg.a)

// CP (CMP)
ALU_OP(OP_B8, CMP, reg.b) ALU_OP(OP_B9, CMP, reg.c) ALU_OP(OP_BA, CMP, reg.d) ALU_OP(OP_BB, CMP, reg.e)
ALU_OP(OP_BC, CMP, reg.h) ALU_OP(OP_BD, CMP, reg.l) ALU_HL(OP_BE, CMP)        ALU_OP(OP_BF, CMP, reg.a)

#undef ALU_OP
#undef ALU_HL


// ==========================================================
// CONTROL FLOW & I/O (0xC0 - 0xFF)
// ==========================================================

// --- MACROS ---
#define POP_16(op, reg_pair)    void CPU::op() { reg_pair = PopWord(); }
#define PUSH_16(op, reg_pair)   void CPU::op() { PushWord(reg_pair); }
#define RESET(op, vec)            void CPU::op() { RST(vec); }

// Conditional Returns
#define RET_CC(op, flag, val)   void CPU::op() { if(flag == val) { RET(); } }
// Conditional Jumps
#define JP_CC(op, flag, val)    void CPU::op() { uint16_t addr = FetchWord(); if(flag == val) { JP(addr); } }
// Conditional Calls
#define CALL_CC(op, flag, val)  void CPU::op() { uint16_t addr = FetchWord(); if(flag == val) { CALL(addr); } }


// Row C
RET_CC(OP_C0, reg.GetZ(), false)
POP_16(OP_C1, reg.bc)
JP_CC(OP_C2, reg.GetZ(), false)
void CPU::OP_C3() { JP(FetchWord()); } // JP a16
CALL_CC(OP_C4, reg.GetZ(), false)
PUSH_16(OP_C5, reg.bc)
void CPU::OP_C6() { ADD(FetchByte()); }      // ADD A, n8
RESET(OP_C7, 0x00)

RET_CC(OP_C8, reg.GetZ(), true)
void CPU::OP_C9() { RET(); }           // RET
JP_CC(OP_CA, reg.GetZ(), true)

//CB Instruction here

CALL_CC(OP_CC, reg.GetZ(), true)
void CPU::OP_CD() { CALL(FetchWord()); } // CALL a16
void CPU::OP_CE() { ADDC(FetchByte()); }       // ADC A, n8
RESET(OP_CF, 0x08)

// Row D
RET_CC(OP_D0, reg.GetC(), false)
POP_16(OP_D1, reg.de)
JP_CC(OP_D2, reg.GetC(), false)
void CPU::OP_D3() {} // Unused/Illegal
CALL_CC(OP_D4, reg.GetC(), false)
PUSH_16(OP_D5, reg.de)
void CPU::OP_D6() { SUB(FetchByte()); }        // SUB n8
RESET(OP_D7, 0x10)

RET_CC(OP_D8, reg.GetC(), true)
void CPU::OP_D9() { RET(); EI(); }       // RETI
JP_CC(OP_DA, reg.GetC(), true)
void CPU::OP_DB() {} // Unused
CALL_CC(OP_DC, reg.GetC(), true)
void CPU::OP_DD() {} // Unused
void CPU::OP_DE() { SUBC(FetchByte()); }       // SBC A, n8
RESET(OP_DF, 0x18)

// Row E
void CPU::OP_E0() { uint8_t offset = FetchByte(); bus->Write(0xFF00 + offset, reg.a); } // LDH (n8), A
POP_16(OP_E1, reg.hl)
void CPU::OP_E2() { bus->Write(0xFF00 + reg.c, reg.a); } // LD (C), A
void CPU::OP_E3() {} // Unused
void CPU::OP_E4() {} // Unused
PUSH_16(OP_E5, reg.hl)
void CPU::OP_E6() { AND(FetchByte()); } // AND n8
RESET(OP_E7, 0x20)

void CPU::OP_E8() { ADD_SP_e8((int8_t)FetchByte()); }
void CPU::OP_E9() { JP(reg.hl); } // JP (HL) -> Jump to address IN HL
void CPU::OP_EA() { uint16_t addr = FetchWord(); bus->Write(addr, reg.a); } // LD (a16), A
void CPU::OP_EB() {} // Unused
void CPU::OP_EC() {} // Unused
void CPU::OP_ED() {} // Unused
void CPU::OP_EE() { XOR(FetchByte()); } // XOR n8
RESET(OP_EF, 0x28)

// Row F
void CPU::OP_F0() { uint8_t offset = FetchByte(); reg.a = bus->Read(0xFF00 + offset); } // LDH A, (n8)
void CPU::OP_F1() { reg.af = PopWord(); reg.f &= 0xF0; } // POP AF (Cleanup flags!)
void CPU::OP_F2() { reg.a = bus->Read(0xFF00 + reg.c); } // LD A, (C)
void CPU::OP_F3() { DI(); }
void CPU::OP_F4() {} // Unused
PUSH_16(OP_F5, reg.af)
void CPU::OP_F6() { OR(FetchByte()); } // OR n8
RESET(OP_F7, 0x30)

void CPU::OP_F8() { LD_HL_SP_e8((int8_t)FetchByte()); }
void CPU::OP_F9() { reg.sp = reg.hl; } // LD SP, HL
void CPU::OP_FA() { uint16_t addr = FetchWord(); reg.a = bus->Read(addr); } // LD A, (a16)
void CPU::OP_FB() { EI(); }
void CPU::OP_FC() {} // Unused
void CPU::OP_FD() {} // Unused
void CPU::OP_FE() { CMP(FetchByte()); } // CP n8
RESET(OP_FF, 0x38)

#undef POP_16
#undef PUSH_16
#undef RST
#undef RET_CC
#undef JP_CC
#undef CALL_CC

;
void CPU::OP_CB()
{
    uint8_t cb_op = FetchByte();
    totalCyclesForInstruction += cbInstructions[cb_op].cycles; 

    uint8_t reg_idx = cb_op & 0x07;
    uint8_t op_idx = (cb_op >> 3) & 0x1F;

    // Decode the Target (Register or Memory)
    // We use a pointer to the value so we can write it back easily
    uint8_t val;

    // If it's (HL), we read from memory
    if (reg_idx == 6)
    {
        val = bus->Read(reg.hl);
    }
    else
    {
        // Map index to register reference
        switch (reg_idx)
        {
        case 0: val = reg.b; break;
        case 1: val = reg.c; break;
        case 2: val = reg.d; break;
        case 3: val = reg.e; break;
        case 4: val = reg.h; break;
        case 5: val = reg.l; break;
        case 7: val = reg.a; break;
        }
    }

    // Perform the Operation
    // ------------------------------------------
    // ROTATES & SHIFTS (Rows 0-3)
    if (op_idx == 0)
    { // RLC
        bool carry = (val & 0x80) != 0;
        val = (val << 1) | carry;
        reg.SetZ(val == 0); reg.SetN(0); reg.SetH(0); reg.SetC(carry);
    }
    else if (op_idx == 1)
    { // RRC
        bool carry = (val & 0x01) != 0;
        val = (val >> 1) | (carry ? 0x80 : 0);
        reg.SetZ(val == 0); reg.SetN(0); reg.SetH(0); reg.SetC(carry);
    }
    else if (op_idx == 2)
    { // RL
        bool old_c = reg.GetC();
        bool new_c = (val & 0x80) != 0;
        val = (val << 1) | old_c;
        reg.SetZ(val == 0); reg.SetN(0); reg.SetH(0); reg.SetC(new_c);
    }
    else if (op_idx == 3)
    { // RR
        bool old_c = reg.GetC();
        bool new_c = (val & 0x01) != 0;
        val = (val >> 1) | (old_c ? 0x80 : 0);
        reg.SetZ(val == 0); reg.SetN(0); reg.SetH(0); reg.SetC(new_c);
    }
    else if (op_idx == 4)
    { // SLA
        bool carry = (val & 0x80) != 0;
        val = val << 1;
        reg.SetZ(val == 0); reg.SetN(0); reg.SetH(0); reg.SetC(carry);
    }
    else if (op_idx == 5)
    { // SRA
        bool carry = (val & 0x01) != 0;
        val = (val >> 1) | (val & 0x80);
        reg.SetZ(val == 0); reg.SetN(0); reg.SetH(0); reg.SetC(carry);
    }
    else if (op_idx == 6)
    { // SWAP
        val = (val << 4) | (val >> 4);
        reg.SetZ(val == 0); reg.SetN(0); reg.SetH(0); reg.SetC(0);
    }
    else if (op_idx == 7)
    { // SRL
        bool carry = (val & 0x01) != 0;
        val = val >> 1;
        reg.SetZ(val == 0); reg.SetN(0); reg.SetH(0); reg.SetC(carry);
    }
    // BIT TEST (Rows 4-7)
    else if (op_idx >= 8 && op_idx <= 15)
    {
        uint8_t bit = op_idx - 8;
        reg.SetZ(!((val >> bit) & 1));
        reg.SetN(0);
        reg.SetH(1);
        return; // BIT doesn't write back!
    }
    // RES (Rows 8-B)
    else if (op_idx >= 16 && op_idx <= 23)
    {
        uint8_t bit = op_idx - 16;
        val &= ~(1 << bit);
    }
    // SET (Rows C-F)
    else if (op_idx >= 24 && op_idx <= 31)
    {
        uint8_t bit = op_idx - 24;
        val |= (1 << bit);
    }

    // 5. Write Back
    if (reg_idx == 6)
    {
        bus->Write(reg.hl, val);
    }
    else
    {
        switch (reg_idx)
        {
        case 0: reg.b = val; break;
        case 1: reg.c = val; break;
        case 2: reg.d = val; break;
        case 3: reg.e = val; break;
        case 4: reg.h = val; break;
        case 5: reg.l = val; break;
        case 7: reg.a = val; break;
        }
    }
}