/**
 * disasm.cpp - the module implementing the MIPS disassembler.
 * @author Grigoriy Chirkov <grigoriy.chirkov@phystech.edu>
 * Copyright 2015 MIPT-MIPS iLab project
 */

// Generic C
#include <cassert>

// Generic C++

// MIPT-MIPS modules
#include "func_instr.h"
#include "types.h"
#include "elf_parser.h"
#include "func_memory.h"

int main( int argc, const char* argv[])
{
    const int expected_arg_count = 2;
    if ( argc != expected_arg_count + 1)
    {
        cout << "Wrong args" << endl;
        exit(1);
    }
    const char* filename = argv[1];
    const char* sect_name = argv[2];
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( filename, sections_array);
    
}
