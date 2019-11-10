/**
 *  @file       CommonDef.h
 *  @brief      Common type definitions
 *
 *  @author     Mark L. Short
 *
 */
#pragma once

#if !defined(_COMMON_DEF_H__)
#define _COMMON_DEF_H__

#ifndef _BASETSD_H_
    #include <basetsd.h>
#endif

typedef unsigned __int8  BYTE;
typedef unsigned __int32 DWORD;

#ifndef _FSTREAM_
    #include <fstream>
#endif

/**
 *  Predetermined project requirement contraints
 */
namespace req
{
    constexpr int g_CACHE_NUM_BLOCKS      = 16;  ///< number of cache data blocks (Project Requirement)
/**
    @brief size of cache block (in bytes)

    Block size (sometimes referred to as line size) describes the granularity at
    which the cache operates. Each block is a contiguous series of bytes in
    memory and begins on a naturally aligned boundary.

    For example, in a cache with 16 - byte blocks, each block would contain 16
    bytes, and the first byte in each block would be aligned to 16 - byte
    boundaries in the address space, implying that the low - order 4 bits of the
    address of the first byte would always be zero(i.e., 0b ... 0000). The
    smallest usable block size is the natural word size of the processor
    (i.e., 4 bytes for a 32 - bit machine, or 8 bytes for a 64 - bit machine),
    since each access will require the cache to supply at least that many bytes,
    and splitting a single access over multiple blocks would introduce
    unacceptable overhead into the access path.

    Whenever the block size is greater than 1 byte, the low-order bits of an
    address must be used to find the byte or word being accessed within the block.
    As stated above, the low-order bits for the first byte in the block must
    always be zero, corresponding to a naturally aligned block in memory. However,
    if a byte other than the first byte needs to be accessed, the low-order bits
    must be used as a block offset to index into the block to find the right byte.

    The number of bits needed for the block offset is the log2 of the block size,
    so that enough bits are available to span all the bytes in the block. For
    example, if the block size is 64 bytes, log2(64) = 6 low-order bits are used
    as the block offset. The remaining higher-order bits are then used to locate
    the appropriate block in the cache memory.
*/
    constexpr int g_CACHE_BLOCK_SIZE      = 32;  ///< size of cache block (in bytes) (Project Requirement)
    constexpr int g_4WAY_BLOCKS_PER_SET   =  4;  ///< 4-way set associative cache (Project Requirement)
    constexpr int g_4WAY_CACHE_SETS       = g_CACHE_NUM_BLOCKS / g_4WAY_BLOCKS_PER_SET;
    constexpr int g_MAX_ARRAY_SIZE        = 512;
}

#endif