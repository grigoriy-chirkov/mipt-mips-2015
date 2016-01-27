/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */


#include <func_instr.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>

#define MAX_REG 32

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name  opcode  func   format    operation     func_addr
    { "add",    0x0, 0x20,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::add },
    { "addu",   0x0, 0x21,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::addu },
    { "sub",    0x0, 0x22,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::sub },
    { "subu",   0x0, 0x23,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::subu },
    { "addi",   0x8, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::addi },
    { "addiu",  0x9, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::addiu },
    { "mult",   0x0, 0x18,  FORMAT_R, OUT_R_MUL,    &FuncInstr::mult },
    { "multu",  0x0, 0x19,  FORMAT_R, OUT_R_MUL,    &FuncInstr::multu },
    { "div",    0x0, 0x1A,  FORMAT_R, OUT_R_MUL,    &FuncInstr::div },
    { "divu",   0x0, 0x1B,  FORMAT_R, OUT_R_MUL,    &FuncInstr::divu },
    { "mfhi",   0x0, 0x10,  FORMAT_R, OUT_R_FHILO,   &FuncInstr::mfhi },
    { "mthi",   0x0, 0x11,  FORMAT_R, OUT_R_THILO,   &FuncInstr::mthi },
    { "mflo",   0x0, 0x12,  FORMAT_R, OUT_R_FHILO,   &FuncInstr::mflo },
    { "mtlo",   0x0, 0x13,  FORMAT_R, OUT_R_THILO,   &FuncInstr::mtlo },
    { "sll",    0x0, 0x0,   FORMAT_R, OUT_R_SHAMT,  &FuncInstr::sll },
    { "srl",    0x0, 0x2,   FORMAT_R, OUT_R_SHAMT,  &FuncInstr::srl },
    { "sra",    0x0, 0x3,   FORMAT_R, OUT_R_SHAMT,  &FuncInstr::sra },
    { "sllv",   0x0, 0x4,   FORMAT_R, OUT_R_ARITHM, &FuncInstr::sllv },
    { "srlv",   0x0, 0x6,   FORMAT_R, OUT_R_ARITHM, &FuncInstr::srlv },
    { "srav",   0x0, 0x7,   FORMAT_R, OUT_R_ARITHM, &FuncInstr::srav },
    { "lui",    0xF, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::lui },
    { "slt",    0x0, 0x2A,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::slt },
    { "sltu",   0x0, 0x2B,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::sltu },
    { "slti",   0xA, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::slti },
    { "sltiu",  0xB, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::sltiu },
    { "and",    0x0, 0x24,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::_and },
    { "or",     0x0, 0x25,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::_or },
    { "xor",    0x0, 0x26,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::_xor },
    { "nor",    0x0, 0x27,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::nor },
    { "andi",   0xC, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::andi },
    { "ori",    0xD, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::ori },
    { "xori",   0xE, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::xori },
    { "beq",    0x4, 0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::beq },
    { "bne",    0x5, 0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::bne },
    { "blez",   0x6, 0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::blez },
    { "bgtz",   0x7, 0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::bgtz },
    { "jal",    0x3, 0x0,   FORMAT_J, OUT_J_JUMP,   &FuncInstr::jal },
    { "j",      0x2, 0x0,   FORMAT_J, OUT_J_JUMP,   &FuncInstr::jump },
    { "jr",     0x0, 0x8,   FORMAT_R, OUT_R_JUMP,   &FuncInstr::jr },
    { "jalr",   0x0, 0x9,   FORMAT_R, OUT_R_JUMP,   &FuncInstr::jalr },
    { "lb",     0x20,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lb },
    { "lh",     0x21,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lh },
    { "lw",     0x23,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lw },
    { "lbu",    0x24,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lbu },
    { "lhu",    0x25,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lhu },
    { "sb",     0x28,0x0,   FORMAT_I, OUT_I_STORE,  &FuncInstr::sb },
    { "sh",     0x29,0x0,   FORMAT_I, OUT_I_STORE,  &FuncInstr::sh },
    { "sw",     0x2b,0x0,   FORMAT_I, OUT_I_STORE,  &FuncInstr::sw },
    { "syscall",0x0, 0xC,   FORMAT_R, OUT_R_SPECIAL,&FuncInstr::syscall },
    { "break",  0x0, 0xD,   FORMAT_R, OUT_R_SPECIAL,&FuncInstr::_break },
    { "trap",   0x1A,0x0,   FORMAT_J, OUT_J_SPECIAL, &FuncInstr::trap },
};
const uint32 FuncInstr::isaTableSize = sizeof(isaTable) / sizeof(isaTable[0]);

const char *FuncInstr::regTable[] =
{
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9",
    "k0", "k1",
    "gp",
    "sp",
    "fp",
    "ra"
};

FuncInstr::FuncInstr( uint32 bytes, uint32 PC) : instr( bytes), PC( PC)
{
    initFormat();
    switch ( format)
    {
        case FORMAT_R:
            initR();
            break;
        case FORMAT_I:
            initI();
            break;
        case FORMAT_J:
            initJ();
            break;
        case FORMAT_UNKNOWN:
            initUnknown();
            break;
    }
}

std::string FuncInstr::Dump( std::string indent) const
{
    std::ostringstream oss;
    oss << hex << setfill( '0') << "0x" << setw(8) << PC
        << setfill( ' ') << setw( 9);
    if ( instr.raw == 0)
    {
        oss << "nop";
        return indent + oss.str();
    }
    oss << isaTable[isaNum].name << setfill( '0');
    switch ( operation)
    {
        case OUT_R_ARITHM:
            oss << " [0x" << setw( 8) << v_dst << "], $"
                << regTable[instr.asR.rs]
                << " [0x" << setw( 8) << v_src << "], $"
                << regTable[instr.asR.rt]
                << " [0x" << setw( 8) << v_trg << "]";
            break;
        case OUT_R_SHAMT:
            oss << " $" << regTable[instr.asR.rd]
                << " [0x" << setw( 8) << v_dst << "], $"
                << regTable[instr.asR.rt]
                << " [0x" << setw( 8) << v_trg << "], "
                << dec << instr.asR.shamt << hex;
            break;
        case OUT_R_JUMP:
            oss << " $" << regTable[instr.asR.rs]
                << " [" << setw( 8) << v_src
                << "]";
            break;
        case OUT_R_THILO:
            oss << " $" << regTable[instr.asR.rs]
                << " [0x" << setw( 8) << v_src
                << "]";
            break;
        case OUT_R_FHILO:
            oss << " $" << regTable[instr.asR.rd]
                << " [0x" << setw( 8) << v_dst
                << "]";
            break;
        case OUT_R_MUL:
            oss << " $" << regTable[instr.asR.rs]
                << " [0x" << setw( 8) << v_src << "], $"
                << regTable[instr.asR.rt]
                << " [0x" << setw( 8) << v_trg << "]";
            break;
        case OUT_R_SPECIAL:
            break;
        case OUT_I_ARITHM:
            oss << " $" << regTable[instr.asI.rt]
                << " [0x" << setw( 8) << v_trg << "], $"
                << regTable[instr.asI.rs]
                << " [0x" << setw( 8) << v_src << "], "
                << "0x" << static_cast< signed int>( instr.asI.imm);
            break;
        case OUT_I_BRANCH:
            oss << " $" << regTable[instr.asI.rs]
                << " [0x" << setw( 8) << v_src << "], $"
                << regTable[instr.asI.rt]
                << " [0x" << setw( 8) << v_trg << "], "
                << "0x" << static_cast< signed int>( instr.asI.imm);
            break;
        case OUT_I_LOAD:
            oss << " $" << regTable[instr.asI.rt]
                << " [0x" << setw( 8) << v_trg << "], "
                << "0x" << static_cast< signed int>( instr.asI.imm)
                << "($" << regTable[instr.asI.rs]
                << " [0x" << setw( 8) << v_src << "]" << ")";
            break;
        case OUT_I_STORE:
            oss << " $" << regTable[instr.asI.rt]
                << " [0x" << setw( 8) << v_trg << "], "
                << "0x" << static_cast< signed int>( instr.asI.imm)
                << "($" << regTable[instr.asI.rs]
                << " [0x" << setw( 8) << v_src << "]" << ")";
            break;
        case OUT_J_JUMP:
            oss << " " << "0x" << instr.asJ.imm;
            break;
        case OUT_J_SPECIAL:
            break;
        default:
            assert( 0);
    }
    return indent + oss.str();
    //return indent + disasm;
}

void FuncInstr::initFormat()
{
    for ( size_t i = 0; i < isaTableSize; i++) {
        if ( instr.asR.opcode == isaTable[i].opcode)
        {
            format = isaTable[i].format;
            operation = isaTable[i].operation;
            isaNum = i;
            return;
        }
    }
    format = FORMAT_UNKNOWN;
}


void FuncInstr::initR()
{
    // find instr by functor
    for (isaNum = 0; isaNum < isaTableSize; ++isaNum) {
        if (( instr.asR.opcode == isaTable[isaNum].opcode) &&
            ( instr.asR.funct == isaTable[isaNum].funct))
        {
            operation = isaTable[isaNum].operation;
            break;
        }
    }
    if ( isaNum == isaTableSize)     // if didn't found funct
    {
        initUnknown();
        return;
    }

    ostringstream oss;
    oss << isaTable[isaNum].name;
    switch ( operation)
    {
        case OUT_R_ARITHM:
            oss << " $" << regTable[instr.asR.rd] << ", $" \
                        << regTable[instr.asR.rs] << ", $" \
                        << regTable[instr.asR.rt];
            break;
        case OUT_R_SHAMT:
            oss << " $" << regTable[instr.asR.rd] << ", $" \
                        << regTable[instr.asR.rt] << ", " \
                        << dec << instr.asR.shamt;
            break;
        case OUT_R_JUMP:
            oss << " $" << regTable[instr.asR.rs];
            break;
        case OUT_R_SPECIAL:
            break;
    }
    disasm = oss.str();

    v_src = instr.asR.rs;
    v_trg = instr.asR.rt;
    v_dst = instr.asR.rd;
    ra_reg = HI = LO = mem_addr = imm = 0;
    shamt = instr.asR.shamt;
    new_PC = PC + 4;
}


void FuncInstr::initI()
{
    std::ostringstream oss;
    oss << isaTable[isaNum].name << " $";
    switch ( operation)
    {
        case OUT_I_ARITHM:
            oss << regTable[instr.asI.rt] << ", $"
                << regTable[instr.asI.rs] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_BRANCH:
            oss << regTable[instr.asI.rs] << ", $"
                << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
            break;
        case OUT_I_LOAD:
            oss << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm)
                << std::dec << "($" << regTable[instr.asI.rs] << ")";
            break;
        case OUT_I_STORE:
            oss << regTable[instr.asI.rt] << ", "
                << std::hex << "0x" << static_cast< signed int>( instr.asI.imm)
                << std::dec << "($" << regTable[instr.asI.rs] << ")";
            break;
    }
    disasm = oss.str();

    v_trg = instr.asI.rt;
    v_src = instr.asI.rs;
    imm = instr.asI.imm;
    shamt = ra_reg = HI = LO = v_dst = mem_addr = 0;
    new_PC = PC + 4;

}

void FuncInstr::initJ()
{
    std::ostringstream oss;
    oss << isaTable[isaNum].name;
    switch ( operation)
    {
        case OUT_J_JUMP:
            oss << " " << std::hex << "0x" <<instr.asJ.imm;
            break;
        case OUT_J_SPECIAL:
            break;
    }
    disasm = oss.str();

    new_PC = PC + 4;
    ra_reg = v_src = v_dst = v_trg = HI = LO = mem_addr = shamt = 0;
    imm = instr.asJ.imm;
}

void FuncInstr::initUnknown()
{
    std::ostringstream oss;
    oss << std::hex << std::setfill( '0')
        << "0x" << std::setw( 8) << instr.raw << '\t' << "Unknown" << std::endl;
    disasm = oss.str();
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit(EXIT_FAILURE);
}

std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}

int FuncInstr::get_src_num_index() const
{
    if ( format == FORMAT_J)
        return MAX_REG;
    else
        return instr.asR.rs;
}

int FuncInstr::get_trg_num_index() const
{
    if ( format == FORMAT_J)
        return MAX_REG;
    else
        return instr.asR.rt;
}

int FuncInstr::get_dst_num_index() const
{
    if ( format == FORMAT_R)
        return instr.asR.rd;
    else
        return MAX_REG;
}


/* Operations */

// Add/subtract
void FuncInstr::add()
{
    v_dst = ( uint32)( ( int32)v_src + ( int32)v_trg);
}

void FuncInstr::sub()
{
    v_dst = ( uint32)( ( int32)v_src - ( int32)v_trg);
}

void FuncInstr::addi()
{
    v_trg = ( uint32)( ( int32)v_src + ( int16)imm);
}

void FuncInstr::addu()
{
    v_dst = v_src + v_trg;
}

void FuncInstr::subu()
{
    v_dst = v_src - v_trg;
}

void FuncInstr::addiu()
{
    v_trg = v_src + imm;
}

// Multiplication/division
void FuncInstr::mult()
{
    int64 res = ( int32)v_src * ( int32) v_trg;
    LO = ( uint32) ( res & 0xffffffff);
    HI = ( uint32) ( res >> 0x20);
}

void FuncInstr::div()
{
    assert( v_trg);
    LO = ( uint32)( ( int32)v_src / ( int32)v_trg);
    HI = ( uint32)( ( int32)v_src % ( int32)v_trg);
}


void FuncInstr::mfhi()
{
    v_dst = HI;
}

void FuncInstr::mthi()
{
    HI = v_src;
}

void FuncInstr::mtlo()
{
    LO = v_src;
}

void FuncInstr::mflo()
{
    v_dst = LO;
}

void FuncInstr::multu()
{
    uint64 res = v_src * v_trg;
    LO = res & 0xffffffff;
    HI = res >> 0x20;
}

void FuncInstr::divu()
{
    assert( v_trg);
    LO = v_src / v_trg;
    HI = v_src % v_trg;
}

// Shifts
void FuncInstr::sll()
{
    v_dst = v_trg << shamt;
}

void FuncInstr::srl()
{
    v_dst = v_trg >> shamt;
}

void FuncInstr::sra()
{
    v_dst = ( uint32)(( int32)v_trg >> shamt);
}

void FuncInstr::sllv()
{
    v_dst = v_trg << v_src;
}

void FuncInstr::srlv()
{
    v_dst = v_trg >> v_src;
}

void FuncInstr::srav()
{
    v_dst = ( uint32)( ( int32)v_trg >> v_src);
}

void FuncInstr::lui()
{
    v_trg = imm << 16;
}

// Comparisons
void FuncInstr::slt()
{
    v_dst = ( ( int32)v_src < (int32)v_trg);
}

void FuncInstr::sltu()
{
    v_dst = ( v_src < v_trg);
}

void FuncInstr::slti()
{
    v_trg = ( ( int32)v_src < ( int32)imm);
}

void FuncInstr::sltiu()
{
    v_trg = ( v_src < imm);
}

// Logical operations
void FuncInstr::_and()
{
    v_dst = v_src & v_trg;
}

void FuncInstr::_or()
{
    v_dst = v_src | v_trg;
}

void FuncInstr::_xor()
{
    v_dst = v_src ^ v_trg;
}

void FuncInstr::nor()
{
    v_dst = ~( v_src | v_trg);
}

void FuncInstr::andi()
{
    v_trg = v_src & imm;
}

void FuncInstr::ori()
{
    v_trg = v_src | imm;
}

void FuncInstr::xori()
{
    v_trg = v_src ^ imm;
}

// Conditional branches
void FuncInstr::beq()
{
    if ( v_src == v_trg)
    {
        new_PC += ( imm << 2);
    }
}

void FuncInstr::bne()
{
    if ( v_src != v_trg)
    {
        new_PC += ( imm << 2);
    }
}

void FuncInstr::blez()
{
    if ( v_src <= 0)
    {
        new_PC += ( imm << 2);
    }
}

void FuncInstr::bgtz()
{
    if ( v_src > 0)
    {
        new_PC += ( imm << 2);
    }
}

// Unconditional jump
void FuncInstr::jump()
{
    new_PC = (PC & 0xf0000000) | ( imm << 2);
}

void FuncInstr::jal()
{
    new_PC = (PC & 0xf0000000) | ( imm << 2);
    ra_reg = PC + 4;
}

void FuncInstr::jr()
{
    new_PC = v_src;
}

void FuncInstr::jalr()
{
    new_PC = v_src;
    ra_reg = PC + 4;
}

// Loads
void FuncInstr::lb()
{
    mem_addr = v_src + imm;
}

void FuncInstr::lh()
{
    mem_addr = v_src + imm;
}

void FuncInstr::lw()
{
    mem_addr = v_src + imm;
}

void FuncInstr::lbu()
{
    mem_addr = v_src + imm;
}

void FuncInstr::lhu()
{
    mem_addr = v_src + imm;
}

// Stores
void FuncInstr::sb()
{
    mem_addr = v_src + imm;
}

void FuncInstr::sh()
{
    mem_addr = v_src + imm;
}

void FuncInstr::sw()
{
    mem_addr = v_src + imm;
}

// Special instructions
void FuncInstr::syscall()
{

}

void FuncInstr::_break()
{

}

void FuncInstr::trap()
{

}

void FuncInstr::execute()
{
    ( this->*( isaTable[isaNum].func_addr))();
}
