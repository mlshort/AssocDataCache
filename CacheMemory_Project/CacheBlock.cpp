/**
 *  @file       CacheBlock.cpp
 *  @brief      CCacheBlock class implementation
 *
 *  @author     Mark L. Short
 *
 */

#include "stdafx.h"
#include <memory.h>
#include "CacheBlock.h"


CCacheBlock::CCacheBlock ( )
    : m_dwTag (0)
{  // intentionally marking the memory block with fixed value
   // for testing and debugging purposes (yeah, like M$)
    memset (m_rgBlock, 'FE',  sizeof(m_rgBlock) );
};


bool CCacheBlock::GetCacheData (size_t cbOffset, DWORD_PTR& dwData) const
{
    bool bReturn = false;
    if ( cbOffset < ( sizeof (m_rgBlock) + sizeof (DWORD_PTR) - sizeof (BYTE)) )
    {
        dwData  = *reinterpret_cast<const DWORD_PTR*>(&m_rgBlock[cbOffset]);
        bReturn = true;
    }
    return bReturn;
}

bool CCacheBlock::LoadCacheBlock (DWORD_PTR dwTag, const BYTE* pData, 
                                  size_t cbLen /* = g_CACHE_BLOCK_SIZE */)
{
    bool bReturn = false;
    if ( pData != nullptr )
    {
        if ( cbLen <= sizeof(m_rgBlock) )
        {
            memcpy (m_rgBlock, pData, cbLen);
            m_dwTag = dwTag;
            bReturn = true;
        }
    }
    return bReturn;
}
