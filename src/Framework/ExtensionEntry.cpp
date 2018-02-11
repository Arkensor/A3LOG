/**********************************************************************************************************************\

    DESCRIPTION: Entry point for the extension

------------------------------------------------------------------------------------------------------------------------

    CREATION DATE:  28.12.2017

------------------------------------------------------------------------------------------------------------------------

    Copyright © 2018
    Paul L. (https://github.com/Arkensor)
    Ollie W. (https://github.com/BoGuu)
    All rights reserved!

\**********************************************************************************************************************/

#include "ExtensionBase.hpp"

A3::Extension::CExtensionBase *g_Extension;

#ifdef _WINDOWS
#define _EXTENSION_EXPORT __declspec ( dllexport )
#define _EXTENSION_STDCALL __stdcall
int __stdcall DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
#else
#define _EXTENSION_EXPORT
#define _EXTENSION_STDCALL
static void __attribute__( ( constructor ) ) extension_init( void )
#endif
{
#ifdef _WINDOWS
        switch ( ul_reason_for_call )
        {
            case DLL_PROCESS_ATTACH:
#endif
                g_Extension = new A3::Extension::CExtensionBase( _EXTENSION_NAME, _EXTENSION_VERSION);
#ifdef _WINDOWS
                break;

            case DLL_PROCESS_DETACH:
                delete g_Extension;
                break;

            default:
                break;
        }

    return 1;
#endif
}

#ifndef _WINDOWS
static void __attribute__( ( destructor ) )    extension_destroy( void )
{
    delete g_Extension;
}
#endif

extern "C"
{
    _EXTENSION_EXPORT void _EXTENSION_STDCALL RVExtension( char *output, int outputSize, const char *function );
    _EXTENSION_EXPORT int  _EXTENSION_STDCALL RVExtensionArgs( char *output, int outputSize, const char *function, const char **args, int argCnt );
    _EXTENSION_EXPORT void _EXTENSION_STDCALL RVExtensionVersion( char *output, int outputSize );
}

void _EXTENSION_STDCALL RVExtensionVersion( char *output, int outputSize )
{
    strncpy( output, _EXTENSION_VERSION , ( size_t ) --outputSize );
}

void _EXTENSION_STDCALL RVExtension( char *output, int outputSize, const char *function )
{
    RVExtensionArgs( output, outputSize, "__RVEXTENSION__", &function, 1 );
}

int _EXTENSION_STDCALL RVExtensionArgs( char *output, int outputSize, const char *function, const char **args, int argCnt )
{
    if( g_Extension->m_eExtensionState != A3::DataTypes::EExtensionState::e_ShutDown )
    {
        try
        {
            return g_Extension->call(output, outputSize, function, args, argCnt);
        }
        catch ( std::exception & oException )
        {
            g_Extension->m_eExtensionState = A3::DataTypes::EExtensionState::e_ShutDown;
            g_Extension->m_strExtensionStateDescription = oException.what();
        }
        catch ( ... )
        {
            g_Extension->m_eExtensionState = A3::DataTypes::EExtensionState::e_ShutDown;
            g_Extension->m_strExtensionStateDescription = "An unknown exception has occurred.";
        }
    }

    strncpy( output, std::string(
        fmt::format( "{0} {1} - A fatal error has occurred, the extension stopped working! Details: {2}",
                     g_Extension->m_strName,
                     g_Extension->m_strVersion,
                     g_Extension->m_strExtensionStateDescription ) ).c_str(), ( size_t ) --outputSize );

    return -1;
}