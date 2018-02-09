/**********************************************************************************************************************\

    DESCRIPTION: Class that contains all information and data that is returned from the worker threads

------------------------------------------------------------------------------------------------------------------------

    CREATION DATE:  31.12.2017

------------------------------------------------------------------------------------------------------------------------

    Copyright © 2018
    Paul L. (https://github.com/Arkensor)
    Ollie W. (https://github.com/BoGuu)
    All rights reserved!

\**********************************************************************************************************************/

#include "ProcessorResult.hpp"

namespace A3
{
namespace Extension
{
namespace Processor
{

CProcessorResult::CProcessorResult( const std::string & rstrID,
                                    const std::string & rstrData )
    : m_strID( rstrID )
    , m_strData( rstrData )
    , m_bIsMultiPart( false )
    , m_nPartIndex( 0 )
{
}

CProcessorResult::CProcessorResult( const std::string & rstrID,
                                    const std::string & rstrData,
                                    bool bIsMultiPart ,
                                    A3::DataTypes::uint64 nPartIndex )
    : m_strID( rstrID )
    , m_strData( rstrData )
    , m_bIsMultiPart( bIsMultiPart )
    , m_nPartIndex( nPartIndex )
{
}

CProcessorResult::~CProcessorResult()
{
}

}; // end namespace CProcessor
}; // end namespace Extension
}; // end namespace A3