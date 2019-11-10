# 4-way set associative data cache
  
# [Project Documentation](https://mlshort.github.io/AssocDataCache/html/index.html)

DIRECTORY STRUCTURE
===================
```
  +-- Assoc Data Cache
        |
        +-- CacheMemory_Project  (source)
        |
        +-- Bin              (compiled executable)
        |
        +-- Data             (output file data)
        |
        +-- Documentation    (implementation documentation)
        |
        +-- Obj              (discardable binary intermediaries)  
```    

Associative Data Cache Project Overview
========================================================================

1. ASSIGNMENT:

  CMPS 5133 Advanced Computer Architecture
  Assignment #2 - Memory

  Given the following benchmark code running in a four-way associative data 
  cache with 16 blocks of 32 bytes and knowing that an integer variable 
  occupies 4 bytes, and operations follow the usual priority, assume that 
  at each operation the leftmost operand is fetched first and the address of 
  A[0] is zero. Compute the number of cache misses, considering the loop index 
  variable residing in a process register (and involved in the count of the 
  misses) and that arrays A, B, and C reside consecutively in memory.
      
      int A[512], B[512], C[512]
      for (i = 0; i < 511, i++)
      {
         A[i] = A[i] + B[i] + B[i+1] * C[i]
      }

2. BACKGROUND

   When this assignment was given, the instructor did not consider that it could 
   be solved programmatically, given the fact that in a typical program the memory 
   address of any given variable is somewhat pseudo-randomly determined at runtime. 
   (the assignment is based off the assumption that the arrays (A[512], B[512], C[512])
   resided contiguously in memory, with a memory address beginning at 0x00000000 )

   I decided to proceed to solve the problem programmatically anyway, but with 
   the added difficulty of compensating for the fact that &A[0] will never equal 
   0x00000000. (an impossibility in C++)

   I was very aware I would have to somehow compensate for being given a random
   piece of physical memory and implement some form of virtual memory addressing, 
   but additionally I would need to have in place a mechanism to validate and 
   verify the accuracy of the underlying 4-way set-associative cache to insure 
   that I legitimately solved the problem via a software implementation.

   Additionally, due to being ill the previous week, by the time I finally 
   received the assignment, I had only 2 days to accomplish this in.

  IMPLEMENTATION
===============================================================================

   Really my approach was very simple.
   1. Have a plan on how to validate the reported cache-hits.
   2. Implement the virtual address translation.
   3. Implement the 4-way set-associative cache.
   4. Test to see how close I got.
   5. Be prepared to adjust my algorithm & revalidate until I got it perfect.   

   The obstacles:
   1. No prior development experience with cache or memory address translations.
   2. No sample code to work off of.
   3. To date, some very poor PowerPoint lecture slides detailing how this is all
      supposed to work. (that is ok, I am fairly proficient at domain research)

   The process:
   1.  Even though my course book & lecture lacked enough detailed information 
       for me to work from, I quickly found other lecture notes that filled in the 
       gaps.  One example was here : 
       [Cornell University Cache Lecture](http://www.cs.cornell.edu/~tomf/notes/cps104/cache.html])
   2.  This was exactly the kind of detailed information I needed to implement a 
       real software solution.  If you notice my source code documentation, I 
       incorporated pertinent narrations directly in my comments for further 
       reference.
   3.  Based off of this more detailed information, I got amazingly close on my 
       first real attempt; however, my tests were showing a modest level of 
       variation in my results.  My cache hit/miss results were varying by 
       around +- 2.  There was a boundary or rounding issue I had not accounted 
       for yet.
   4.  I approached my instructor, who was intrigued by my approach.  He suggested
       adding additional global variables prior to my A[512] array.  That put me 
       on the right path and I could see exactly what was happening.  It was a 
       subtle data alignment issue that would randomly assign my variables addresses 
       that were not properly "cache-line" aligned.
   5.  That's ok, I know a few tricks of my own.  That is where the __declspec(align(32)) 
       came into play.
