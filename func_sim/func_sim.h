/**
 * func_sim.h - header of module implementing MIPS 
 * single-cycle simulator
 * @author Grigoriy Chirkov <grigoriy.chirkov@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab project
 */

// Protection from multi-include
#ifndef FUNC_SIM__FUNC_SIM_H
#define FUNC_SIM__FUNC_SIM_H

// Generic C

// Generic C++
#include <string>

// MIPT-MIPS modules
#include "types.h"
#include "func_memory.h"
#include "func_instr.h"

enum RegNum : unsigned short
{
    ZERO_REG,
    AT_REG,
    V0_REG, V1_REG,
    A0_REG, A1_REG, A2_REG, A3_REG,
    T0_REG, T1_REG, T2_REG, T3_REG, T4_REG, T5_REG, T6_REG, T7_REG,
    S0_REG, S1_REG, S2_REG, S3_REG, S4_REG, S5_REG, S6_REG, S7_REG,
    T8_REG, T9_REG,
    K0_REG, K1_REG,
    GP_REG,
    SP_REG,
    FP_REG,
    RA_REG,
    MAX_REG
};

class RF
{
    uint32 array[MAX_REG];
    uint32 HI;
    uint32 LO;
public:
    uint32 read( RegNum index) const;
    uint32 read_HI() const;
    uint32 read_LO() const;
    void write( RegNum index, uint32 data);
    void write_HI( uint32 data);
    void write_LO( uint32 data);
    void reset( RegNum index);
    RF();
};


class MIPS
{
    RF* rf;
    uint32 PC;
    FuncMemory* mem;
    
 //   uint32 startPC;
    
    uint32 fetch() const;
    void updatePC( const FuncInstr& instr);
    void read_src( FuncInstr& instr);
    void load( FuncInstr& instr);
    void store( const FuncInstr& instr);
    void ld_st( FuncInstr& instr);
    void wb( const FuncInstr& instr);
public:
    MIPS();
    void run( const string&, uint instr_to_run);
};





#endif //ifndef FUNC_SIM__FUNC_SIM_H


