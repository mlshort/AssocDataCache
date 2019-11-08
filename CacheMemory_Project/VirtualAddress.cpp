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

//           32 bit Address
//     Tag       (set) Index   (blocK) Offset
// |   bits[t]  |   bits[i]    |   bits[o]    |
// |    25      |     2        |      5       |
// |   0..24    |   25..26     |   27..31     |

//           64 bit Address
//     Tag       (set) Index   (blocK) Offset
// |   bits[t]  |   bits[i]    |   bits[o]    |
// |    57      |     2        |      5       |
// |   0..56    |   57..58     |   59..63     |

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
    if ( m_pAddress )
    {
        DWORD_PTR dwReturn = reinterpret_cast<DWORD_PTR>(m_pAddress);

        return (dwReturn & 0x001F); /// @todo: cleanup these hardcoded values
    }
    else
        return DECODE_ERROR;
}

DWORD_PTR CVirtualAddress::DecodeIndex (void) const noexcept
{
    if ( m_pAddress )
    {
        DWORD_PTR dwReturn = reinterpret_cast<DWORD_PTR>(m_pAddress);

        dwReturn = dwReturn >> OFFSET_BITS;

        return (dwReturn & 0x0003); /// @todo: cleanup these hard-coded values
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
