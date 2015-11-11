/**
 * func_instr.cpp - the module implementing the MIPS assembler.
 * @author Grigoriy Chirkov <grigoriy.chirkov@phystech.edu>
 * Copyright 2015 MIPT-MIPS iLab project
 */

// Generic C
#include <cassert>

// Generic C++
#include <string>

// MIPT-MIPS modules
#include "types.h"
#include "elf_parser.h"
#include "func_memory.h"
#include "func_instr.h"

using namespace std;

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    /* name     opcode     func       format               type      */
    { "add    ", 0x00,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD },
    { "addu   ", 0x00,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADD },
    { "sub    ", 0x00,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB },
    { "subu   ", 0x00,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUB },
    { "addi   ", 0x08,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD },
    { "addiu  ", 0x09,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD },
    { "mult   ", 0x00,     0x18, FuncInstr::FORMAT_R, FuncInstr::MUL },
    { "multu  ", 0x00,     0x19, FuncInstr::FORMAT_R, FuncInstr::MUL },
    { "div    ", 0x00,     0x1a, FuncInstr::FORMAT_R, FuncInstr::DIV },
    { "divu   ", 0x00,     0x1b, FuncInstr::FORMAT_R, FuncInstr::DIV },
    
    { "mfhi   ", 0x00,     0x10, FuncInstr::FORMAT_R, FuncInstr::MOV },
    { "mthi   ", 0x00,     0x11, FuncInstr::FORMAT_R, FuncInstr::MOV },
    { "mflo   ", 0x00,     0x12, FuncInstr::FORMAT_R, FuncInstr::MOV },
    { "mtlo   ", 0x00,     0x13, FuncInstr::FORMAT_R, FuncInstr::MOV },
    
    { "sll    ", 0x00,     0x00, FuncInstr::FORMAT_R, FuncInstr::SH  },
    { "srl    ", 0x00,     0x02, FuncInstr::FORMAT_R, FuncInstr::SH  },
    { "sra    ", 0x00,     0x03, FuncInstr::FORMAT_R, FuncInstr::SH  },
    { "sllv   ", 0x00,     0x04, FuncInstr::FORMAT_R, FuncInstr::SH  },
    { "srlv   ", 0x00,     0x06, FuncInstr::FORMAT_R, FuncInstr::SH  },
    { "srav   ", 0x00,     0x07, FuncInstr::FORMAT_R, FuncInstr::SH  },
    
    { "lui    ", 0x0F,     0x00, FuncInstr::FORMAT_I, FuncInstr::LD  },
    { "slt    ", 0x00,     0x2a, FuncInstr::FORMAT_R, FuncInstr::CMP },
    { "sltu   ", 0x00,     0x2b, FuncInstr::FORMAT_R, FuncInstr::CMP },
    { "slti   ", 0x0a,     0x00, FuncInstr::FORMAT_I, FuncInstr::CMP },
    { "sltiu  ", 0x0b,     0x00, FuncInstr::FORMAT_I, FuncInstr::CMP },
    
    { "and    ", 0x00,     0x24, FuncInstr::FORMAT_R, FuncInstr::AND },
    { "or     ", 0x00,     0x25, FuncInstr::FORMAT_R, FuncInstr::OR  },
    { "xor    ", 0x00,     0x26, FuncInstr::FORMAT_R, FuncInstr::XOR },
    { "nor    ", 0x00,     0x27, FuncInstr::FORMAT_R, FuncInstr::NOR },
    { "andi   ", 0x0c,     0x00, FuncInstr::FORMAT_I, FuncInstr::AND },
    { "ori    ", 0x0d,     0x00, FuncInstr::FORMAT_I, FuncInstr::OR  },
    { "xori   ", 0x0e,     0x00, FuncInstr::FORMAT_I, FuncInstr::XOR },
    
    { "beq    ", 0x04,     0x00, FuncInstr::FORMAT_I, FuncInstr::CND },
    { "bne    ", 0x05,     0x00, FuncInstr::FORMAT_I, FuncInstr::CND },
    { "blez   ", 0x06,     0x00, FuncInstr::FORMAT_I, FuncInstr::CND },
    { "bgtz   ", 0x07,     0x00, FuncInstr::FORMAT_I, FuncInstr::CND },
    
    { "j      ", 0x02,     0x00, FuncInstr::FORMAT_J, FuncInstr::JMP },
    { "jal    ", 0x03,     0x00, FuncInstr::FORMAT_J, FuncInstr::JMP },
    { "jr     ", 0x00,     0x08, FuncInstr::FORMAT_R, FuncInstr::JMP },
    { "jalr   ", 0x00,     0x09, FuncInstr::FORMAT_R, FuncInstr::JMP },
    
    { "lb     ", 0x20,     0x00, FuncInstr::FORMAT_I, FuncInstr::LD  },
    { "lh     ", 0x21,     0x00, FuncInstr::FORMAT_I, FuncInstr::LD  },
    { "lw     ", 0x23,     0x00, FuncInstr::FORMAT_I, FuncInstr::LD  },
    { "lbu    ", 0x24,     0x00, FuncInstr::FORMAT_I, FuncInstr::LD  },
    { "lhu    ", 0x25,     0x00, FuncInstr::FORMAT_I, FuncInstr::LD  },
    
    { "sb     ", 0x28,     0x00, FuncInstr::FORMAT_I, FuncInstr::STR },
    { "sh     ", 0x29,     0x00, FuncInstr::FORMAT_I, FuncInstr::STR },
    { "sw     ", 0x2b,     0x00, FuncInstr::FORMAT_I, FuncInstr::STR },
    
    { "syscall", 0x00,     0x0c, FuncInstr::FORMAT_R, FuncInstr::INT },
    { "break  ", 0x00,     0x0d, FuncInstr::FORMAT_R, FuncInstr::INT },
    { "trap   ", 0x1a,     0x00, FuncInstr::FORMAT_J, FuncInstr::INT }
};

FuncInstr::FuncInstr( uint32 bytes)
{
    this->bytes.raw = bytes;
}

FuncInstr::~FuncInstr()
{
    //nothing
}

string FuncInstr::Dump( string indent) const
{
    return string("Not implemented yet");
}

ostream& operator<<( ostream& out, const FuncInstr& instr)
{
    out << instr.Dump("");
    return out;
}