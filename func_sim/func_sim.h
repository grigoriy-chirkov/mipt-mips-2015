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

// MIPT-MIPS modules


class MIPS
{
    // storages of internal state
    RF* rf;
    uint32 PC;
    FuncMemory* mem;
    
    
    uint32 fetch() const { return mem->read( PC); }
    void updatePC( const FuncInstr& instr) { PC = instr.new_PC; }
    void read_src( FuncInstr& instr);
    void load( FuncInstr& instr);
    void store( const FuncInstr& instr);
    void ld_st( FuncInstr& instr);
public:
    MIPS();
    void run( const string&, uint instr_to_run);
};



enum RegNum {
    /// ....
    MAX_REG
};

class RF {
    uint32 array[MAX_REG];
public:
    // ...
    uint32 read( RegNum index) const;
    void write( RegNum index, uint32 data);
    void reset( RegNum index); // clears register to 0 value
    // ....
};

#endif //ifndef FUNC_SIM__FUNC_SIM_H


