/**
 *  @file       VirtualAddress.h
 *  @brief      CVirtualAddress class interface
 *
 *  @author     Mark L. Short
 *
 */

#if !defined(_VIRTUAL_ADDRESS_H__)
#define _VIRTUAL_ADDRESS_H__

#if !defined(_COMMON_DEF_H__)
    #include "CommonDef.h"
#endif

#ifndef _OSTREAM_
    #include <ostream>
#endif

//           32 bit Address
//     Tag       (set) Index   (block) Offset
// |   bits[t]  |   bits[i]    |   bits[o]    |
// |    25      |     2        |      5       |
// |   0..24    |   25..26     |   27..31     |

//           64 bit Address
//     Tag       (set) Index   (block) Offset
// |   bits[t]  |   bits[i]    |   bits[o]    |
// |    57      |     2        |      5       |
// |   0..56    |   57..58     |   59..63     |

/// Error Code returned on decoding failure
const DWORD_PTR DECODE_ERROR = (DWORD_PTR) -1;

/**
    This class is used to translate physical memory addresses into "virtual" 
    addresses by decoding relevant bit patterns into corresponding information 
    fields needed to reference cache memory.  The original address is partitioned 
    into three portions: the index bits are used to select a block; the block 
    offset bits are used to select a word within a selected block, and the tag 
    bits are used to do a tag match against the tag stored in the tag field of 
    the selected entry.

    Set-associative caches permit the flexible placement of data among all the 
    entries of a "set". The index bits select a particular set, the tag bits 
    select an entry (i.e. block) within the set,  and the block offset bits select 
    the word within the selected entry (i.e. block).

*/
class CVirtualAddress
{
    /// calculated as lg (BlockSize), specifically lg (32) in this instance
    const size_t   OFFSET_BITS = 5;  
    /// calculated as lg (NumSets), specifically lg (4) in this instance
    const size_t   INDEX_BITS  = 2;  
    const size_t   TAG_BITS    = sizeof (DWORD_PTR) - INDEX_BITS - OFFSET_BITS;
 
    const void* m_pAddress;

public:

    /// Initialization Constructor
    CVirtualAddress (const void* pAddress)
        : m_pAddress (pAddress)
    { };

 /**
    Decodes and returns Tag from the underlying memory address

    @retval  Tag          on success
    @retval  DECODE_ERROR on failure
 */
    DWORD_PTR DecodeTag       (void) const;

 /**
    Decodes and returns Block Offset from the underlying memory address

    @retval Offset       on success
    @retval DECODE_ERROR on failure
 */
    DWORD_PTR DecodeOffset    (void) const;

 /**
    Decodes and returns Cache Set Index from the underlying memory address

    @retval Index           on success
    @retval DECODE_ERROR    on error
 */
    DWORD_PTR DecodeIndex     (void) const;

    const DWORD_PTR DecodeAddress   (void) const
    { return reinterpret_cast<const DWORD_PTR>(m_pAddress); };

    std::ostream& operator << (std::ostream& os) const;

private:
    // We really do not want this class to be instantiated in this manner,
    // so going to make private

    /// Default Constructor
    CVirtualAddress ( )
        : m_pAddress (nullptr)
    { };

};

std::ostream& operator<< (std::ostream& os, const CVirtualAddress& va);

#endif