/*
 * func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014 MIPT-MIPS
 */


#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <string>
#include <cassert>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>

//enum class RegNum : unsigned short;

class FuncInstr
{
    public:
        friend class MIPS;

        FuncInstr( uint32 bytes, uint32 PC = 0);
        std::string Dump( std::string indent = " ") const;

        int get_src_num_index() const;
        int get_trg_num_index() const;
        int get_dst_num_index() const;

        /* Operations */

        // Add/subtract
        void add();
        void sub();
        void addi();
        void addu();
        void subu();
        void addiu();
        // Multiplication/division
        void mult();
        void div();
        void mfhi();
        void mthi();
        void mflo();
        void mtlo();
        void multu();
        void divu();
        // Shifts
        void sll();
        void srl();
        void sra();
        void sllv();
        void srlv();
        void srav();
        void lui();
        // Comparisons
        void slt();
        void sltu();
        void slti();
        void sltiu();
        // Logical operations
        void _and();
        void _or();
        void _xor();
        void nor();
        void andi();
        void xori();
        void ori();
        // Conditional branches
        void beq();
        void bne();
        void blez();
        void bgtz();
        // Unconditional jump
        void jump();
        void jal();
        void jr();
        void jalr();
        // Loads
        void lb();
        void lh();
        void lw();
        void lbu();
        void lhu();
        // Stores
        void sb();
        void sh();
        void sw();
        // Special instructions
        void syscall();
        void _break();
        void trap();

        void execute();

    private:
        const uint32 PC;
        uint32 v_src;
        uint32 v_trg;
        uint32 v_dst;
        uint32 HI;
        uint32 LO;
        uint32 ra_reg;
        uint32 imm;
        uint32 mem_addr;
        uint32 new_PC;
        uint32 shamt;

        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            FORMAT_UNKNOWN
        } format;

        enum OperationType
        {
            OUT_R_ARITHM,
            OUT_R_SHAMT,
            OUT_R_JUMP,
            OUT_R_SPECIAL,
            OUT_R_THILO,
            OUT_R_FHILO,
            OUT_R_MUL,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_I_LOAD,
            OUT_I_STORE,
            OUT_J_JUMP,
            OUT_J_SPECIAL
        } operation;

        union _instr
        {
            struct
            {
                unsigned funct  :6;
                unsigned shamt  :5;
                unsigned rd     :5;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asR;
            struct
            {
                unsigned imm    :16;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asI;
            struct
            {
                unsigned imm    :26;
                unsigned opcode :6;
            } asJ;
            uint32 raw;

            _instr(uint32 bytes) {
                 raw = bytes;
            }
        } instr;

        struct ISAEntry
        {
            std::string name;

            uint8 opcode;
            uint8 funct;

            Format format;
            OperationType operation;
            void (FuncInstr::*func_addr)(void);
        };
        uint32 isaNum;

        static const ISAEntry isaTable[];
        static const uint32 isaTableSize;
        static const char *regTable[];

        std::string disasm;

        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H
