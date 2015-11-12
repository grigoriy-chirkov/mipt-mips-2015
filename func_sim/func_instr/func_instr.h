/**
 * func_instr.h - Header of modules implementing the MIPS assembler
 * and disassembler.
 * @author Grigoriy Chirkov <grigoriy.chirkov@phystech.edu>
 * Copyright 2015 MIPT-MIPS iLab project
 */

// Protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C
 
// Generic C++
#include <iostream>
#include <sstream>
#include <string>

// MIPT-MIPS modules
#include "types.h"
#include "elf_parser.h"
#include "func_memory.h"

class FuncInstr
{
private:
    enum Format
    {
        NO_FORMAT = 0,
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    } format;
    
    enum Type
    {
        NO_TYPE = 0,
        ADD, SUB, MUL, DIV,
        MOV,
        SH,
        CMP, AND, OR, XOR, NOR,
        JMP, CND,
        LD, STR,
        INT
    };
    
    enum Reg
    {
        $zero,
        $at,
        $v0, $v1,
        $a0, $a1, $a2, $a3,
        $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7,
        $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7,
        $t8, $t9,
        $k0, $k1,
        $gp,
        $sp,
        $s8,
        $ra,
        NOREG
    };

    enum OutFormat
    {
        dst_OUT, tsC_OUT, st_OUT, d_OUT, s_OUT, dtS_OUT, dts_OUT,
        tC_OUT, stC_OUT, sC_OUT, A_OUT, tC_s_OUT, NO_OUT  
    };
    union
    {
        struct
        {
            unsigned imm:16;
            unsigned t_reg:5;
            unsigned s_reg:5;
            unsigned opcode:6;
        } asI;
        struct
        {
            unsigned func:6;
            unsigned shamt:5;
            unsigned d_reg:5;
            unsigned t_reg:5;
            unsigned s_reg:5;
            unsigned opcode:6;
        } asR;
        struct
        {
            unsigned offset:26;
            unsigned opcode:6;
        } asJ;
        uint32 raw;
    } bytes;
    
    struct ISAEntry
    {
        const char* name;
        
        uint8 opcode;
        uint8 func;
        
        FuncInstr::Format format;
        FuncInstr::Type type;
        FuncInstr::OutFormat out_format;
    };
   
    static const ISAEntry isa_table[];
    static const char* reg_name[];

    FuncInstr();
    unsigned isa_table_num;
    
public:
    FuncInstr( uint32 bytes);
    ~FuncInstr();
    std::string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif // ifndef FUNC_INSTR__FUNC_INSTR_H
