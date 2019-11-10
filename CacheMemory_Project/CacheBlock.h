/**
 *  @file       CacheBlock.h
 *  @brief      CCacheBlock class interface
 *
 *  @author     Mark L. Short
 *
 */

#if !defined(_COMMON_DEF_H__)
    #include "CommonDef.h"
#endif




/**
 *   CCacheBlock class manages a logical unit of contiguous memory and the
 *   associated reference Tag.  
 */
class CCacheBlock
{
    DWORD_PTR     m_dwTag; ///< Tag identifier associated with cache date block
    BYTE          m_rgBlock[req::g_CACHE_BLOCK_SIZE]; ///< actual cache data block

public:
/**
 *  Default Constructor
 *
 *  @note (is_nothrow_default_constructible == true)
 */
    CCacheBlock ( ) noexcept;

 /**
    Sets cache block Tag identifier

    @param [in] dwSet   new Tag identifier
 */
    void set_Tag (DWORD_PTR dwSet) noexcept 
    { m_dwTag = dwSet; };

 /**
    Returns cache block Tag identifier

    @retval DWORD_PTR containing Tag identifier
 */
    constexpr DWORD_PTR get_Tag (void) const noexcept
    { return m_dwTag; };

 /**
    Attempts to retrieve data from the underlying cache block based on offset
    
    @param [in]  cbOffset     count of byte (cb) offset into cache block
    @param [out] dwData       output variable to return stored data value

    @retval true     on cache hit, dwData is set
    @retval false    on cache miss, dwData is not set 
 */
    bool GetCacheData   (size_t cbOffset, DWORD& dwData) const noexcept;

 /**
    Loads a contiguous block of memory, upto CACHE_BLOCK_SIZE, into cache block. It
    further establishes an association with the updated data via the dwTag.

    @param [in] dwTag       value containing Tag to associate with this cache block
    @param [in] pData       pointer to contiguous block of memory to load
    @param [in] cbLen       count of bytes (cb) of data length (optional parameter)

    @retval true    on success
    @retval false   on error
 */
    bool LoadCacheBlock (DWORD_PTR dwTag, const BYTE* pData, 
                         size_t cbLen = req::g_CACHE_BLOCK_SIZE) noexcept;

private:

    CCacheBlock(const CCacheBlock& rhs) = delete;

    CCacheBlock& operator = (const CCacheBlock& rhs) = delete;

};

