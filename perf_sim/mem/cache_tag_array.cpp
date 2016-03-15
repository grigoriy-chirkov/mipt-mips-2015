/*
 * cache_array_tag.cpp - mips cache perf simulator
 * @author Grigoriy Chirkov grigoriy.chirkov@phystech.edu
 * Copyright 2015-2016 MIPT-MIPS
 */


#include "cache_tag_array.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

CacheTagArray::CacheTagArray( uint64 size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits    ):
size_in_bytes( size_in_bytes), ways( ways),
block_size_in_bytes( block_size_in_bytes),
addr_size_in_bits( addr_size_in_bits)
{
    
    assert( block_size_in_bytes);
    assert( ways);
    assert( size_in_bytes);
    assert( addr_size_in_bits);
    
    way_size = size_in_bytes / ways;
    n_of_sets = way_size / block_size_in_bytes;
    
    block_size_in_bits = ilogb( block_size_in_bytes);
    set_size_in_bits = ilogb( n_of_sets);
    tag_size_in_bits = addr_size_in_bits - block_size_in_bits - set_size_in_bits;
    assert( block_size_in_bits);
    assert( tag_size_in_bits);

    max_addr = ( ( uint64)1 << addr_size_in_bits) - 1;
    max_set = n_of_sets - 1;
    max_tag = ( ( uint64)1 << tag_size_in_bits) - 1;
    
    tag_arrays = new ( std::nothrow) TagArrayEntry*[ n_of_sets];
    assert( tag_arrays);
    
    for ( int i = 0; i < n_of_sets; i++)
    {
        tag_arrays[i] = new ( std::nothrow) TagArrayEntry[ ways];
        assert( tag_arrays[i]);
    }
}

CacheTagArray::~CacheTagArray()
{
    for ( int i = 0; i < n_of_sets; i++)
    {
        delete tag_arrays[i];
    }
    delete tag_arrays;
}

bool CacheTagArray::read( uint64 addr)
{
    assert( addr <= max_addr);
    uint64 tag = getTag( addr);
    uint64 set = getSet( addr);
    bool success = false;
    for ( unsigned i = 0; i < ways; i++)
    {
        TagArrayEntry& tag_entry = tag_arrays[set][i];
        tag_entry.incCounter();
        if ( tag_entry == tag)
        {
            tag_entry.resetCounter();
            success = true;
        }
    }
    if ( !success)
        write( addr);
    return success;
}

void CacheTagArray::write( uint64 addr)
{
    assert( addr <= max_addr);
    uint64 tag = getTag( addr);
    uint64 set = getSet( addr);
    uint64 way = findLRU( set);
    tag_arrays[set][way] = tag;
}

uint64 CacheTagArray::getTag( uint64 addr)
{
    assert( addr <= max_addr);
    uint64 res = addr >> ( addr_size_in_bits - tag_size_in_bits);
    assert( res <= max_tag);
    return res;
}

uint64 CacheTagArray::getSet( uint64 addr)
{
    assert( addr <= max_addr);
    uint64 res = ( addr >> block_size_in_bits) & max_set;
    assert( res <= max_set);
    return res;
}

void TagArrayEntry::operator = ( uint64 tag)
{
    this->tag = tag;
    this->resetCounter();
}

bool TagArrayEntry::operator == ( uint64 tag)
{
    return this->tag == tag;
}


uint64 CacheTagArray::findLRU( uint64 set)
{
    assert( set < n_of_sets);
    uint64 res = 0;
    for ( unsigned i = 1; i < ways; i++)
        if ( tag_arrays[set][res].isFresher( tag_arrays[set][i]))
            res = i;
    return res;
}

string CacheTagArray::Dump()
{
    ostringstream oss;
    for ( uint64 i = 0; i < n_of_sets; i++)
    {
        for ( uint j = 0; j < ways; j++)
        {
            oss << tag_arrays[i][j] << "\t";
        }
        oss << endl;
    }
    return oss.str();
}

string TagArrayEntry::Dump()
{
    ostringstream oss;
    oss << hex << "0x" << setw( 8) << setfill( '0') << tag << ":" << dec << counter;
    return oss.str();
}




