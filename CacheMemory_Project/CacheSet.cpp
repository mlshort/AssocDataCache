/**
 *  @file       CacheSet.cpp
 *  @brief      CCacheSet class implementation
 *
 *  @author     Mark L. Short
 *
 */

#include "stdafx.h"
#include <iostream>
#include "CacheSet.h"

CCacheSet::CCacheSet() noexcept
    : m_queAvailableBlocks()
{
};

void CCacheSet::Init(void)
{
    for (int i = 0; i < _countof(m_rgCacheBlock); i++)
        m_queAvailableBlocks.push(&m_rgCacheBlock[i]);
};

bool CCacheSet::GetCacheData (DWORD_PTR dwTag, size_t cbOffset, DWORD& dwData) noexcept
{
    bool bReturn = false;
    // lets iterate through our cache blocks and see if any matches 'dwTag'
    // Actually, this should be done in multiple threads simultaneously
    bool bFound = false;
    int i;
    for ( i = 0; i < _countof(m_rgCacheBlock); i++ )
    {
        DWORD_PTR dwCacheTag = m_rgCacheBlock[i].get_Tag ( );

#ifdef _DEBUG
        std::cout << "    Checking Cache Block [" << i << "] " 
                  << "Cache Tag ["            << dwCacheTag << "]" << std::endl;
#endif
        if ( dwTag == dwCacheTag )
        {
            bFound = true;
            break;
        }
    }

    if ( bFound )
    {
        bReturn = m_rgCacheBlock[i].GetCacheData (cbOffset, dwData);
#ifdef _DEBUG
        std::cout << "    ** Cache Hit ** ";
        if (bReturn)
            std::cout << "Data returned [" << dwData << "]" << std::endl;
        else
            std::cout << "Error retrieving data!" << std::endl;
#endif
    }

    return bReturn;
}

/**
    @note LoadCacheBlock utilitizes the FIFO replacement policy

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
    characteristics inherent in a program's reference stream, since some memory
    locations are accessed continually throughout the execution (e.g., commonly
    referenced global variables). Such references would experience frequent misses
    under a FIFO policy, since the blocks used to satisfy them would be evicted
    at regular intervals, as soon as every other block in the candidate eviction
    set had been evicted.
*/
bool CCacheSet::LoadCacheBlock (DWORD_PTR dwTag, const void* pAddress) noexcept
{
    bool bReturn = false;
    // lets find a stale CacheBlock to load
    if (!m_queAvailableBlocks.empty())
    {
        CCacheBlock* pCacheBlock = m_queAvailableBlocks.front();
        m_queAvailableBlocks.pop();
        /*
             In order to keep everything matching up correctly with our Tag association,
             we need to load memory addresses that would have the same tag and index
             fields when subsequently broken down.

             to do this, i am going clear the Offset bits (5 bits) of the pAddress parameter
             to generate an address that points to memory that is properly aligned to match
             up with our tag + index field associations
        */
        DWORD_PTR dwAdjustedAddress = (reinterpret_cast<DWORD_PTR>(pAddress) & ~(0x001F));

        bReturn = pCacheBlock->LoadCacheBlock(dwTag,
            reinterpret_cast<const BYTE*>(dwAdjustedAddress));

        m_queAvailableBlocks.push(pCacheBlock);
    }

    return bReturn;
}

