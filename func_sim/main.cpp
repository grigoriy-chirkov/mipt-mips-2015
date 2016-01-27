/**
 * main.cpp - runs MIPS single-cycle simulator
 * @author Grigoriy Chirkov <grigoriy.chirkov@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab project
 */

// Generic C
#include <cstdlib>

// Generic C++
#include <string>

// MIPT-MIPS modules
#include "func_sim.h"

int main( int argc, const char* argv[])
{
    const int args_expected = 3;
    if ( argc < args_expected)
    {
        std::cout << "Not enough args";
        exit( 1);
    }
    MIPS* mips = new MIPS;
    mips->run( string( argv[1]), atoi( argv[2]));
    return 0;
}
