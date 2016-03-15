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

CacheTagArray::CacheTagArray( unsigned size_in_bytes,
                              unsigned ways,
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
    
    for ( unsigned set = 0; set < n_of_sets; set++)
    {
        tag_arrays[ set] = new ( std::nothrow) TagArrayEntry[ ways];
        assert( tag_arrays[ set]);
    }
    last_addr = 0;
}

CacheTagArray::~CacheTagArray()
{
    for ( unsigned set = 0; set < n_of_sets; set++)
    {
        delete tag_arrays[set];
    }
    delete tag_arrays;
}

bool CacheTagArray::read( uint64 addr)
{
    assert( addr <= max_addr);
    uint64 tag = getTag( addr);
    unsigned set = getSet( addr);
    bool success = false;
    for ( unsigned way = 0; way < ways; way++)
    {
        TagArrayEntry& tag_entry = tag_arrays[set][way];
        tag_entry.incCounter();
        if ( tag_entry == tag)
        {
            tag_entry.resetCounter();
            success = true;
            last_tag_hitted = true;
        }
    }
    if ( !success)
    {
        write( addr);
        last_tag_hitted = false;
    }
    last_addr = addr;
    return success;
}

void CacheTagArray::write( uint64 addr)
{
    assert( addr <= max_addr);
    uint64 tag = getTag( addr);
    unsigned set = getSet( addr);
    unsigned way = findLRU( set);
    tag_arrays[set][way] = tag;
}

uint64 CacheTagArray::getTag( uint64 addr)
{
    assert( addr <= max_addr);
    uint64 res = addr >> ( addr_size_in_bits - tag_size_in_bits);
    assert( res <= max_tag);
    return res;
}

unsigned CacheTagArray::getSet( uint64 addr)
{
    assert( addr <= max_addr);
    unsigned res = ( addr >> block_size_in_bits) & max_set;
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


unsigned CacheTagArray::findLRU( unsigned set)
{
    assert( set < n_of_sets);
    unsigned res = 0;
    for ( unsigned way = 1; way < ways; way++)
        if ( tag_arrays[set][res].isFresher( tag_arrays[set][way]))
            res = way;
    return res;
}

string CacheTagArray::Dump()
{
    ostringstream oss;
    oss << "-----------------------------------------" << endl;
    oss << "CacheTagArray Dump" << endl;
    oss << "Size of cache: " << ( size_in_bytes >> 10) << "KB" << endl;
    oss << "Ways: " << ways << endl;
    oss << "Block size in bytes: " << block_size_in_bytes << endl;
    oss << "Address size in bits: " << addr_size_in_bits << ", consists of:" << endl;
    oss << "Tag: " << tag_size_in_bits << " bits, "
        << "Set: " << set_size_in_bits << " bits, "
        << "Offset: " << block_size_in_bits << " bits." << endl;
    if ( last_tag_hitted)
    {
        oss << "Last read was cache hit, in addr: " ;
    } else
    {
        oss << "Last read was cache miss, in addr: ";
    }
    oss << hex << "0x" << last_addr << endl;
    oss << "End of dump" << endl << "-----------------------------------------" << endl;
    return oss.str();
}