/**
 *  @file       VirtualAddress.cpp
 *  @brief      CVirtualAddress class implementation
 *
 *  @author     Mark L. Short
 *
 */
#include "stdafx.h"

#include <iostream>
#include <iomanip>

#include "VirtualAddress.h"


template <typename _T>
static constexpr _T bitmask(size_t nBitsSet)
{
    return static_cast<_T>(-(nBitsSet != 0))
        & (static_cast<_T>(-1) >> ((sizeof(_T) * CHAR_BIT) - nBitsSet));
};

constexpr size_t static_log2(size_t n)
{
    return ((n < 2) ? 0 : 1 + static_log2(n >> 1));
};

/// calculated as log2(BlockSize), specifically log2(32) in this instance
constexpr size_t   OFFSET_BITS = static_log2(req::g_CACHE_BLOCK_SIZE);
/// calculated as log2(NumSets), specifically log2(4) in this instance
constexpr size_t   INDEX_BITS  = static_log2(req::g_4WAY_CACHE_SETS);

constexpr size_t   TAG_BITS    = sizeof(DWORD_PTR) - INDEX_BITS - OFFSET_BITS;

DWORD_PTR CVirtualAddress::DecodeTag (void) const noexcept
{
    if ( m_pAddress )
    {
        DWORD_PTR dwReturn = reinterpret_cast<DWORD_PTR>(m_pAddress);

        return (dwReturn >> (INDEX_BITS + OFFSET_BITS));
    }
    else
        return DECODE_ERROR;
}

DWORD_PTR CVirtualAddress::DecodeOffset (void) const noexcept
{
    // compile time generation of 0x001F bitmask
    static DWORD_PTR offsetMask = bitmask<DWORD_PTR>(OFFSET_BITS);

    if ( m_pAddress )
    {
        DWORD_PTR dwReturn = reinterpret_cast<DWORD_PTR>(m_pAddress);

        return (dwReturn & offsetMask);
    }
    else
        return DECODE_ERROR;
}

DWORD_PTR CVirtualAddress::DecodeIndex (void) const noexcept
{
    // compile time generation of 0x0003 bitmask
    static DWORD_PTR indexMask = bitmask<DWORD_PTR>(INDEX_BITS);

    if ( m_pAddress )
    {
        DWORD_PTR dwReturn = reinterpret_cast<DWORD_PTR>(m_pAddress);

        dwReturn = dwReturn >> OFFSET_BITS;

        return (dwReturn & indexMask);
    }
    else
        return DECODE_ERROR;
}

std::ostream& CVirtualAddress::operator << (std::ostream& os) const
{
    os  << "Address[0x"  << std::hex << std::setw (2 * sizeof (DWORD_PTR)) 
        << std::setfill ('0') << DecodeAddress () << "] "
        << std::dec
        << "Tag["        << DecodeTag ( )       << "] "
        << "Index["      << DecodeIndex ( )     << "] "
        << "Offset["     << DecodeOffset ( )    << "] ";

    return os;

};

std::ostream& operator<< (std::ostream& os, const CVirtualAddress& va)
{
    return va.operator<< (os);
}
