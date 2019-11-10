/**
 *  @file       CacheManager.cpp
 *  @brief      CCacheManager class implementation
 *
 *  @author     Mark L. Short
 *
 */
#include "stdafx.h"

#include <iostream>
#include <iomanip>

#include "VirtualAddress.h"
#include "CacheManager.h"

void CCacheManager::Init(void)
{
    for (auto& it : m_rgCacheSets)
        it.Init();
}

bool CCacheManager::GetCacheData (const void* pAddress, DWORD& dwData) noexcept
{
    bool bReturn = false;
    // we need to decode pAddress and see if it maps to what we have in cache
    if ( pAddress )
    {
        CVirtualAddress vAddress (pAddress);
        DWORD_PTR dwIndex = vAddress.DecodeIndex ( );

        if ( (dwIndex < _countof(m_rgCacheSets) ) && (dwIndex != DECODE_ERROR) )
        {
            DWORD_PTR dwTag    = vAddress.DecodeTag ( );
            DWORD_PTR dwOffset = vAddress.DecodeOffset ( );
#ifdef _DEBUG
            std::cout << "  Checking Cache Set [" << dwIndex  << "] "
                      << "for Tag ["              << dwTag    << "] "
                      << "Offset ["               << dwOffset << "]" << std::endl;
#endif
/* 
   On each lookup, we must read the tag and compare it with the address bits of 
   the reference being performed to determine whether a hit or miss has occurred.

   A compromise between the indexed memory and the associative memory is the 
   set-associative memory which uses both indexing and associative search; An 
   address is used to index into one of the sets, while the multiple entries 
   within a set are searched with a key to identify one particular entry. This 
   compromise provides some flexibility in the placement of data without 
   incurring the complexity of a fully associative memory.
*/
            bReturn = m_rgCacheSets[dwIndex].GetCacheData (dwTag, dwOffset, dwData);
        }
    }

    return bReturn;
}

bool CCacheManager::LoadCachePage (const void* pAddress) noexcept
{
    bool bReturn = false;

    if ( pAddress )
    {
        CVirtualAddress vAddress (pAddress);
        DWORD_PTR dwIndex = vAddress.DecodeIndex ( );
        if ( (dwIndex < _countof(m_rgCacheSets) ) && (dwIndex != DECODE_ERROR) )
        {
            bReturn = m_rgCacheSets[dwIndex].LoadCacheBlock(vAddress.DecodeTag( ), 
                                                            pAddress);
        }
    }
    return bReturn;
}


