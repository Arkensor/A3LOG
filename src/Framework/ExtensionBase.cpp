/**********************************************************************************************************************\

    DESCRIPTION: Base extension that provides all the underlying functionality.

------------------------------------------------------------------------------------------------------------------------

    CREATION DATE:  27.12.2017

------------------------------------------------------------------------------------------------------------------------

    Copyright © 2018
    Paul L. (https://github.com/Arkensor)
    Ollie W. (https://github.com/BoGuu)
    All rights reserved!

\**********************************************************************************************************************/

#include "ExtensionBase.hpp"

#include <sstream>

#ifndef _WINDOWS
    #undef _EXTENSION_USE_CONSOLE_LOGGING
#endif

namespace A3
{
namespace Extension
{

CExtensionBase::CExtensionBase( const std::string & rstrName, const std::string & rstrVersion )
    : m_strName( rstrName )
    , m_strVersion( rstrVersion )
    , m_eExtensionState( A3::DataTypes::EExtensionState::e_Initialized )
    , m_strExtensionStateDescription( "" )
    , m_nMaxOutputSize( -1 )
    , m_strExecutablePath( std::experimental::filesystem::current_path().string() )
{
    //Parse ArmA process parameter
    auto poParameterHandler = std::make_shared< A3::Extension::StartParameter::CStartParameterHandler >();

    m_poStartParameterHandler.swap( poParameterHandler );

    //Setup mutlithreaded processing
    auto oProcessor = std::make_shared< A3::Extension::Processor::CProcessor >();

    m_poProcessor.swap( oProcessor );

#ifdef _EXTENSION_USE_CONSOLE_LOGGING
    AllocConsole();

    //Set console title
    std::string strConsoleTitle = m_strName + " " + m_strVersion;

    SetConsoleTitle( TEXT( strConsoleTitle.c_str() ) );

    //Disable key combinations
    SetConsoleCtrlHandler( nullptr, true );

    //Disable closing button
    HWND oConsoleModule = ::GetConsoleWindow();

    if ( oConsoleModule )
    {
        HMENU oMenuModule = ::GetSystemMenu( oConsoleModule, FALSE );

        if ( oMenuModule )
        {
            DeleteMenu( oMenuModule, SC_CLOSE, MF_BYCOMMAND );
        }
    }

    freopen_s( &m_oStream, "CONOUT$", "w", stdout );

    auto oConsoleLogger = spdlog::stdout_color_mt( "console" );

    m_poConsoleLogger.swap( oConsoleLogger );

    m_poConsoleLogger->set_level( loglevel::trace );

    m_poConsoleLogger->flush_on( loglevel::trace );

    m_poConsoleLogger->set_pattern( "[%Y-%m-%d %H:%M:%S] %v" );

#endif

#ifdef _EXTENSION_USE_DEFAULT_FILE_LOGGER
    std::string strLogLocation = m_strExecutablePath + "/@" + m_strName + "/Logs";

    std::string strLogName = fmt::format( "{0}/{1}_{2}.log", strLogLocation, m_strName, GetDateTime( "%Y-%m-%d_%I-%M-%S" ) );

    std::experimental::filesystem::create_directories( strLogLocation );

    auto oFileLogger = spdlog::basic_logger_mt( "general", strLogName );
    m_poFileLogger.swap( oFileLogger );

    //Collect all logs not matter the level for this.
    m_poFileLogger->set_level( loglevel::trace );

    m_poFileLogger->flush_on( loglevel::trace );

    m_poFileLogger->set_pattern( "[%Y-%m-%d %H:%M:%S] %v" );
#endif

#ifdef _EXTENSION_USE_INI_CONFIGURATION

    std::string strConfigurationPath;

    //Find the configuration.ini path in the startup parameters
    std::string strExtensionLower = _EXTENSION_NAME;
    std::transform( strExtensionLower.begin(), strExtensionLower.end(), strExtensionLower.begin(), ::tolower );
    if( m_poStartParameterHandler->m_oStartParamters.count( strExtensionLower ) > 0 )
    {
        strConfigurationPath = m_poStartParameterHandler->m_oStartParamters.at( strExtensionLower );
    }

    //Find the configuration file in the filesystem
    if( strConfigurationPath.empty() )
    {
        std::string strConfigName;

#ifdef _EXTENSION_CONFIGFILE_NAME
        strConfigName = _EXTENSION_CONFIGFILE_NAME;
#else
        strConfigName = _EXTENSION_NAME;
        strConfigName += ".ini";
#endif

        TRecursiveIterator oEndIt;

        for ( TRecursiveIterator oDirIt( std::experimental::filesystem::current_path() ); oDirIt != oEndIt; ++oDirIt )
        {
            const std::experimental::filesystem::path oFile = ( *oDirIt );

            if ( !std::experimental::filesystem::is_directory( oFile ) &&
                 !std::experimental::filesystem::is_socket( oFile ) )
            {
                if ( oFile.string().substr( oFile.string().size() - strConfigName.size() ) == strConfigName )
                {
                    strConfigurationPath = oFile.string();
                }
            }
        }
    }

    if( strConfigurationPath.empty() )
    {
        throw std::runtime_error( "Configuration file could not be located." );
    }

    auto oConfigReader = std::make_shared< INIReader >( strConfigurationPath );

    m_poConfiguration.swap( oConfigReader );
#endif
}

CExtensionBase::~CExtensionBase()
{
}

std::string
CExtensionBase::GetDateTime( std::string strFormat )
{
    time_t oRawTime;
    struct tm * oTimeInfo;
    char buffer[ 256 ];

    time ( &oRawTime );
    oTimeInfo = localtime( &oRawTime );

    strftime( buffer, sizeof( buffer ), strFormat.c_str(), oTimeInfo );

    return std::string( buffer );
}

int
CExtensionBase::call( char * pstrOutput, int nOutputSize, const char *pstrFunction, const char **pArguments, int nArguments )
{
    --nOutputSize;

    if( m_nMaxOutputSize == -1 )
    {
        m_nMaxOutputSize = nOutputSize;
    }

    if( !m_poProcessor->m_bActive )
    {
        #ifdef _EXTENSION_MAX_THREADS
            #define _EXTENSION_THREAD_COUNT _EXTENSION_MAX_THREADS
        #else
            #define _EXTENSION_THREAD_COUNT -1
        #endif

        m_poProcessor->start( std::bind( &A3::Extension::CExtensionBase::Worker, this, std::placeholders::_1 ), _EXTENSION_THREAD_COUNT );

        Setup(); //Load custom setup from outside the framework
    }

    if( nArguments )
    {
        AddWorkloads(pstrFunction, pArguments, nArguments);
    }

    //Opening and closing brackets.
    A3::DataTypes::uint64 nCurrentSize = 2;

    std::string strResult = "[";

    std::vector< A3::Extension::Processor::CProcessorResult > results;

    //Check for results and append them
    if (this->CheckResults(results, nCurrentSize) )
    {
        for ( auto oResult : results )
        {
            strResult += ResultToString(oResult) + ",";
        }

        strResult.pop_back();
    }

    strResult += "]"; //Append closing bracket

    strncpy( pstrOutput, strResult.c_str(), ( size_t ) nOutputSize );

    return 0;
}

std::vector< A3::Extension::Processor::CProcessorResult >
CExtensionBase::Worker( A3::Extension::Processor::CProcessorWorkload oWorkload )
{
    return SplitResult(Execute(oWorkload), oWorkload);
}

void
CExtensionBase::AddWorkloads( const char *pcExtensionFunction, const char **pExtensionData, int nExtensionDataCount )
{
    for ( int nExtensionDataSet = 0; nExtensionDataSet < nExtensionDataCount; ++nExtensionDataSet )
    {
        std::string strArguments( pExtensionData[ nExtensionDataSet ] );
        strArguments = strArguments.substr( 1, ( strArguments.size() - 2 ) );
        A3::DataTypes::TStrVector oArguments = Split(strArguments, 29);
        std::string strID;
        std::string strFunction( pcExtensionFunction );

        if( oArguments.size() > 1 )
        {
            strID = oArguments[ 0 ];
            oArguments.erase( oArguments.begin() );

            //In case we do not need a result
            if( strID == "-1" )
            {
                strID.clear();
            }
        }

        A3::Extension::Processor::CProcessorWorkload oWorkload( strID, strFunction, oArguments );

        m_poProcessor->Add( oWorkload );
    }
}

bool
CExtensionBase::CheckResults( std::vector<A3::Extension::Processor::CProcessorResult> &roResults,
                              A3::DataTypes::uint64 nCurrentSize)
{
    return m_poProcessor->try_get_results( roResults, nCurrentSize, m_nMaxOutputSize );
}

std::vector< std::string >
CExtensionBase::Split(const std::string &rstrData, char cDelimiter)
{
    std::vector< std::string > oResult;
    std::stringstream string_stream( rstrData );
    std::string strNextPart;

    while ( std::getline( string_stream, strNextPart, cDelimiter ) )
    {
        oResult.push_back( strNextPart );
    }

    return oResult;
}

std::string
CExtensionBase::ResultToString( A3::Extension::Processor::CProcessorResult &roResult )
{
    return fmt::format( "[{0},{1},\"{2}\"]",
                        roResult.m_strID,
                        ( roResult.m_bIsMultiPart ? std::to_string( roResult.m_nPartIndex ) : "-1" ),
                        roResult.m_strData );
}

std::vector< A3::Extension::Processor::CProcessorResult >
CExtensionBase::SplitResult( std::string strResult, A3::Extension::Processor::CProcessorWorkload &roWorkload )
{
    std::vector< A3::Extension::Processor::CProcessorResult > oResults;

    //Workload hat has no id to return oResults -> input only call
    if( roWorkload.m_strID.empty() )
    {
        return oResults;
    }

    //Need to find the most optimal max size to store, to if there is lots of big oResults, little oResults get returned too..

    auto nMaxReturnConsumption = ( A3::DataTypes::uint64 )( 0.8 * m_nMaxOutputSize ); //Allow up to 80% of the return to be one request
    auto nParts = strResult.size() / nMaxReturnConsumption; //Amount of parts for the case of too much data for one
    bool bIsMultiPart = ( nParts != 0 ); //Do we have multipart?

    //Build Results as long as our data output is longer than one Result
    while ( strResult.size() > nMaxReturnConsumption )
    {
        oResults.emplace_back( A3::Extension::Processor::CProcessorResult (
                                roWorkload.m_strID,
                                strResult.substr( 0, ( size_t ) nMaxReturnConsumption ), true, nParts ) );

        strResult = strResult.substr( ( size_t )nMaxReturnConsumption );

        nParts--;
    }

    //Put in final Result, or the only one in case it was small enough for one anyway
    oResults.emplace_back( A3::Extension::Processor::CProcessorResult( roWorkload.m_strID,
                                                                       strResult,
                                                                       bIsMultiPart,
                                                                       ( A3::DataTypes::uint64 ) ( bIsMultiPart ? 0 : -1 ) ) );

    return oResults;
}

}; // end namespace CExtension
}; // end namespace A3