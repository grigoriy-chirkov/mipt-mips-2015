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