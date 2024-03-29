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


CCacheBlock::CCacheBlock ( ) noexcept
    : m_dwTag (0)
{  // intentionally marking the memory block with fixed value
   // for testing and debugging purposes (yeah, like M$)
    memset (m_rgBlock, 'FE',  sizeof(m_rgBlock) );
};


bool CCacheBlock::GetCacheData (size_t cbOffset, DWORD& dwData) const noexcept
{
    bool bReturn = false;
    if ( cbOffset < ( sizeof (m_rgBlock) + sizeof (DWORD) - sizeof (BYTE)) )
    {
        dwData  = *reinterpret_cast<const DWORD*>(&m_rgBlock[cbOffset]);
        bReturn = true;
    }
    else
    {
        _CrtDbgBreak();
    }
    return bReturn;
}

bool CCacheBlock::LoadCacheBlock (DWORD_PTR dwTag, const BYTE* pData, 
                                  size_t cbLen /* = g_CACHE_BLOCK_SIZE */) noexcept
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
