#ifndef A3DB_DATATYPES_HPP
#define A3DB_DATATYPES_HPP
/**********************************************************************************************************************\

    DESCRIPTION: Datatyes that can be used to have precise byte sizes for the datatypes aross all compilers

------------------------------------------------------------------------------------------------------------------------

    CREATION DATE:  27.12.2017

------------------------------------------------------------------------------------------------------------------------

    Copyright © 2018
    Paul L. (https://github.com/Arkensor)
    Ollie W. (https://github.com/BoGuu)
    All rights reserved!

\**********************************************************************************************************************/

#include <string>
#include <vector>

#ifdef _WINDOWS

//Windows specific includes
#include <Windows.h>
#include <cstdio>

typedef  __int64 testin;

namespace A3
{
namespace DataTypes
{

typedef __int8 int8;
typedef unsigned __int8 uint8;
typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;
typedef float a3float;
typedef double a3double;

}; // end namespace DataTypes
}; // end namespace A3

#else

//Linux specifics
#include <stdint.h>

namespace A3
{
namespace DataTypes
{

typedef char int8;
typedef unsigned char uint8;
typedef short int int16;
typedef unsigned short int uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef float a3float;
typedef double a3double;

}; // end namespace DataTypes
}; // end namespace A3

#endif

namespace A3
{
namespace DataTypes
{

enum EExtensionState : int8
{
    e_Initialized,
    e_Started,
    e_ShutDown,
};

typedef std::vector< std::string > TStrVector;

}; // end namespace DataTypes
}; // end namespace A3

#endif //A3DB_DATATYPES_HPP