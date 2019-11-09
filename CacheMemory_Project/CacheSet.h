/**
 *  @file       CacheSet.h
 *  @brief      CCacheSet class interface
 *
 *  @author     Mark L. Short
 *
 */

#if !defined(_CACHE_SET_H__)
#define _CACHE_SET_H__

#if !defined(_COMMON_DEF_H__)
    #include "CommonDef.h"
#endif

#ifndef _QUEUE_
    #include <queue>
#endif

#if !defined(_CACHE_BLOCK_H__)
    #include "CacheBlock.h"
#endif

/// Number of caches sets needed
constexpr int g_CACHE_SETS           = g_CACHE_NUM_BLOCKS / 4;        
constexpr int g_CACHE_BLOCKS_PER_SET = g_CACHE_NUM_BLOCKS / g_CACHE_SETS;

/*
    The simplest approach, direct-mapped, forces a many-to-one mapping between
    addresses and the available storage locations in the cache. In other words, 
    a particular address can reside only in a single location in the cache; that 
    location is usually determined by extracting n bits from the address and using 
    those n bits as a direct index into one of 2n possible locations in the cache.

    Of course, since there is a many-to-one mapping, each location must also store
    a tag that contains the remaining address bits corresponding to the block of 
    data stored at that location. On each lookup, the hardware must read the tag 
    and compare it with the address bits of the reference being performed to 
    determine whether a hit or miss has occurred.

    In the degenerate case where a direct-mapped memory contains enough storage
    locations for every address block (i.e., the n index bits include all bits of 
    the address), no tag is needed, as the mapping between addresses and storage 
    locations is now one-to-one instead of many-to-one. The register file inside 
    the processor is an example of such a memory; it need not be tagged since all 
    the address bits (all bits of the register identifier) are used as the index 
    into the register file.

    Set-associative, is a many-to-few mapping between addresses and storage locations.
    On each lookup, a subset of address bits is used to generate an index, just 
    as in the direct-mapped case. However, this index now corresponds to a set 
    of entries, usually two to eight, that are searched in parallel for a matching 
    tag. In practice, this approach is much more efficient from a hardware 
    implementation perspective, since it requires fewer address comparators than 
    a fully associative cache, but due to its flexible mapping policy behaves 
    similarly to a fully associative cache.

    Set-associative caches permit the flexible placement of data among all the entries 
    of a set.

    - index bits            - select a particular set,
    - tag bits              - select an entry within the set,
    - block offset bits     - select the word within the selected entry.

*/
// n-way set associative
// - Each set contains n entries
// - Block number determines which set
//      - (Block number) mod (Sets in cache)
// - Search all entries in a given set at once
//
// Four-way set-associative data cache
//
// Blocks     = 16;
// BlockSize  = 32 (Bytes);
//
// Sets (s)   = Blocks / 4;
//         s  = 16 / 4;
//         s  = 4;
// Offset (o) - Select the word within each block
//         o  = lg (BlockSize)
//         o  = lg (32)
//         o  = 5;
// Index (i)  = Select set of blocks
//         i  = lg (Number of Sets)
//         i  = lg (4)
//         i  = 2
// Tag (t)    = ID blocks within a set
//         t  = 32 - o - i;
//         t  = 32 - 5 - 2;
//         t  = 25;
//
//           32 bit Address
//     Tag       (set) Index   (blocK) Offset
// |   bits[t]  |   bits[i]    |   bits[o]    |
// |    25      |     2        |      5       |
// |   0..24    |   25..26     |   27..31     |
//
//           64 bit Address
//     Tag       (set) Index   (block) Offset
// |   bits[t]  |   bits[i]    |   bits[o]    |
// |    57      |     2        |      5       |
// |   0..56    |   57..58     |   59..63     |


/**
    @note The following policy will be followed:

    The FIFO policy simply keeps track of the insertion order of the candidates
    and evicts the entry that has resided in the cache for the longest amount of 
    time.  The mechanism that implements this policy is straightforward, since 
    the candidate eviction set (all blocks in a fully associative cache, or all 
    blocks in a single set in a set-associative cache) can be managed as a circular 
    queue. The circular queue has a single pointer to the oldest entry which is 
    used to identify the eviction candidate and the pointer is incremented whenever 
    a new entry is placed in the queue. This results in a single update for every 
    miss in the cache.

    However, the FIFO policy does not always match the temporal locality 
    characteristics inherent in a program’s reference stream, since some memory 
    locations are accessed continually throughout the execution (e.g., commonly 
    referenced global variables). Such references would experience frequent misses 
    under a FIFO policy, since the blocks used to satisfy them would be evicted 
    at regular intervals, as soon as every other block in the candidate eviction 
    set had been evicted.
*/

class CCacheSet
{
    CCacheBlock              m_rgCacheBlock[g_CACHE_BLOCKS_PER_SET];
    std::queue<CCacheBlock*> m_queAvailableBlocks;

public:
    /// Default Constructor (is_nothrow_default_constructible == true)
    CCacheSet ( ) noexcept;

    void Init();

 /**
    Attempts to retrieve data from cache memory based on 
    tag and offset

    @param [in]  dwTag        tag associated with the cache block
    @param [in]  cbOffset     count of byte (cb) offset into cache block
    @param [out] dwData       output variable to return stored data value

    @retval true     on cache hit, dwData is set
    @retval false    on cache miss, dwData is not set 
 */
    bool GetCacheData (DWORD_PTR dwTag, DWORD_PTR cbOffset, DWORD_PTR& dwData) noexcept;
    
 /**
    Loads a contiguous block of memory of CACHE_BLOCK_SIZE, into cache block. It
    further establishes an association with the updated data via the dwTag.

    @param [in] dwTag       value containing Tag to associate with this cache block
    @param [in] pAddress    pointer to contiguous block of memory to load


    @retval true    if successful
    @retval false   on error
 */
    bool LoadCacheBlock (DWORD_PTR dwTag, const void* pAddress) noexcept;

private:
/// Following methods are generated automatically by the compiler if not 
/// explicitly implemented.  The copy constructor and assignment operators 
/// have been declared as 'private' methods to ensure they cannot be called.

    CCacheSet(const CCacheSet& rhs) = delete;
    CCacheSet& operator=(const CCacheSet& rhs) = delete;
};

#endif
