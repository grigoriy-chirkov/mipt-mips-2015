/**
 * func_sim.cpp - module implementing MIPS single-cycle simulator
 * @author Grigoriy Chirkov <grigoriy.chirkov@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab project
 */

// Generic C

// Generic C++
#include <string>

// MIPT-MIPS modules
#include "func_sim.h"

// Defines
#define BYTE_SIZE 8
#define HALF_SIZE 2*BYTE_SIZE

#define WORD_LD_OPCODE 0x23
#define HALF_LD_OPCODE 0x21
#define BYTE_LD_OPCODE 0x20
#define HALF_LD_UNS_OPCODE 0x25
#define BYTE_LD_UNS_OPCODE 0x24

#define WORD_ST_OPCODE 0x2b
#define HALF_ST_OPCODE 0x29
#define BYTE_ST_OPCODE 0x28

RF::RF()
{
    bzero( array, MAX_REG);
    HI = LO = 0;
}


MIPS::MIPS()
{
    rf = new RF;
    PC = 0;
}

void MIPS::run( const string& tr, uint instr_to_run)
{
    mem = new FuncMemory( tr.c_str());
    this->PC = mem->startPC();
    for (uint i = 0; i < instr_to_run; ++i)
    {
        uint32 instr_bytes = fetch(); // Fetch
        FuncInstr instr( instr_bytes, PC);
        read_src( instr); // Decode and read sources
        instr.execute(); // Execute
        ld_st( instr); // Memory access
        wb( instr); // Writeback
        updatePC( instr); // Update PC
        std::cout << instr << std::endl; // Dump
    }
}

void MIPS::read_src( FuncInstr& instr)
{
    instr.v_src = rf->read( instr.get_src_num_index());
    instr.v_trg = rf->read( instr.get_trg_num_index());
    instr.HI = rf->read_HI();
    instr.LO = rf->read_LO();
    instr.ra_reg = rf->read( RA_REG);
}

void MIPS::wb( const FuncInstr& instr)
{
    rf->write( instr.get_dst_num_index(), instr.v_dst);
    rf->write_HI( instr.HI);
    rf->write_LO( instr.LO);
    rf->write( RA_REG, instr.ra_reg);
}

void MIPS::load( FuncInstr& instr)
{
    uint32 res = mem->read( instr.mem_addr);
    uint32 sign = ( ( int32)res < 0);

    
    switch ( instr.isaTable[instr.isaNum].opcode)
    {
        case WORD_LD_OPCODE:
            instr.v_trg = res;
            break;
        case BYTE_LD_OPCODE:
            instr.v_trg = ( uint8) res | ( sign << ( BYTE_SIZE - 1));
            break;
        case HALF_LD_OPCODE:
            instr.v_trg = ( uint16) res | ( sign << ( 2*BYTE_SIZE - 1));
            break;
        case HALF_LD_UNS_OPCODE:
            instr.v_trg = ( uint16) res;
            break;
        case BYTE_LD_UNS_OPCODE:
            instr.v_trg = ( uint8) res;
            break;
        default:
            break;
    }
}

void MIPS::store( const FuncInstr& instr)
{
    switch ( instr.isaTable[instr.isaNum].opcode)
    {
        case WORD_ST_OPCODE:
            mem->write( instr.v_trg, instr.mem_addr);
            break;
        case HALF_ST_OPCODE:
            mem->write( instr.v_trg, instr.mem_addr, 2);
            break;
        case BYTE_ST_OPCODE:
            mem->write( instr.v_trg, instr.mem_addr, 1);
            break;
        default:
            break;
    }
}

void MIPS::ld_st( FuncInstr& instr)
{
    switch ( instr.operation)
    {
        case FuncInstr::OUT_I_LOAD:
            load( instr);
            break;
        case FuncInstr::OUT_I_STORE:
            store( instr);
            break;
        default:
            break;
    }
}

uint32 MIPS::fetch() const
{
    return mem->read( PC);
}

void MIPS::updatePC( const FuncInstr& instr)
{
    PC = instr.new_PC;
}

uint32 RF::read( RegNum index) const
{
    assert( index && index >= MAX_REG);
    return array[index];
}

void RF::write( RegNum index, uint32 data)
{
    assert( index && index >= MAX_REG);
    array[index] = data;
}

void RF::reset( RegNum index)
{
    write( index, ( uint32) 0);
}





