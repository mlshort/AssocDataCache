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
    @brief size, in bytes, of each cache block

    Block size (sometimes referred to as line size) describes the granularity at 
    which the cache operates. Each block is a contiguous series of bytes in 
    memory and begins on a naturally aligned boundary.

    For example, in a cache with 16 - byte blocks, each block would contain 16 
    bytes, and the first byte in each block would be aligned to 16 - byte 
    boundaries in the address space, implying that the low - order 4 bits of the 
    address of the first byte would always be zero(i.e., 0b ... 0000). The 
    smallest usable block size is the natural word size of the processor 
    (i.e., 4 bytes for a 32 - bit machine, or 8 bytes for a 64 - bit machine), 
    since each access will require the cache to supply at least that many bytes, 
    and splitting a single access over multiple blocks would introduce 
    unacceptable overhead into the access path.

    Whenever the block size is greater than 1 byte, the low-order bits of an
    address must be used to find the byte or word being accessed within the block. 
    As stated above, the low-order bits for the first byte in the block must 
    always be zero, corresponding to a naturally aligned block in memory. However, 
    if a byte other than the first byte needs to be accessed, the low-order bits 
    must be used as a block offset to index into the block to find the right byte.

    The number of bits needed for the block offset is the log2 of the block size, 
    so that enough bits are available to span all the bytes in the block. For 
    example, if the block size is 64 bytes, log2(64) = 6 low-order bits are used 
    as the block offset. The remaining higher-order bits are then used to locate 
    the appropriate block in the cache memory.
*/
const int g_CACHE_BLOCK_SIZE     = 32;
const int g_CACHE_NUM_BLOCKS     = 16;  ///< number of cache data blocks

/**
    CCacheBlock class manages a logical unit of contiguous memory (i.e. block, 
    entity)
*/
class CCacheBlock
{
    /// Tag that is associated with this cache block
    DWORD_PTR     m_dwTag;                       
    BYTE          m_rgBlock[g_CACHE_BLOCK_SIZE]; ///< actual cache data block

public:
    /// Default Constructor (is_nothrow_default_constructible == true)
    CCacheBlock ( ) noexcept;

    /// Destructor
    ~CCacheBlock() = default;

 /**
    a simple data accessor

    @param [in] dwSet
 */
    void set_Tag (DWORD_PTR dwSet) noexcept 
    { m_dwTag = dwSet; };

 /**
    a simple data accessor

    @retval DWORD_PTR containing tag
 */
    constexpr DWORD_PTR get_Tag (void) const noexcept
    { return m_dwTag; };

 /**
    Attempts to retrieve data from cache memory based on offset
    
    @param [in]  cbOffset     count of byte (cb) offset into cache block
    @param [out] dwData       output variable to return stored data value

    @retval true     on cache hit, dwData is set
    @retval false    on cache miss, dwData is not set 
 */
    bool GetCacheData   (size_t cbOffset, DWORD_PTR& dwData) const noexcept;

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
                         size_t cbLen = g_CACHE_BLOCK_SIZE) noexcept;

private:
/// Following methods are generated automatically by the compiler if not 
/// explicitly implemented.  The copy constructor and assignment operators 
/// have been declared as 'private' methods to ensure they cannot be called.

    CCacheBlock(const CCacheBlock& rhs) = delete;

    CCacheBlock& operator = (const CCacheBlock& rhs) = delete;

};

