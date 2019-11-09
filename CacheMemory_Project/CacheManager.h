/**
 *  @file       CacheManager.h
 *  @brief      CCacheManager class interface
 *
 *  @author     Mark L. Short
 *
 */

#if !defined(_CACHE_MANAGER_H__)
#define _CACHE_MANAGER_H__

#if !defined(_COMMON_DEF_H__)
    #include "CommonDef.h"
#endif

#if !defined(_CACHE_SET_H__)
    #include "CacheSet.h"
#endif


class CCacheManager
{
    CCacheSet m_rgCacheSets[g_CACHE_SETS];

public:

    /// Default Constructor (is_nothrow_default_constructible == true)
    CCacheManager ( ) noexcept
    { };

    void Init();

/**
    Attempts to retrieve data from cache memory based on address
    
    @param [in]  pAddress     memory address to check for cache hit
    @param [out] dwData       output variable to return stored data value

    @retval true     on cache hit, dwData is set
    @retval false    on cache miss, dwData is not set 
*/
    bool GetCacheData  (const void* pAddress, DWORD_PTR& dwData) noexcept;

 /**
    Loads a contiguous block of memory, upto CACHE_BLOCK_SIZE, based on the
    address pointer passed.

    @param [in] pAddress       address of memory the actual page load is based on

    @retval true      on success
    @retval false     on error
 */
    bool LoadCachePage (const void* pAddress) noexcept;

};



#endif