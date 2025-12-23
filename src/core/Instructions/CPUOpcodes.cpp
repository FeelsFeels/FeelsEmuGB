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

    // --- CHANGED: Max cycles for conditional jumps ---
    instructions[0x20] = { &CPU::OP_20, "JR NZ, n8", 12 }; // 12 taken, 8 not taken
    instructions[0x21] = { &CPU::OP_21, "LD HL, n16", 12 };
    instructions[0x22] = { &CPU::OP_22, "LDI (HL), A", 8 };
    instructions[0x23] = { &CPU::OP_23, "INC HL", 8 };
    instructions[0x24] = { &CPU::OP_24, "INC H", 4 };
    instructions[0x25] = { &CPU::OP_25, "DEC H", 4 };
    instructions[0x26] = { &CPU::OP_26, "LD H, n8", 8 };
    instructions[0x27] = { &CPU::OP_27, "DAA", 4 };

    // --- CHANGED ---
    instructions[0x28] = { &CPU::OP_28, "JR Z, n8", 12 }; // 12 taken, 8 not taken
    instructions[0x29] = { &CPU::OP_29, "ADD HL, HL", 8 };
    instructions[0x2A] = { &CPU::OP_2A, "LDI A, (HL)", 8 };
    instructions[0x2B] = { &CPU::OP_2B, "DEC HL", 8 };
    instructions[0x2C] = { &CPU::OP_2C, "INC L", 4 };
    instructions[0x2D] = { &CPU::OP_2D, "DEC L", 4 };
    instructions[0x2E] = { &CPU::OP_2E, "LD L, n8", 8 };
    instructions[0x2F] = { &CPU::OP_2F, "CPL", 4 };

    // --- CHANGED ---
    instructions[0x30] = { &CPU::OP_30, "JR NC, n8", 12 }; // 12 taken, 8 not taken
    instructions[0x31] = { &CPU::OP_31, "LD SP, n16", 12 };
    instructions[0x32] = { &CPU::OP_32, "LDD (HL), A", 8 };
    instructions[0x33] = { &CPU::OP_33, "INC SP", 8 };
    instructions[0x34] = { &CPU::OP_34, "INC (HL)", 12 };
    instructions[0x35] = { &CPU::OP_35, "DEC (HL)", 12 };
    instructions[0x36] = { &CPU::OP_36, "LD (HL), n8", 12 };
    instructions[0x37] = { &CPU::OP_37, "SCF", 4 };

    // --- CHANGED ---
    instructions[0x38] = { &CPU::OP_38, "JR C, n8", 12 }; // 12 taken, 8 not taken
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
    // --- CHANGED: Max cycles for conditional control flow ---
    instructions[0xC0] = { &CPU::OP_C0, "RET NZ", 20 };  // 20 taken, 8 not taken
    instructions[0xC1] = { &CPU::OP_C1, "POP BC", 12 };
    instructions[0xC2] = { &CPU::OP_C2, "JP NZ, a16", 16 }; // 16 taken, 12 not taken
    instructions[0xC3] = { &CPU::OP_C3, "JP a16", 16 };
    instructions[0xC4] = { &CPU::OP_C4, "CALL NZ, a16", 24 }; // 24 taken, 12 not taken
    instructions[0xC5] = { &CPU::OP_C5, "PUSH BC", 16 };
    instructions[0xC6] = { &CPU::OP_C6, "ADD A, n8", 8 };
    instructions[0xC7] = { &CPU::OP_C7, "RST 00H", 16 };

    instructions[0xC8] = { &CPU::OP_C8, "RET Z", 20 }; // 20 taken, 8 not taken
    instructions[0xC9] = { &CPU::OP_C9, "RET", 16 };
    instructions[0xCA] = { &CPU::OP_CA, "JP Z, a16", 16 }; // 16 taken, 12 not taken

    // --- CHANGED: Prefix itself takes 0 here, cycles added in OP_CB ---
    instructions[0xCB] = { &CPU::OP_CB, "PREFIX CB", 0 };

    instructions[0xCC] = { &CPU::OP_CC, "CALL Z, a16", 24 }; // 24 taken, 12 not taken
    instructions[0xCD] = { &CPU::OP_CD, "CALL a16", 24 };
    instructions[0xCE] = { &CPU::OP_CE, "ADC A, n8", 8 };
    instructions[0xCF] = { &CPU::OP_CF, "RST 08H", 16 };

    instructions[0xD0] = { &CPU::OP_D0, "RET NC", 20 }; // 20 taken, 8 not taken
    instructions[0xD1] = { &CPU::OP_D1, "POP DE", 12 };
    instructions[0xD2] = { &CPU::OP_D2, "JP NC, a16", 16 }; // 16 taken, 12 not taken
    // D3 unused
    instructions[0xD4] = { &CPU::OP_D4, "CALL NC, a16", 24 }; // 24 taken, 12 not taken
    instructions[0xD5] = { &CPU::OP_D5, "PUSH DE", 16 };
    instructions[0xD6] = { &CPU::OP_D6, "SUB n8", 8 };
    instructions[0xD7] = { &CPU::OP_D7, "RST 10H", 16 };

    instructions[0xD8] = { &CPU::OP_D8, "RET C", 20 }; // 20 taken, 8 not taken
    instructions[0xD9] = { &CPU::OP_D9, "RETI", 16 };
    instructions[0xDA] = { &CPU::OP_DA, "JP C, a16", 16 }; // 16 taken, 12 not taken
    // DB unused
    instructions[0xDC] = { &CPU::OP_DC, "CALL C, a16", 24 }; // 24 taken, 12 not taken
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

    // --- CB PREFIX INSTRUCTIONS (Data Only) ---
    for (int i = 0; i < 256; i++)
    {
        uint8_t code = i;
        uint8_t reg_idx = code & 0x07;
        uint8_t op_idx = (code >> 3) & 0x1F;

        // Calculate Cycles (Standard GB behavior for CB)
        // Normal: 8 cycles
        // (HL): 16 cycles
        // BIT (HL): 12 cycles
        int cycles = (reg_idx == 6) ? 16 : 8;
        if (op_idx >= 8 && op_idx <= 15) // BIT
        {
            cycles = (reg_idx == 6) ? 12 : 8;
        }

        cbInstructions[i] = { nullptr, "CB", (uint8_t)cycles };
    }
}



// --- 0x00 - 0x0F ---
void CPU::OP_00() { NOP(); }
void CPU::OP_01() { reg.bc = FetchWord(); } // LD BC, n16
void CPU::OP_02() { bus->Write(reg.bc, reg.a); } // LD (BC), A
void CPU::OP_03() { reg.bc++; } // INC BC
void CPU::OP_04() { INC_r8(reg.b); }
void CPU::OP_05() { DEC_r8(reg.b); }
void CPU::OP_06() { LD_r8_n8(reg.b, FetchByte()); }
void CPU::OP_07() { RLC(reg.a); reg.SetZ(false); } // RLCA

void CPU::OP_08() { LD_addr_SP(FetchWord()); } // LD (a16), SP
void CPU::OP_09() { ADD_HL(reg.bc); }
void CPU::OP_0A() { LD_r8_addr(reg.a, reg.bc); } // LD A, (BC)
void CPU::OP_0B() { reg.bc--; } // DEC BC
void CPU::OP_0C() { INC_r8(reg.c); }
void CPU::OP_0D() { DEC_r8(reg.c); }
void CPU::OP_0E() { LD_r8_n8(reg.c, FetchByte()); }
void CPU::OP_0F() { RRC(reg.a); reg.SetZ(false); } // RRCA

// --- 0x10 - 0x1F ---
void CPU::OP_10() { STOP(); }
void CPU::OP_11() { reg.de = FetchWord(); } // LD DE, n16
void CPU::OP_12() { bus->Write(reg.de, reg.a); } // LD (DE), A
void CPU::OP_13() { reg.de++; } // INC DE
void CPU::OP_14() { INC_r8(reg.d); }
void CPU::OP_15() { DEC_r8(reg.d); }
void CPU::OP_16() { LD_r8_n8(reg.d, FetchByte()); }
void CPU::OP_17() { RL(reg.a); reg.SetZ(false); } // RLA

void CPU::OP_18() { JR((int8_t)FetchByte()); }
void CPU::OP_19() { ADD_HL(reg.de); }
void CPU::OP_1A() { LD_r8_addr(reg.a, reg.de); } // LD A, (DE)
void CPU::OP_1B() { reg.de--; } // DEC DE
void CPU::OP_1C() { INC_r8(reg.e); }
void CPU::OP_1D() { DEC_r8(reg.e); }
void CPU::OP_1E() { LD_r8_n8(reg.e, FetchByte()); }
void CPU::OP_1F() { RR(reg.a); reg.SetZ(false); } // RRA

// --- 0x20 - 0x2F (CONDITIONALS WITH PENALTIES) ---
void CPU::OP_20() { int8_t off = (int8_t)FetchByte(); if (!reg.GetZ()) JR(off); else totalCyclesForInstruction -= 4; } // JR NZ
void CPU::OP_21() { reg.hl = FetchWord(); } // LD HL, n16
void CPU::OP_22() { bus->Write(reg.hl, reg.a); reg.hl++; } // LD (HL+), A
void CPU::OP_23() { reg.hl++; } // INC HL
void CPU::OP_24() { INC_r8(reg.h); }
void CPU::OP_25() { DEC_r8(reg.h); }
void CPU::OP_26() { LD_r8_n8(reg.h, FetchByte()); }
void CPU::OP_27() { DAA(); }

void CPU::OP_28() { int8_t off = (int8_t)FetchByte(); if (reg.GetZ()) JR(off); else totalCyclesForInstruction -= 4; } // JR Z
void CPU::OP_29() { ADD_HL(reg.hl); }
void CPU::OP_2A() { LD_r8_addr(reg.a, reg.hl); reg.hl++; } // LD A, (HL+)
void CPU::OP_2B() { reg.hl--; } // DEC HL
void CPU::OP_2C() { INC_r8(reg.l); }
void CPU::OP_2D() { DEC_r8(reg.l); }
void CPU::OP_2E() { LD_r8_n8(reg.l, FetchByte()); }
void CPU::OP_2F() { CPL(); }

// --- 0x30 - 0x3F (CONDITIONALS WITH PENALTIES) ---
void CPU::OP_30() { int8_t off = (int8_t)FetchByte(); if (!reg.GetC()) JR(off); else totalCyclesForInstruction -= 4; } // JR NC
void CPU::OP_31() { reg.sp = FetchWord(); } // LD SP, n16
void CPU::OP_32() { bus->Write(reg.hl, reg.a); reg.hl--; } // LD (HL-), A
void CPU::OP_33() { reg.sp++; } // INC SP
void CPU::OP_34()
{ // INC (HL)
    uint8_t val = bus->Read(reg.hl);
    val++;
    bus->Write(reg.hl, val);
    reg.SetZ(val == 0); reg.SetN(false); reg.SetH((val & 0x0F) == 0);
}
void CPU::OP_35()
{ // DEC (HL)
    uint8_t val = bus->Read(reg.hl);
    val--;
    bus->Write(reg.hl, val);
    reg.SetZ(val == 0); reg.SetN(true); reg.SetH((val & 0x0F) == 0x0F);
}
void CPU::OP_36() { bus->Write(reg.hl, FetchByte()); } // LD (HL), n8
void CPU::OP_37() { SCF(); }

void CPU::OP_38() { int8_t off = (int8_t)FetchByte(); if (reg.GetC()) JR(off); else totalCyclesForInstruction -= 4; } // JR C
void CPU::OP_39()
{
    ADD_HL(reg.sp); // Note: ADD HL, SP
}
void CPU::OP_3A() { LD_r8_addr(reg.a, reg.hl); reg.hl--; } // LD A, (HL-)
void CPU::OP_3B() { reg.sp--; } // DEC SP
void CPU::OP_3C() { INC_r8(reg.a); }
void CPU::OP_3D() { DEC_r8(reg.a); }
void CPU::OP_3E() { LD_r8_n8(reg.a, FetchByte()); }
void CPU::OP_3F() { CCF(); }

// --- 0x40 - 0x4F ---
void CPU::OP_40() { LD_r8_r8(reg.b, reg.b); }
void CPU::OP_41() { LD_r8_r8(reg.b, reg.c); }
void CPU::OP_42() { LD_r8_r8(reg.b, reg.d); }
void CPU::OP_43() { LD_r8_r8(reg.b, reg.e); }
void CPU::OP_44() { LD_r8_r8(reg.b, reg.h); }
void CPU::OP_45() { LD_r8_r8(reg.b, reg.l); }
void CPU::OP_46() { LD_r8_addr(reg.b, reg.hl); }
void CPU::OP_47() { LD_r8_r8(reg.b, reg.a); }

void CPU::OP_48() { LD_r8_r8(reg.c, reg.b); }
void CPU::OP_49() { LD_r8_r8(reg.c, reg.c); }
void CPU::OP_4A() { LD_r8_r8(reg.c, reg.d); }
void CPU::OP_4B() { LD_r8_r8(reg.c, reg.e); }
void CPU::OP_4C() { LD_r8_r8(reg.c, reg.h); }
void CPU::OP_4D() { LD_r8_r8(reg.c, reg.l); }
void CPU::OP_4E() { LD_r8_addr(reg.c, reg.hl); }
void CPU::OP_4F() { LD_r8_r8(reg.c, reg.a); }

// --- 0x50 - 0x5F ---
void CPU::OP_50() { LD_r8_r8(reg.d, reg.b); }
void CPU::OP_51() { LD_r8_r8(reg.d, reg.c); }
void CPU::OP_52() { LD_r8_r8(reg.d, reg.d); }
void CPU::OP_53() { LD_r8_r8(reg.d, reg.e); }
void CPU::OP_54() { LD_r8_r8(reg.d, reg.h); }
void CPU::OP_55() { LD_r8_r8(reg.d, reg.l); }
void CPU::OP_56() { LD_r8_addr(reg.d, reg.hl); }
void CPU::OP_57() { LD_r8_r8(reg.d, reg.a); }

void CPU::OP_58() { LD_r8_r8(reg.e, reg.b); }
void CPU::OP_59() { LD_r8_r8(reg.e, reg.c); }
void CPU::OP_5A() { LD_r8_r8(reg.e, reg.d); }
void CPU::OP_5B() { LD_r8_r8(reg.e, reg.e); }
void CPU::OP_5C() { LD_r8_r8(reg.e, reg.h); }
void CPU::OP_5D() { LD_r8_r8(reg.e, reg.l); }
void CPU::OP_5E() { LD_r8_addr(reg.e, reg.hl); }
void CPU::OP_5F() { LD_r8_r8(reg.e, reg.a); }

// --- 0x60 - 0x6F ---
void CPU::OP_60() { LD_r8_r8(reg.h, reg.b); }
void CPU::OP_61() { LD_r8_r8(reg.h, reg.c); }
void CPU::OP_62() { LD_r8_r8(reg.h, reg.d); }
void CPU::OP_63() { LD_r8_r8(reg.h, reg.e); }
void CPU::OP_64() { LD_r8_r8(reg.h, reg.h); }
void CPU::OP_65() { LD_r8_r8(reg.h, reg.l); }
void CPU::OP_66() { LD_r8_addr(reg.h, reg.hl); }
void CPU::OP_67() { LD_r8_r8(reg.h, reg.a); }

void CPU::OP_68() { LD_r8_r8(reg.l, reg.b); }
void CPU::OP_69() { LD_r8_r8(reg.l, reg.c); }
void CPU::OP_6A() { LD_r8_r8(reg.l, reg.d); }
void CPU::OP_6B() { LD_r8_r8(reg.l, reg.e); }
void CPU::OP_6C() { LD_r8_r8(reg.l, reg.h); }
void CPU::OP_6D() { LD_r8_r8(reg.l, reg.l); }
void CPU::OP_6E() { LD_r8_addr(reg.l, reg.hl); }
void CPU::OP_6F() { LD_r8_r8(reg.l, reg.a); }

// --- 0x70 - 0x7F ---
void CPU::OP_70() { LD_addr_r8(reg.hl, reg.b); }
void CPU::OP_71() { LD_addr_r8(reg.hl, reg.c); }
void CPU::OP_72() { LD_addr_r8(reg.hl, reg.d); }
void CPU::OP_73() { LD_addr_r8(reg.hl, reg.e); }
void CPU::OP_74() { LD_addr_r8(reg.hl, reg.h); }
void CPU::OP_75() { LD_addr_r8(reg.hl, reg.l); }
void CPU::OP_76() { HALT(); }
void CPU::OP_77() { LD_addr_r8(reg.hl, reg.a); }

void CPU::OP_78() { LD_r8_r8(reg.a, reg.b); }
void CPU::OP_79() { LD_r8_r8(reg.a, reg.c); }
void CPU::OP_7A() { LD_r8_r8(reg.a, reg.d); }
void CPU::OP_7B() { LD_r8_r8(reg.a, reg.e); }
void CPU::OP_7C() { LD_r8_r8(reg.a, reg.h); }
void CPU::OP_7D() { LD_r8_r8(reg.a, reg.l); }
void CPU::OP_7E() { LD_r8_addr(reg.a, reg.hl); }
void CPU::OP_7F() { LD_r8_r8(reg.a, reg.a); }


// ==========================================================
// 8-BIT ALU GROUP (0x80 - 0xBF)
// ==========================================================

// --- ADD (0x80 - 0x87) ---
void CPU::OP_80() { ADD(reg.b); }
void CPU::OP_81() { ADD(reg.c); }
void CPU::OP_82() { ADD(reg.d); }
void CPU::OP_83() { ADD(reg.e); }
void CPU::OP_84() { ADD(reg.h); }
void CPU::OP_85() { ADD(reg.l); }
void CPU::OP_86() { ADD(bus->Read(reg.hl)); }
void CPU::OP_87() { ADD(reg.a); }

// --- ADC (0x88 - 0x8F) ---
void CPU::OP_88() { ADDC(reg.b); }
void CPU::OP_89() { ADDC(reg.c); }
void CPU::OP_8A() { ADDC(reg.d); }
void CPU::OP_8B() { ADDC(reg.e); }
void CPU::OP_8C() { ADDC(reg.h); }
void CPU::OP_8D() { ADDC(reg.l); }
void CPU::OP_8E() { ADDC(bus->Read(reg.hl)); }
void CPU::OP_8F() { ADDC(reg.a); }

// --- SUB (0x90 - 0x97) ---
void CPU::OP_90() { SUB(reg.b); }
void CPU::OP_91() { SUB(reg.c); }
void CPU::OP_92() { SUB(reg.d); }
void CPU::OP_93() { SUB(reg.e); }
void CPU::OP_94() { SUB(reg.h); }
void CPU::OP_95() { SUB(reg.l); }
void CPU::OP_96() { SUB(bus->Read(reg.hl)); }
void CPU::OP_97() { SUB(reg.a); }

// --- SBC (0x98 - 0x9F) ---
void CPU::OP_98() { SUBC(reg.b); }
void CPU::OP_99() { SUBC(reg.c); }
void CPU::OP_9A() { SUBC(reg.d); }
void CPU::OP_9B() { SUBC(reg.e); }
void CPU::OP_9C() { SUBC(reg.h); }
void CPU::OP_9D() { SUBC(reg.l); }
void CPU::OP_9E() { SUBC(bus->Read(reg.hl)); }
void CPU::OP_9F() { SUBC(reg.a); }

// --- AND (0xA0 - 0xA7) ---
void CPU::OP_A0() { AND(reg.b); }
void CPU::OP_A1() { AND(reg.c); }
void CPU::OP_A2() { AND(reg.d); }
void CPU::OP_A3() { AND(reg.e); }
void CPU::OP_A4() { AND(reg.h); }
void CPU::OP_A5() { AND(reg.l); }
void CPU::OP_A6() { AND(bus->Read(reg.hl)); }
void CPU::OP_A7() { AND(reg.a); }

// --- XOR (0xA8 - 0xAF) ---
void CPU::OP_A8() { XOR(reg.b); }
void CPU::OP_A9() { XOR(reg.c); }
void CPU::OP_AA() { XOR(reg.d); }
void CPU::OP_AB() { XOR(reg.e); }
void CPU::OP_AC() { XOR(reg.h); }
void CPU::OP_AD() { XOR(reg.l); }
void CPU::OP_AE() { XOR(bus->Read(reg.hl)); }
void CPU::OP_AF() { XOR(reg.a); }

// --- OR (0xB0 - 0xB7) ---
void CPU::OP_B0() { OR(reg.b); }
void CPU::OP_B1() { OR(reg.c); }
void CPU::OP_B2() { OR(reg.d); }
void CPU::OP_B3() { OR(reg.e); }
void CPU::OP_B4() { OR(reg.h); }
void CPU::OP_B5() { OR(reg.l); }
void CPU::OP_B6() { OR(bus->Read(reg.hl)); }
void CPU::OP_B7() { OR(reg.a); }

// --- CP (0xB8 - 0xBF) ---
void CPU::OP_B8() { CMP(reg.b); }
void CPU::OP_B9() { CMP(reg.c); }
void CPU::OP_BA() { CMP(reg.d); }
void CPU::OP_BB() { CMP(reg.e); }
void CPU::OP_BC() { CMP(reg.h); }
void CPU::OP_BD() { CMP(reg.l); }
void CPU::OP_BE() { CMP(bus->Read(reg.hl)); }
void CPU::OP_BF() { CMP(reg.a); }


// ==========================================================
// CONTROL FLOW & I/O (0xC0 - 0xFF)
// ==========================================================

// --- 0xC0 - 0xCF ---
void CPU::OP_C0() { if (!reg.GetZ()) RET(); else totalCyclesForInstruction -= 12; } // RET NZ: 20 -> 8
void CPU::OP_C1() { reg.bc = PopWord(); } // POP BC
void CPU::OP_C2() { uint16_t addr = FetchWord(); if (!reg.GetZ()) JP(addr); else totalCyclesForInstruction -= 4; } // JP NZ: 16 -> 12
void CPU::OP_C3() { JP(FetchWord()); } // JP a16
void CPU::OP_C4() { uint16_t addr = FetchWord(); if (!reg.GetZ()) CALL(addr); else totalCyclesForInstruction -= 12; } // CALL NZ: 24 -> 12
void CPU::OP_C5() { PushWord(reg.bc); } // PUSH BC
void CPU::OP_C6() { ADD(FetchByte()); }
void CPU::OP_C7() { RST(0x00); }

void CPU::OP_C8() { if (reg.GetZ()) RET(); else totalCyclesForInstruction -= 12; } // RET Z: 20 -> 8
void CPU::OP_C9() { RET(); }
void CPU::OP_CA() { uint16_t addr = FetchWord(); if (reg.GetZ()) JP(addr); else totalCyclesForInstruction -= 4; } // JP Z: 16 -> 12
//void CPU::OP_CB() {  } // At the bottom of the file
void CPU::OP_CC() { uint16_t addr = FetchWord(); if (reg.GetZ()) CALL(addr); else totalCyclesForInstruction -= 12; } // CALL Z: 24 -> 12
void CPU::OP_CD() { CALL(FetchWord()); }
void CPU::OP_CE() { ADDC(FetchByte()); }
void CPU::OP_CF() { RST(0x08); }

// --- 0xD0 - 0xDF ---
void CPU::OP_D0() { if (!reg.GetC()) RET(); else totalCyclesForInstruction -= 12; } // RET NC: 20 -> 8
void CPU::OP_D1() { reg.de = PopWord(); } // POP DE
void CPU::OP_D2() { uint16_t addr = FetchWord(); if (!reg.GetC()) JP(addr); else totalCyclesForInstruction -= 4; } // JP NC: 16 -> 12
void CPU::OP_D3() {} // Unused
void CPU::OP_D4() { uint16_t addr = FetchWord(); if (!reg.GetC()) CALL(addr); else totalCyclesForInstruction -= 12; } // CALL NC: 24 -> 12
void CPU::OP_D5() { PushWord(reg.de); } // PUSH DE
void CPU::OP_D6() { SUB(FetchByte()); }
void CPU::OP_D7() { RST(0x10); }

void CPU::OP_D8() { if (reg.GetC()) RET(); else totalCyclesForInstruction -= 12; } // RET C: 20 -> 8
// --- CHANGED: RETI enables interrupts IMMEDIATELY ---
void CPU::OP_D9() { RET(); ime = true; } // RETI
void CPU::OP_DA() { uint16_t addr = FetchWord(); if (reg.GetC()) JP(addr); else totalCyclesForInstruction -= 4; } // JP C: 16 -> 12
void CPU::OP_DB() {} // Unused
void CPU::OP_DC() { uint16_t addr = FetchWord(); if (reg.GetC()) CALL(addr); else totalCyclesForInstruction -= 12; } // CALL C: 24 -> 12
void CPU::OP_DD() {} // Unused
void CPU::OP_DE() { SUBC(FetchByte()); }
void CPU::OP_DF() { RST(0x18); }

// --- 0xE0 - 0xEF ---
void CPU::OP_E0() { bus->Write(0xFF00 + FetchByte(), reg.a); } // LDH (n8), A
void CPU::OP_E1() { reg.hl = PopWord(); } // POP HL
void CPU::OP_E2() { bus->Write(0xFF00 + reg.c, reg.a); } // LD (C), A
void CPU::OP_E3() {} // Unused
void CPU::OP_E4() {} // Unused
void CPU::OP_E5() { PushWord(reg.hl); } // PUSH HL
void CPU::OP_E6() { AND(FetchByte()); }
void CPU::OP_E7() { RST(0x20); }

void CPU::OP_E8() { ADD_SP_e8((int8_t)FetchByte()); }
void CPU::OP_E9() { JP(reg.hl); } // JP (HL)
void CPU::OP_EA() { bus->Write(FetchWord(), reg.a); } // LD (a16), A
// EB, EC, ED unused
void CPU::OP_EE() { XOR(FetchByte()); }
void CPU::OP_EF() { RST(0x28); }

// --- 0xF0 - 0xFF ---
void CPU::OP_F0() { reg.a = bus->Read(0xFF00 + FetchByte()); } // LDH A, (n8)
void CPU::OP_F1() { reg.af = PopWord(); reg.f &= 0xF0; } // POP AF (Clean flags)
void CPU::OP_F2() { reg.a = bus->Read(0xFF00 + reg.c); } // LD A, (C)
void CPU::OP_F3() { DI(); }
void CPU::OP_F4() {} // Unused
void CPU::OP_F5() { PushWord(reg.af); } // PUSH AF
void CPU::OP_F6() { OR(FetchByte()); }
void CPU::OP_F7() { RST(0x30); }

void CPU::OP_F8() { LD_HL_SP_e8((int8_t)FetchByte()); }
void CPU::OP_F9() { reg.sp = reg.hl; } // LD SP, HL
void CPU::OP_FA() { reg.a = bus->Read(FetchWord()); } // LD A, (a16)
void CPU::OP_FB() { EI(); }
void CPU::OP_FC() {} // Unused
void CPU::OP_FD() {} // Unused
void CPU::OP_FE() { CMP(FetchByte()); }
void CPU::OP_FF() { RST(0x38); }


// Our special CB
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