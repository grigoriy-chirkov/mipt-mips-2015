/**
 * disasm.cpp - the module implementing the MIPS disassembler.
 * @author Grigoriy Chirkov <grigoriy.chirkov@phystech.edu>
 * Copyright 2015 MIPT-MIPS iLab project
 */

// Generic C
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>

// Generic C++

// MIPT-MIPS modules
#include "func_instr.h"
#include "types.h"
#include "elf_parser.h"
#include "func_memory.h"

#define WORD 4

int main( int argc, const char* argv[])
{
    const int expected_arg_count = 2;
    if ( argc != expected_arg_count + 1)
    {
        cout << "Wrong args" << endl;
        return 1;
    }
    const char* filename = argv[1];
    const char* sect_name = argv[2];
   

    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( filename, sections_array);
    int sect_num = 0;
    for ( sect_num = 0; sect_num < sections_array.size(); sect_num++)
    {
        if ( !strcmp( sections_array[sect_num].name, sect_name))
        {
            break;
        }
    }
    assert( sect_num < sections_array.size());
    cout << endl << "    Disassembly section " << sect_name << ":" << endl << endl;
    ElfSection sect = sections_array[sect_num];
    
    cout.fill( '0');
    for ( int pos = 0; pos < sect.size; pos += WORD)
    {
        uint32 bytes = *( ( uint32*)( sect.content + pos));
        if ( bytes == 0x0)
        {
            cout << "          ..." << endl;
            while ( bytes == 0)
            {
                pos += WORD;
                bytes = *( ( uint32*)( sect.content + pos));
                if ( pos >= sect.size)
                {
                    return 0;
                }
            }
        }
        FuncInstr cur_instr( bytes);
        cout << hex << "0x";
        cout.width(8);
        cout << sect.start_addr + pos << ":      0x";
        cout.width(8);
        cout << bytes << "        " << cur_instr << endl;
    }
    return 0;
}
