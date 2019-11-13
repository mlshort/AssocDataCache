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


/// Error Code returned on decoding failure
constexpr DWORD_PTR DECODE_ERROR = std::numeric_limits<DWORD_PTR>::max();

/**
  This class is used to translate physical memory addresses into "virtual" 
  addresses by decoding relevant bit patterns into corresponding information 
  fields needed to reference cache memory.
    
  The original address is partitioned into three portions: 
    the index bits are used to select a block (and the associated set); 
    the block offset bits are used to select a word within a selected block; and 
    the tag bits are used to do a tag match against the tag stored in the tag field of 
    the selected block.

  The underlying member pointer address is translated as follows:

                 32 bit Address
 |    Tag      |  (set) Index  |  (block) Offset |
 | :---------: | :-----------: | :-------------: |
 |             | log2(NumSets) | log2(BlockSize) |
 |   25 bits   |    2 bits     |     5 bits      |
 |    0..24    |    25..26     |     27..31      |

                64 bit Address
 |    Tag      |  (set) Index  |  (block) Offset |
 | :---------: | :-----------: | :-------------: |
 |             | log2(NumSets) | log2(BlockSize) |
 |   57 bits   |    2 bits     |      5 bits     |
 |    0..56    |    57..58     |      59..63     |

*/
class CVirtualAddress
{ 
    const void* m_pAddress;

public:

    /// Initialization Constructor
    constexpr CVirtualAddress (const void* pAddress) noexcept
        : m_pAddress (pAddress)
    { };

 /**
    Decodes and returns Tag from the underlying memory address

    @retval  DWORD_PTR containg Tag   on success
    @retval  DECODE_ERROR             on failure
 */
    DWORD_PTR DecodeTag       (void) const noexcept;

 /**
    Decodes and returns Block Offset from the underlying memory address

    @retval DWORD_PTR containing Offset   on success
    @retval DECODE_ERROR                  on failure
 */
    DWORD_PTR DecodeOffset    (void) const noexcept;

 /**
    Decodes and returns Cache Set Index from the underlying memory address

    @retval Index           on success
    @retval DECODE_ERROR    on error
 */
    DWORD_PTR DecodeIndex     (void) const noexcept;

    constexpr DWORD_PTR DecodeAddress   (void) const noexcept
    { return reinterpret_cast<const DWORD_PTR>(m_pAddress); };

    std::ostream& operator << (std::ostream& os) const;

private:
    // We really do not want this class to be instantiated in this manner

    CVirtualAddress() = delete;
};

std::ostream& operator<< (std::ostream& os, const CVirtualAddress& va);

#endif
