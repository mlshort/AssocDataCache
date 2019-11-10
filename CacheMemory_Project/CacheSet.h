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

/*
    A Set-associative cache, is a many-to-few mapping between addresses and 
    storage locations. On each lookup, a subset of address bits is used to 
    generate an index, just as in the direct-mapped case. However, this index 
    now corresponds to a set of entries, usually two to eight, that are searched 
    in parallel for a matching tag. In practice, this approach is much more 
    efficient from a hardware implementation perspective, since it requires 
    fewer address comparators than a fully associative cache, but due to its 
    flexible mapping policy behaves similarly to a fully associative cache.

    Set-associative caches permit the flexible placement of data among all the 
    entries of a set.

    - index bits            - select a particular set,
    - tag bits              - select an entry within the set,
    - block offset bits     - select the word within the selected entry.
*/

// n-way set associative
//
// - Each set contains n entries (or blocks)
// - Block number determines which set
//      - (Block number) mod (Sets in cache)
// - Search all entries in a given set at once
//

// In a 4-way set-associative data cache, the following data is used:
//
// Total Number of Blocks (b)      = 16; (see requirements)
// BlockSize (in bytes)            = 32; (see requirements) 
// Number of Blocks per Set (N)    = 4;  (see requirements) 4-way set associative
//
// Calculate other items as follows:
// ---------------------------------
// Number of Sets (s) = b / N;
//                 s  = 16 / 4;
//                 s  = 4;
//
// Offset Bits (o) - allows Selecting the word within each block
//              o  = log2 (BlockSize)
//              o  = log2(32)
//              o  = 5;
//
// Index Bits (i)  = allows Selecting the set of blocks
//              i  = log2 (Number of Sets)
//              i  = log2 (4)
//              i  = 2
//
// Tag Bits (t)    = ID blocks within a set
//              t  = sizeof(address) - o - i;
//              (given a 32-bit address)
//              t  = 32 - 5 - 2;
//              t  = 25;
//


/**
 *  Contains a set of cache blocks and manages the associated replacement policy
 */
class CCacheSet
{
    CCacheBlock              m_rgCacheBlock[req::g_4WAY_BLOCKS_PER_SET];
    std::queue<CCacheBlock*> m_queAvailableBlocks;

public:
/**
 *  Default Constructor
 *
 *  @note (is_nothrow_default_constructible == true)
 */
    CCacheSet ( ) noexcept;

/**
 *  Initiializes internal data-structures
 *
 *  Two-stage construction: Construct and initialize the object in two separate stages. 
 *  The constructor creates the object and an initialization function initializes it.
 */
    void Init();

 /**
    Attempts to retrieve data from cache memory based on 
    tag and offset

    @param [in]  dwTag        Tag associated with the cache block
    @param [in]  cbOffset     count of byte (cb) offset into cache block
    @param [out] dwData       output variable to return stored data value

    @retval true     on cache hit, dwData is set
    @retval false    on cache miss, dwData is not set 
 */
    bool GetCacheData (DWORD_PTR dwTag, size_t cbOffset, DWORD& dwData) noexcept;
    
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

    CCacheSet(const CCacheSet& rhs) = delete;
    CCacheSet& operator=(const CCacheSet& rhs) = delete;
};

#endif
