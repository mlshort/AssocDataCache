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

bool CCacheSet::GetCacheData (DWORD_PTR dwTag, DWORD_PTR dwOffset, DWORD_PTR& dwData) noexcept
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
        bReturn = m_rgCacheBlock[i].GetCacheData (dwOffset, dwData);
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

