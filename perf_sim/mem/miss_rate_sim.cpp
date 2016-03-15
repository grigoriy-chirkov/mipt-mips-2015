#include "cache_tag_array.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <thread>

const unsigned MIN_SIZE_IN_KB = 1;
const unsigned MIN_SIZE = MIN_SIZE_IN_KB * 1024;

const unsigned MAX_SIZE_IN_KB = 1024;
const unsigned MAX_SIZE = MAX_SIZE_IN_KB*1024;

const unsigned N_OF_SIZES = 11;

const unsigned MAX_N_OF_WAYS = 16;
const unsigned MIN_N_OF_WAYS = 1;

using namespace std;

int main( const int argc, const char* argv[])
{
#define WAITED_N_OF_ARGS 2
    if ( argc - 1 != WAITED_N_OF_ARGS)
    {
        cout << "Usage: ./miss_rate_sim mem_trace.txt miss_rate.csv" << endl;
        return 1;
    }
    
    
    ifstream input;
    input.open( argv[1]);
    if ( !input.is_open())
    {
        cout << "Can't open " << argv[1] << endl;
        return 1;
    }
    input >> hex;
    
    vector<uint64> trace;
    uint64 addr = 0;
    while( input >> addr)
        trace.push_back( addr);
    
    input.close();
    
    
    
    ofstream output;
    output.open( argv[2]);
    if ( !output.is_open())
    {
        cout << "Can't open " << argv[2] << endl;
        return 1;
    }
    
    output << "Associativity\\Size; ";
    for ( unsigned size_in_kb = MIN_SIZE_IN_KB;
          size_in_kb <= MAX_SIZE_IN_KB;
          size_in_kb = size_in_kb << 1          )
        output << size_in_kb << "; ";
    output << "\r\n";
    
    
    for ( unsigned ways = MIN_N_OF_WAYS; ways <= MAX_N_OF_WAYS; ways = ways << 1)
    {
        output << ways << " way; ";
        for ( unsigned size = MIN_SIZE; size <= MAX_SIZE; size = size << 1)
        {
            CacheTagArray tag_array( size, ways);
            double hits = 0;
            double misses = 0;
            for ( vector<uint64>::iterator it = trace.begin();
                 it != trace.end(); it++)
            {
                if ( tag_array.read( *it))
                    hits++;
                else
                    misses++;
            }
            output << misses/( hits + misses)<< "; ";
        }
        output << "\r\n";
    }
    
    
    output << "Full; ";
    for ( unsigned size = MIN_SIZE; size <= MAX_SIZE; size = size << 1)
    {
        CacheTagArray tag_array( size, size >> 2);
        double hits = 0;
        double misses = 0;
        for ( vector<uint64>::iterator it = trace.begin();
             it != trace.end(); it++)
        {
            if ( tag_array.read( *it))
                hits++;
            else
                misses++;
        }
        output << misses/( hits + misses) << "; ";
        cout << tag_array.Dump();
    }
    
    output << "\r\n";
    output.close();
    
    return 0;
}
