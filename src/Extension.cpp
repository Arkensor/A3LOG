/**********************************************************************************************************************\

    DESCRIPTION: A3LOG - A lightweight logging extension

------------------------------------------------------------------------------------------------------------------------

    CREATION DATE:  02.02.2018

------------------------------------------------------------------------------------------------------------------------

    Copyright © 2018
    Paul L. (https://github.com/Arkensor)
    All rights reserved!

    Based on A3EXT (https://github.com/Arkensor/A3EXT) by
    Paul L. (https://github.com/Arkensor)
    Ollie W. (https://github.com/BoGuu)

\**********************************************************************************************************************/

#include "Framework/ExtensionBase.hpp"

namespace A3
{
namespace Extension
{

spdlog::level::level_enum
parseLogLevel( std::string strLogLevel )
{
    std::transform( strLogLevel.begin(), strLogLevel.end(), strLogLevel.begin(), ::tolower );

    if( strLogLevel == "trace" )
    {
        return loglevel::trace;
    }
    else if ( strLogLevel == "debug" )
    {
        return loglevel::debug;
    }
    else if ( strLogLevel == "warn" )
    {
        return loglevel::warn;
    }
    else if ( strLogLevel == "err" || strLogLevel == "error" )
    {
        return loglevel::err;
    }
    else if ( strLogLevel == "crit" || strLogLevel == "critical" )
    {
        return loglevel::critical;
    }

    return loglevel::info;
}

/** Setup is called once before the first workload is processed
 * @Return: void
 */
void
CExtensionBase::Setup()
{
    m_poConsoleLogger->set_level( loglevel::trace );

    for( auto & oSection : m_poConfiguration->Sections() )
    {
        std::string strSectionLower = oSection;
        std::transform( strSectionLower.begin(), strSectionLower.end(), strSectionLower.begin(), ::tolower );

        if( strSectionLower != "settings" )
        {
            std::string strFileName = m_poConfiguration->Get( oSection, "FileName", oSection );

            if( !strFileName.empty() )
            {
                std::string strDirectory = m_poConfiguration->Get( oSection, "Directory", m_strExecutablePath );

                std::string strPattern = m_poConfiguration->Get( oSection, "Pattern", "[%Y-%m-%d %H:%M:%S] [%l] %v" );

                std::string strLogLevel = m_poConfiguration->Get( oSection, "LogLevel", "trace" );

                bool bRotated = m_poConfiguration->GetBoolean( oSection, "Rotated", false );

                bool bFileNameIncludeTimestamp = m_poConfiguration->GetBoolean( oSection, "FileNameIncludeTimestamp", false );

                bool bFileNameIncludeDate = m_poConfiguration->GetBoolean( oSection, "FileNameIncludeDate", false );

                std::string strFullLogPath = strDirectory + ( (strDirectory.back() == '/' ||strDirectory.back() == '\\') ? "" : "/" );

                if( bRotated )
                {
                    strFullLogPath += GetDateTime( "%Y/%m/%d/" );
                }

                strFullLogPath += std::experimental::filesystem::path( strFileName ).stem().string();

                if( bFileNameIncludeDate )
                {
                    strFullLogPath += GetDateTime( "_%Y-%m-%d" );
                }

                if( bFileNameIncludeTimestamp )
                {
                    strFullLogPath += GetDateTime( "_%I-%M-%S" );
                }

                strFullLogPath += std::experimental::filesystem::path( strFileName ).extension().string();

                try
                {
                    if( !std::experimental::filesystem::is_directory( std::experimental::filesystem::path( strFullLogPath ).parent_path() ) )
                    {
                        if( !std::experimental::filesystem::create_directories( std::experimental::filesystem::path( strFullLogPath ).parent_path() ) )
                        {
                            throw std::exception();
                        }
                    }
                }
                catch ( ... )
                {
                    throw std::exception( fmt::format( "Could not create log directory for {0}: {1}", oSection, strFullLogPath ).c_str() );
                }

                auto oFileLogger = spdlog::basic_logger_mt( strSectionLower, strFullLogPath );

                oFileLogger->set_level( parseLogLevel( strLogLevel ) );

                oFileLogger->set_pattern( strPattern );

                oFileLogger->flush_on( loglevel::trace );
            }
        }
    }
}

/** Execute is called for each workload that is processed
 * @Return: Returns a string as result to the processed workload. This string
 *          is then returned to the sqf side function.
 *          Returns could be prepared for easy parsing in ArmA via parseSimpleArray.
 *          -> https://community.bistudio.com/wiki/parseSimpleArray
 */
std::string
CExtensionBase::Execute( A3::Extension::Processor::CProcessorWorkload oWorkload )
{
    if( !oWorkload.m_oArguments.empty() )
    {
        std::string strCategoryLower = "general";

        if( oWorkload.m_oArguments.size() > 1 )
        {
            if( !oWorkload.m_oArguments [ 1 ].empty() )
            {
                strCategoryLower = oWorkload.m_oArguments [ 1 ] ;
            }
        }

        std::transform( strCategoryLower.begin(), strCategoryLower.end(), strCategoryLower.begin(), ::tolower );

        std::shared_ptr< spdlog::logger > oLogger = spdlog::get( strCategoryLower );

        spdlog::level::level_enum eLevel = loglevel::info;

        if( oWorkload.m_oArguments.size() > 2 )
        {
            eLevel = parseLogLevel( oWorkload.m_oArguments [ 2 ] );
        }

        if( oLogger != nullptr )
        {
            oLogger->log( eLevel, oWorkload.m_oArguments [ 0 ] );
        }

        if( m_poConfiguration->GetBoolean( "Settings", "EnableConsoleOutput", false ) )
        {
            m_poConsoleLogger->log( eLevel, "[{0}] {1}", strCategoryLower, oWorkload.m_oArguments [ 0 ] );
        }
    }

    return "";
}

}; // end namespace Extension
}; // end namespace A3