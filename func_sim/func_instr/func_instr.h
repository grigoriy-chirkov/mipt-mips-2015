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
#include <ostringstream>
#include <string>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>
#include <func_memory.h>

class FuncInstr
{
private:
    enum Format
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    } format;
    
    enum Type
    {
        ADD, SUB, MUL, DIV,
        MOV,
        SH,
        CMP, AND, OR, XOR, NOR,
        JMP, CND,
        LD, STR,
        INT
    } type;
    
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
        $ra
    } reg;
    
    union
    {
        struct
        {
            unsigned opcode:6;
            unsigned s_reg:5;
            unsigned t_reg:5;
            unsigned imm:16;
        } asI;
        struct
        {
            unsigned opcode:6;
            unsigned s_reg:5;
            unsigned t_reg:5;
            unsigned d_reg:5;
            unsigned shamt:5;
            unsigned func:6;
        } asR;
        struct
        {
            unsigned opcode:6;
            unsigned offset:26;
        } asJ;
        uint32 raw;
    } bytes;
    
    struct ISAEntry
    {
        const char* name;
        
        uint8 opcode;
        uint8 func;
        
        FuncInstr::FormatType format;
        FuncInstr::Type type;
        // ...
    };
    static const ISAEntry[] isaTable;
    
public:
    FuncInstr( uint32 bytes);
    std::string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

const FuncInstr::ISAEntry[] FuncInstr::isaTable =
{
    // name   opcode    func      format              type
    { "add  ", 0x0,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD /*...*/ },
    { "addu ", 0x0,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADD /*...*/ },
    { "sub  ", 0x0,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB /*...*/ },
    { "subu ", 0x0,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUB /*...*/ },
    { "addi ", 0x8,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD /*...*/ },
    { "addiu", 0x9,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD /*...*/ },
    { "mult ", 0x0,     0x18, FuncInstr::FORMAT_R, FuncInstr::MUL /*...*/ },
    { "multu", 0x0,     0x19, FuncInstr::FORMAT_R, FuncInstr::MUL /*...*/ },
    { "div  ", 0x0,     0x1a, FuncInstr::FORMAT_R, FuncInstr::DIV /*...*/ },
    { "divu ", 0x0,     0x1b, FuncInstr::FORMAT_R, FuncInstr::DIV /*...*/ },
    { "mfhi ", 0x0,     0x10, FuncInstr::FORMAT_R, FuncInstr::MOV /*...*/ },
    { "mthi ", 0x0,     0x11, FuncInstr::FORMAT_R, FuncInstr::MOV /*...*/ },
    { "mflo ", 0x0,     0x12, FuncInstr::FORMAT_R, FuncInstr::MOV /*...*/ },
    { "mtlo ", 0x0,     0x13, FuncInstr::FORMAT_R, FuncInstr::MOV /*...*/ },
    { "sll  ", 0x0,     0x00, FuncInstr::FORMAT_R, FuncInstr::SH  /*...*/ },
    { "srl  ", 0x0,     0x02, FuncInstr::FORMAT_R, FuncInstr::SH  /*...*/ },
    { "sra  ", 0x0,     0x03, FuncInstr::FORMAT_R, FuncInstr::SH  /*...*/ },
    { "sllv ", 0x0,     0x04, FuncInstr::FORMAT_R, FuncInstr::SH  /*...*/ },
    { "srlv ", 0x0,     0x06, FuncInstr::FORMAT_R, FuncInstr::SH  /*...*/ },
    { "srav ", 0x0,     0x07, FuncInstr::FORMAT_R, FuncInstr::SH  /*...*/ },
    { "lui  ", 0xF,     0x00, FuncInstr::FORMAT_I, FuncInstr::LD  /*...*/ },
    
    { "slt  ", 0x0,     0x2a, FuncInstr::FORMAT_R, FuncInstr::CMP /*...*/ },
    // more instructions ...
};

#endif // ifndef FUNC_INSTR__FUNC_INSTR_H
