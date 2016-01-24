/**
 * func_sim.cpp - module implementing MIPS single-cycle simulator
 * @author Grigoriy Chirkov <grigoriy.chirkov@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab project
 */

// Generic C

// Generic C++

// MIPT-MIPS modules


void MIPS::run( const string& tr, uint instr_to_run)
{
    // load trace
    this->PC = startPC;
    for (uint i = 0; i < instr_to_run; ++i)
    {
        uint32 instr_bytes = 0;
        // Fetch
        // Decode and read sources
        // Execute
        // Memory access
        // Writeback
        // Update PC
        // Dump
    }
}

void MIPS::read_src( FuncInstr& instr)
{
    // ...
    instr.v_src1 = rf->read( instr.get_src1_num_index());
    instr.v_src2 = rf->read( instr.get_src2_num_index());
    // ...
}

void MIPS::load( FuncInstr& instr)
{
    instr.v_dst = mem->read( instr.mem_addr);
}

void MIPS::store( const FuncInstr& instr)
{
    mem->write( instr.mem_addr, instr.v_dst);
}

void MIPS::ld_st( FuncInstr& instr)
{
    // calls load for loads, store for stores, nothing otherwise
}




