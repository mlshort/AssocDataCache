/**
 *  @file       CommonDef.h
 *  @brief      Common type definitions
 *
 *  @author     Mark L. Short
 *
 */
#pragma once

#if !defined(_COMMON_DEF_H__)
#define _COMMON_DEF_H__

#ifndef _BASETSD_H_
    #include <basetsd.h>
#endif

typedef unsigned __int8  BYTE;
typedef unsigned __int32 DWORD;

#ifndef _FSTREAM_
    #include <fstream>
#endif

extern std::ofstream oflog;


#endif