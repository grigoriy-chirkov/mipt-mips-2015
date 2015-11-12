/**
 * func_instr.cpp - the module implementing the MIPS assembler.
 * @author Grigoriy Chirkov <grigoriy.chirkov@phystech.edu>
 * Copyright 2015 MIPT-MIPS iLab project
 */

// Generic C
#include <cassert>
#include <cerrno> 
#include <cstdlib>

// Generic C++
#include <string>

// MIPT-MIPS modules
#include "types.h"
#include "elf_parser.h"
#include "func_memory.h"
#include "func_instr.h"

using namespace std;

const FuncInstr::ISAEntry FuncInstr::isa_table[] =
{
    /* name      opcode     func       format               type       output format*/
    { "add "    , 0x00,     0x20, FuncInstr::FORMAT_R, FuncInstr::ADD, FuncInstr::dst_OUT },
    { "addu "   , 0x00,     0x21, FuncInstr::FORMAT_R, FuncInstr::ADD, FuncInstr::dst_OUT },
    { "sub "    , 0x00,     0x22, FuncInstr::FORMAT_R, FuncInstr::SUB, FuncInstr::dst_OUT },
    { "subu "   , 0x00,     0x23, FuncInstr::FORMAT_R, FuncInstr::SUB, FuncInstr::dst_OUT },
    { "addi "   , 0x08,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD, FuncInstr::tsC_OUT },
    { "addiu "  , 0x09,     0x00, FuncInstr::FORMAT_I, FuncInstr::ADD, FuncInstr::tsC_OUT },
    { "mult "   , 0x00,     0x18, FuncInstr::FORMAT_R, FuncInstr::MUL, FuncInstr::st_OUT  },
    { "multu "  , 0x00,     0x19, FuncInstr::FORMAT_R, FuncInstr::MUL, FuncInstr::st_OUT  },
    { "div "    , 0x00,     0x1a, FuncInstr::FORMAT_R, FuncInstr::DIV, FuncInstr::st_OUT  },
    { "divu "   , 0x00,     0x1b, FuncInstr::FORMAT_R, FuncInstr::DIV, FuncInstr::st_OUT  },
    
    { "mfhi "   , 0x00,     0x10, FuncInstr::FORMAT_R, FuncInstr::MOV, FuncInstr::d_OUT   },
    { "mthi "   , 0x00,     0x11, FuncInstr::FORMAT_R, FuncInstr::MOV, FuncInstr::s_OUT   },
    { "mflo "   , 0x00,     0x12, FuncInstr::FORMAT_R, FuncInstr::MOV, FuncInstr::d_OUT   },
    { "mtlo "   , 0x00,     0x13, FuncInstr::FORMAT_R, FuncInstr::MOV, FuncInstr::s_OUT   },
    
    { "sll "    , 0x00,     0x00, FuncInstr::FORMAT_R, FuncInstr::SH , FuncInstr::dtS_OUT },
    { "srl "    , 0x00,     0x02, FuncInstr::FORMAT_R, FuncInstr::SH , FuncInstr::dtS_OUT },
    { "sra "    , 0x00,     0x03, FuncInstr::FORMAT_R, FuncInstr::SH , FuncInstr::dtS_OUT },
    { "sllv "   , 0x00,     0x04, FuncInstr::FORMAT_R, FuncInstr::SH , FuncInstr::dts_OUT },
    { "srlv "   , 0x00,     0x06, FuncInstr::FORMAT_R, FuncInstr::SH , FuncInstr::dts_OUT },
    { "srav "   , 0x00,     0x07, FuncInstr::FORMAT_R, FuncInstr::SH , FuncInstr::dts_OUT },
    
    { "lui "    , 0x0F,     0x00, FuncInstr::FORMAT_I, FuncInstr::LD , FuncInstr::tC_OUT  },
    { "slt "    , 0x00,     0x2a, FuncInstr::FORMAT_R, FuncInstr::CMP, FuncInstr::dts_OUT },
    { "sltu "   , 0x00,     0x2b, FuncInstr::FORMAT_R, FuncInstr::CMP, FuncInstr::dts_OUT },
    { "slti "   , 0x0a,     0x00, FuncInstr::FORMAT_I, FuncInstr::CMP, FuncInstr::stC_OUT },
    { "sltiu "  , 0x0b,     0x00, FuncInstr::FORMAT_I, FuncInstr::CMP, FuncInstr::stC_OUT },
    
    { "and "    , 0x00,     0x24, FuncInstr::FORMAT_R, FuncInstr::AND, FuncInstr::dts_OUT },
    { "or "     , 0x00,     0x25, FuncInstr::FORMAT_R, FuncInstr::OR , FuncInstr::dts_OUT },
    { "xor "    , 0x00,     0x26, FuncInstr::FORMAT_R, FuncInstr::XOR, FuncInstr::dts_OUT },
    { "nor "    , 0x00,     0x27, FuncInstr::FORMAT_R, FuncInstr::NOR, FuncInstr::dts_OUT },
    { "andi "   , 0x0c,     0x00, FuncInstr::FORMAT_I, FuncInstr::AND, FuncInstr::stC_OUT },
    { "ori "    , 0x0d,     0x00, FuncInstr::FORMAT_I, FuncInstr::OR , FuncInstr::stC_OUT },
    { "xori "   , 0x0e,     0x00, FuncInstr::FORMAT_I, FuncInstr::XOR, FuncInstr::stC_OUT },
    
    { "beq "    , 0x04,     0x00, FuncInstr::FORMAT_I, FuncInstr::CND, FuncInstr::stC_OUT },
    { "bne "    , 0x05,     0x00, FuncInstr::FORMAT_I, FuncInstr::CND, FuncInstr::stC_OUT },
    { "blez "   , 0x06,     0x00, FuncInstr::FORMAT_I, FuncInstr::CND, FuncInstr::sC_OUT  },
    { "bgtz "   , 0x07,     0x00, FuncInstr::FORMAT_I, FuncInstr::CND, FuncInstr::sC_OUT  },
    
    { "j "      , 0x02,     0x00, FuncInstr::FORMAT_J,  FuncInstr::JMP, FuncInstr::A_OUT   },
    { "jal "    , 0x03,     0x00, FuncInstr::FORMAT_J,  FuncInstr::JMP, FuncInstr::A_OUT   },
    { "jr "     , 0x00,     0x08, FuncInstr::FORMAT_R,  FuncInstr::JMP, FuncInstr::s_OUT   },
    { "jalr "   , 0x00,     0x09, FuncInstr::FORMAT_R,  FuncInstr::JMP, FuncInstr::s_OUT   },
    
    { "lb "     , 0x20,     0x00, FuncInstr::FORMAT_I,  FuncInstr::LD , FuncInstr::tC_s_OUT },
    { "lh "     , 0x21,     0x00, FuncInstr::FORMAT_I,  FuncInstr::LD , FuncInstr::tC_s_OUT   },
    { "lw "     , 0x23,     0x00, FuncInstr::FORMAT_I,  FuncInstr::LD , FuncInstr::tC_s_OUT   },
    { "lbu "    , 0x24,     0x00, FuncInstr::FORMAT_I,  FuncInstr::LD , FuncInstr::tC_s_OUT   },
    { "lhu "    , 0x25,     0x00, FuncInstr::FORMAT_I,  FuncInstr::LD , FuncInstr::tC_s_OUT   },
    
    { "sb "     , 0x28,     0x00, FuncInstr::FORMAT_I,  FuncInstr::STR , FuncInstr::tC_s_OUT  },
    { "sh "     , 0x29,     0x00, FuncInstr::FORMAT_I,  FuncInstr::STR , FuncInstr::tC_s_OUT  },
    { "sw "     , 0x2b,     0x00, FuncInstr::FORMAT_I,  FuncInstr::STR , FuncInstr::tC_s_OUT  },
    
    { "syscall ", 0x00,     0x0c, FuncInstr::FORMAT_R,  FuncInstr::INT , FuncInstr::NO_OUT    },
    { "break "  , 0x00,     0x0d, FuncInstr::FORMAT_R,  FuncInstr::INT , FuncInstr::NO_OUT  },
    { "trap "   , 0x1a,     0x00, FuncInstr::FORMAT_J,  FuncInstr::INT , FuncInstr::A_OUT},
    { NULL      , 0x00,     0x00, FuncInstr::NO_FORMAT, FuncInstr::NO_TYPE, FuncInstr::NO_OUT }
};

const char* FuncInstr::reg_name[] =
{
    "$zero",
    "$at",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9",
    "$k0", "$k1",
    "$gp",
    "$sp",
    "$s8",
    "$ra",
    "NOREG"
};

FuncInstr::FuncInstr( uint32 bytes)
{
    this->bytes.raw = bytes;
    int i = 0;
    for ( i = 0; isa_table[i].name != NULL; i++)
    {
        if ( isa_table[i].opcode == this->bytes.asI.opcode)
        {
            if ( isa_table[i].opcode == 0x00)
            {
                if ( isa_table[i].func == this->bytes.asR.func)
                {
                    this->isa_table_num = i;
                    break;
                }
            } else
            {
                this->isa_table_num = i;
                break;
            }
        }
    }
    if ( isa_table[i].name == NULL)
    {
        cerr << "ERROR." << endl;
        exit(1);
    }
}

FuncInstr::~FuncInstr()
{
    //nothing
}

string FuncInstr::Dump( string indent) const
{
    ostringstream oss;
    oss << hex << indent << isa_table[isa_table_num].name;
    switch ( isa_table[isa_table_num].out_format)
    {
        case dst_OUT:
        {
            oss << reg_name[bytes.asR.d_reg] << ", "
                << reg_name[bytes.asR.s_reg] << ", "
                << reg_name[bytes.asR.t_reg];
            break;
        }
        case tsC_OUT:
        {
            oss << reg_name[bytes.asI.t_reg] << ", "
                << reg_name[bytes.asI.s_reg] << ", "
                << "0x" <<  bytes.asI.imm;
            break;
        }
        case st_OUT:
        {
            oss << reg_name[bytes.asR.s_reg] << ", "
                << reg_name[bytes.asR.t_reg];
            break;
        }
        case d_OUT:
        {
            oss << reg_name[bytes.asR.d_reg];
            break;
        }
        case s_OUT:
        {
            oss << reg_name[bytes.asR.s_reg];
            break;
        }
        case dtS_OUT:
        {
            oss << reg_name[bytes.asR.d_reg] << ", "
                << reg_name[bytes.asR.t_reg] << ", " 
                << bytes.asR.shamt;
            break;
        }
        case dts_OUT:
        {
            oss << reg_name[bytes.asR.d_reg] << ", "
                << reg_name[bytes.asR.t_reg] << ", "
                << reg_name[bytes.asR.s_reg];
            break;
        }
        case tC_OUT:
        {
            oss << reg_name[bytes.asI.t_reg] << ", "
                << "0x" << bytes.asI.imm;
            break;
        }
        case stC_OUT:
        {
            oss << reg_name[bytes.asI.s_reg] << ", "
                << reg_name[bytes.asI.t_reg]
                << "0x" << bytes.asI.imm;
            break;
        }
        case sC_OUT:
        {
            oss << reg_name[bytes.asI.s_reg] << ", "
                << "0x" << bytes.asI.imm;
            break;
        }
        case A_OUT:
        {
            oss << "0x" << bytes.asJ.offset;
            break;
        }
        case tC_s_OUT:
        {
            oss << reg_name[bytes.asI.t_reg] << ", "
                << "0x" << bytes.asI.imm << '('
                << reg_name[bytes.asI.s_reg] << ')';
            break;
        }
        case NO_OUT:
        {
            break;   
        }
        default:
        {
            assert( 0);
        }
    }
    return oss.str();
}

ostream& operator<<( ostream& out, const FuncInstr& instr)
{
    out << instr.Dump("");
    return out;
}
