#ifndef A3DB_EXTENSIONBASE_HPP
#define A3DB_EXTENSIONBASE_HPP
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

#include "../Configuration.hpp"

#include "DataTypes.hpp"
#include "Processor.hpp"
#include "StartParameterHandler.hpp"

//Required thridparty includes
#include "ThridParty/spdlog/spdlog.h"

#define LOG_ALL( level, message) \
spdlog::apply_all([&](std::shared_ptr<spdlog::logger> oLogger){oLogger->log( level, message ); oLogger->flush();});

#define LOG_ALL_FORMAT( level, message, ...) \
spdlog::apply_all([&](std::shared_ptr<spdlog::logger> oLogger){oLogger->log( level, message, __VA_ARGS__); oLogger->flush();});

namespace loglevel = spdlog::level;

//Optional includes
#ifdef _EXTENSION_USE_INI_CONFIGURATION
    #include "ThridParty/inih/INIReader.hpp"
#endif

//System includes
#include <string>
#include <memory>
#include <filesystem>

namespace A3
{
namespace Extension
{

class CExtensionBase
{
public:
    CExtensionBase( const std::string & rstrName, const std::string & rstrVersion );
    virtual ~CExtensionBase();

    int
    call( char * pstrOutput, int nOutputSize, const char *pstrFunction, const char **pArguments, int nArguments );

public:
    std::string m_strName;
    std::string m_strVersion;
    std::string m_strExecutablePath;
    A3::DataTypes::int64 m_nMaxOutputSize;
    A3::DataTypes::EExtensionState m_eExtensionState;
    std::string m_strExtensionStateDescription;

protected:
    typedef std::experimental::filesystem::recursive_directory_iterator TRecursiveIterator;

    std::shared_ptr< A3::Extension::Processor::CProcessor > m_poProcessor;

    std::shared_ptr< A3::Extension::StartParameter::CStartParameterHandler > m_poStartParameterHandler;

#ifdef _EXTENSION_USE_CONSOLE_LOGGING
    FILE *m_oStream;
    std::shared_ptr< spdlog::logger > m_poConsoleLogger;
#endif

#ifdef _EXTENSION_USE_DEFAULT_FILE_LOGGER
    std::shared_ptr< spdlog::logger > m_poFileLogger;
#endif

#ifdef _EXTENSION_USE_INI_CONFIGURATION
    std::shared_ptr< INIReader > m_poConfiguration;
#endif

protected:
    void
    Setup();

    std::string
    Execute( A3::Extension::Processor::CProcessorWorkload oWorkload );

    std::string
    GetDateTime( std::string strFormat );

    std::vector< A3::Extension::Processor::CProcessorResult >
    Worker( A3::Extension::Processor::CProcessorWorkload oWorkload );

    void
    AddWorkloads(const char *pcExtensionFunction, const char **pExtensionData, int nExtensionDataCount);

    bool
    CheckResults( std::vector< A3::Extension::Processor::CProcessorResult > & roResults, A3::DataTypes::uint64 nCurrentSize );

    A3::DataTypes::TStrVector
    Split( const std::string &rstrData, char cDelimiter );

    std::string
    ResultToString(A3::Extension::Processor::CProcessorResult &roResult);

    std::vector< A3::Extension::Processor::CProcessorResult >
    SplitResult( std::string strResult, A3::Extension::Processor::CProcessorWorkload &roWorkload );
};

}; // end namespace Extension
}; // end namespace A3

#endif //A3DB_EXTENSIONBASE_HPP