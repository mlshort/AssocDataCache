/**
*  @file        MemoryProject.cpp
*  @brief       Main source file for implementation of data cache simulation
*  @mainpage
*
*  @author      Mark L. Short
*
*  @date        Sept 25, 2014
*
*  <b>CITE:</b>
*     * Modern Processor Design, John Paul Shen, Mikko H. Lipasti, 2005
*     * Cache, Thomas Finley, May 2000, 
*                   http://www.cs.cornell.edu/~tomf/notes/cps104/cache.html
*
*
*  <b>ASSIGNMENT:</b>
*
*  CMPS 5133 Advanced Computer Architecture
*  Assignment #2 - Memory
*
*  Given the following benchmark code running in a four-way associative data 
*  cache with 16 blocks of 32 bytes and knowing that an integer variable 
*  occupies 4 bytes, and operations follow the usual priority, assume that 
*  at each operation the leftmost operand is fetched first and the address of 
*  A[0] is zero. Compute the number of cache misses, considering the loop index 
*  variable residing in a process register (and involved in the count of the 
*  misses) and that arrays A, B, and C reside consecutively in memory.
*      
*      int A[512], B[512], C[512]
*      for (i = 0; i < 511, i++)
*      {
*         A[i] = A[i] + B[i] + B[i+1] * C[i]
*      }
*      
* <b>OUTCOME:</b>
*
*  The executable compiled from the attached C++ achieves the stated purpose 
*  and outputs the results to the console window, with the following caveats:
*
*    1. No assumptions were made regarding the address of A[0] being zero.  
*       As a result, a more involved and accurate four-way associative data 
*       cache implementation was required.
*
*    2. The attached source code has been run on a x64 bit system, but compiled 
*       as a 32bit application. It is not designed or developed for porting 
*       or recompilation as a x64 bit executable.  There are explicit types, 
*       type-casts and assumptions made throughout the code (i.e. casting 
*       memory address to 32-bit types) that limit it to a 32-bit executable.
*
*    3. "Handling Updates to a Block" was not considered at this time and 
*       would require further implementation.
*
*    4. Efforts were made to meaningfully test and debug the algorithms 
*       involved in this implementation.  Code was added to test the validity 
*       of the data returned from cache. Due to the added coding precautions 
*       taken, an error was uncovered that would result in the following:
*
*           Misses: 195 (est)
*           Hits:  1850 (est)
*           Errors:  12
*
*    5. The error was properly identified and diagnosed to be due to failure 
*       to make adjustments to the range of addresses loaded into cache to 
*       insure that they all mapped to the same corresponding "tag" + "index" 
*       address fields.  This was corrected with no errors currently detected.
*
*    6. CACHE DESIGN
*        - Block size               = 32 bytes
*        - Block number             = 16
*        - Number of sets           = 4
*        - Block organization       = (4 way) Set-associative
*        - Block replacement policy = FIFO
*        - Write policy             = not implemented
*
*    7. Given the assignment formula of 'A[i] = A[i] + B[i] + B[i+1] * C[i]', 
*       the operands were accessed in the following order:
*        - B[i+1]
*        - C[i]
*        - A[i]
*        - B[i]
*
*    8. The current implementation results in the following final computation 
*       output:
*              
*           Misses: 192
*           Hits:  1852
*           Errors:   0
*           
*/

#include "stdafx.h"
#include "CommonDef.h"

#include <memory.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "VirtualAddress.h"
#include "CacheManager.h"


constexpr int g_MAX_ARRAY_SIZE = 512;
constexpr int g_DR_PASSOS_LOOP = 511;


__declspec(align(32)) int g_rgA[g_MAX_ARRAY_SIZE] = { 0 };
__declspec(align(32)) int g_rgB[g_MAX_ARRAY_SIZE] = { 0 };
__declspec(align(32)) int g_rgC[g_MAX_ARRAY_SIZE] = { 0 };
#ifdef _DEBUG
__declspec(align(32)) int g_rgD[g_MAX_ARRAY_SIZE] = { -1 };  // for debugging purposes
#endif


std::ostream& PrintIterationHeader(std::ostream& os, int iIteration)
{
    os  << std::dec << std::endl;
    os  << "Cache Miss(es) in Iteration[" << iIteration + 1 << "]"  << std::endl;
    os  << "-----------------------------------------------------"  << std::endl;

    return os;
}

int _tmain (int argc, _TCHAR* argv[])
{
    std::ofstream oflog;
    std::stringstream ss;

    // Let's build our output filename based on the memory address
    // we get for our 1st global variable that we use in our cache
    // simulation.  The only uniqueness in the output is going to be
    // based off of that memory address, so we might as well keep
    // the data around for testing and comparison purposes.

    ss << "..\\Data\\CacheMisses_" << std::hex << std::setw(2 * sizeof(DWORD_PTR) ) 
       << std::setfill('0') 
       << reinterpret_cast<DWORD_PTR>(&g_rgA[0]) << ".txt";

    oflog.open(ss.str().c_str());

    // seeding the global data arrays with some data that we
    // can potentially use to verify if our cache is storing
    // and retrieving correct values
    for ( int i = 0; i < g_MAX_ARRAY_SIZE; i++)
        g_rgA[i] = i + 0x1100;

    for ( int i = 0; i < g_MAX_ARRAY_SIZE; i++ )
        g_rgB[i] = i + 0x2200;

    for ( int i = 0; i < g_MAX_ARRAY_SIZE; i++ )
        g_rgC[i] = i + 0x3300;

    CCacheManager cacheManager;
    cacheManager.Init();

    // let's keep track of some cache statistics
    int iCacheMisses = 0;
    int iCacheHits   = 0;
    int iCacheErrors = 0;

    // also need some local variables to store data
    int iA;
    int iB;
    int iB1;
    int iC;
//
// The following is the benchmark code from Assignment #2
//
//    A[i] = A[i] + B[i] + B[i + 1] * C[i]
//
//
#if defined(_WIN64)
    oflog << "Executing an x64 build" << std::endl;
#else
    oflog << "Executing an x32 build" << std::endl;
#endif

    oflog << "Following based on the physical address of A[0] being 0x" 
          << std::hex << std::setw(2*sizeof(DWORD_PTR)) << std::setfill('0') 
          << reinterpret_cast<DWORD_PTR>(&g_rgA[0]) << std::endl;
    oflog << "================================================================="
          << std::endl;

    for ( int i = 0; i < g_DR_PASSOS_LOOP; i++ )
    {
        DWORD_PTR dataFromCache;
        bool bCacheMissThisIteration = false;

///////////////////////////////////////////////////////////////////////////////
// Attempting to access 1st operand 'B[i + 1]'

        if ( cacheManager.GetCacheData ( &g_rgB[i + 1], dataFromCache) == false )
        {
            iCacheMisses++;
            cacheManager.LoadCachePage ( &g_rgB[i + 1] );
            iB1 = g_rgB[i + 1]; // cache-miss, load the data directly

#ifdef _DEBUG

            CVirtualAddress va ( &g_rgB[i + 1] );

            if (bCacheMissThisIteration == false) 
            { // then lets print the iteration header
                bCacheMissThisIteration = true;
                PrintIterationHeader(oflog, i);
            }

            oflog << std::dec
                  << "   Cache Miss["    << iCacheMisses << "] "
                  << "for 'B[" << i << " + 1]'" << std::endl;

            oflog << "   " << va << std::endl;
#endif

        }
        else
        {
            iCacheHits++;
            iB1 = dataFromCache; // cache-hit, use the data retrieved from cache
#ifdef _DEBUG
        // ok, let's verify if our cache actually stored and retrieved the correct
        // data values
            if (iB1 != g_rgB[i + 1] )
            {
                std::cout << "B[i+1] Data cache inconsistency detected" 
                          << std::endl;
                iCacheErrors++;
            }
#endif
        }

///////////////////////////////////////////////////////////////////////////////
// Attempting to access 2nd operand 'C[i]'

        if ( cacheManager.GetCacheData ( &g_rgC[i], dataFromCache) == false )
        {
            iCacheMisses++;
            cacheManager.LoadCachePage ( &g_rgC[i] );
            iC = g_rgC[i]; // cache-miss, load the data directly

#ifdef _DEBUG
            CVirtualAddress va (&g_rgC[i]);
             
            if ( bCacheMissThisIteration == false ) 
            { // then lets print the iteration header
                bCacheMissThisIteration = true;
                PrintIterationHeader (oflog, i);
            }

            oflog << std::dec 
                  << "   Cache Miss[" << iCacheMisses << "] "
                  << "for 'C[" << i << "]'" << std::endl;

            oflog << "   " << va << std::endl; 
#endif

        }
        else
        {
            iCacheHits++;
            iC = dataFromCache; // cache-hit, use the data retrieved from cache
#ifdef _DEBUG
            if ( iC != g_rgC[i] ) // now let's verify the retrieved cache data
            {
                std::cout << "C[i] Data cache inconsistency detected" 
                          << std::endl;
                iCacheErrors++;
            }
#endif
        }

///////////////////////////////////////////////////////////////////////////////
// Attempting to access 3rd operand 'A[i]'

        if ( cacheManager.GetCacheData (&g_rgA[i], dataFromCache) == false )
        {
            iCacheMisses++;
            cacheManager.LoadCachePage (&g_rgA[i]);
            iA = g_rgA[i]; // cache-miss, load the data directly

#ifdef _DEBUG
            CVirtualAddress va (&g_rgA[i]);

            if ( bCacheMissThisIteration == false ) 
            { // then lets print the iteration header
                bCacheMissThisIteration = true;
                PrintIterationHeader (oflog, i);
            }

            oflog << std::dec 
                  << "   Cache Miss[" << iCacheMisses << "] "
                  << "for 'A["          << i << "]'" << std::endl;

            oflog << "   " << va << std::endl;
#endif
        }
        else
        {
            iCacheHits++;
            iA = dataFromCache; // cache-hit, use the data retrieved from cache

#ifdef _DEBUG
            if ( iA != g_rgA[i] ) // now let's verify the retrieved cache data
            {
                std::cout << "A[i] Data cache inconsistency detected" 
                          << std::endl;
                iCacheErrors++;
            }
#endif
        }

////////////////////////////////////////////////////////////////////////////////
// Attempting to access 4th operand 'B[i]'

        if ( cacheManager.GetCacheData ( &g_rgB[i], dataFromCache) == false )
        {
            iCacheMisses++;
            cacheManager.LoadCachePage (&g_rgB[i]);
            iB = g_rgB[i]; // cache-miss, load the data directly

#ifdef _DEBUG
            CVirtualAddress va(&g_rgB[i]);

            if ( bCacheMissThisIteration == false ) 
            {  // then lets print the iteration header
                bCacheMissThisIteration = true;
                PrintIterationHeader(oflog, i);
            }

            oflog << std::dec 
                  << "   Cache Miss[" << iCacheMisses << "] " 
                  << "for 'B["          << i << "]'" << std::endl;
            oflog << "   " << va << std::endl;
#endif
        }
        else
        {
            iCacheHits++;
            iB = dataFromCache; // cache-hit, use the data retrieved from cache
#ifdef _DEBUG
            if ( iB != g_rgB[i] ) // now let's verify the retrieved cache data
            {
                std::cout << "B[i] Data cache inconsistency detected" 
                          << std::endl;
                iCacheErrors++;
            }
#endif
        }
        
        // parenthesis used to denote explicit operation
        int iResult = iA + iB + (iB1 * iC);  

        g_rgA[i] = iResult;

        std::cout << "Iteration[ i=" << i << " ]" << std::endl;
        std::cout << "A[i] + B[i] + B[i + 1] * C[i] Computation Result:" 
                  << iResult 
                  << std::endl;
        std::cout << "-------------------------------------------------------------" 
                  << std::endl;
        std::cout << std::dec;
        std::cout << "Cache Misses:" << iCacheMisses  << std::endl;
        std::cout << "Cache Hits:  " << iCacheHits    << std::endl;
        std::cout << "Cache Errors:" << iCacheErrors  << std::endl;

    }

    oflog << std::dec;
    oflog << "----------------------------------------" << std::endl;
    oflog << "Cache Misses:" << iCacheMisses << std::endl;
    oflog << "Cache Hits:  " << iCacheHits   << std::endl;
    oflog << "Cache Errors:" << iCacheErrors << std::endl;

    oflog.close();

    std::cout << std::endl;
    std::cout << "[Enter 'q' to exit program]" << std::endl;

    char c;
    std::cin >> c;

    return 0;
}

