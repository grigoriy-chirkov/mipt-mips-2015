/*
 * cache_array_tag.cpp - mips cache perf simulator
 * @author Grigoriy Chirkov grigoriy.chirkov@phystech.edu
 * Copyright 2015-2016 MIPT-MIPS
 */

#ifndef _CACHE_TAG_ARRAY_H
#define _CACHE_TAG_ARRAY_H
#include <types.h>
#include <new>
#include <iostream>

class TagArrayEntry;

class CacheTagArray
{
public:
    /**
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored in the cache,
     *    i.e. if the block size is 16 Bytes then the number of data blocks in the cache is size_in_bytes/16.
     *
     * 2) ways is a number of associative ways in a set, i.e. how many blocks are referred by the same index.
     *
     * 3) block_size_in_bytes is a number of Bytes in a data block
     *
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( unsigned size_in_bytes,
                   unsigned ways,
                   unsigned short block_size_in_bytes = 4,
                   unsigned short addr_size_in_bits = 32  );
    /**
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     *
     * Note that his method updates the LRU information.
     */
    bool read( uint64 addr);
    
    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache.
     *
     * Note: in order to put the given address inside the tags it is needed
     * to select a way where it will be written in.
     * This selection is being done according to LRU (Least Recently Used)
     * policy.
     */
    void write( uint64 addr);
    ~CacheTagArray();
    string Dump();
private:
    CacheTagArray();
    unsigned size_in_bytes;
    unsigned ways;
    unsigned short block_size_in_bytes;
    unsigned short addr_size_in_bits;
    
    unsigned way_size;
    unsigned n_of_sets;
    
    unsigned short block_size_in_bits;
    unsigned short set_size_in_bits;
    unsigned short tag_size_in_bits;
    
    uint64 max_addr;
    uint64 max_tag;
    unsigned max_set;
    
    bool last_tag_hitted;
    uint64 last_addr;
    
    TagArrayEntry** tag_arrays;
    
    inline uint64 getTag( uint64 addr);
    inline unsigned getSet( uint64 addr);
    unsigned findLRU( unsigned set);
    void loadTag( uint64 tag, unsigned set);
};



class TagArrayEntry
{
private:
    uint64 tag;
    unsigned counter;
public:
    TagArrayEntry(): tag( 0), counter( 0) {};
    void operator = ( uint64 tag);
    bool operator == ( uint64 tag);
    void resetCounter() { counter = 0; };
    void incCounter() { counter++; };
    bool isFresher( TagArrayEntry& comp_tag)
    { return this->counter < comp_tag.counter; };
};



#endif
