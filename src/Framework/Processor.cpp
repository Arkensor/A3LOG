/**********************************************************************************************************************\

    DESCRIPTION: Processor that handles incoming jobs using multiple threads

------------------------------------------------------------------------------------------------------------------------

    CREATION DATE:  31.12.2017

------------------------------------------------------------------------------------------------------------------------

    Copyright © 2018
    Paul L. (https://github.com/Arkensor)
    Ollie W. (https://github.com/BoGuu)
    All rights reserved!

\**********************************************************************************************************************/

#include "Processor.hpp"

#include <iostream>

namespace A3
{
namespace Extension
{
namespace Processor
{

CProcessor::CProcessor()
    : m_bActive( false )
{
}

CProcessor::~CProcessor()
{
}

void
CProcessor::start( std::function< std::vector< CProcessorResult >( CProcessorWorkload ) > oFunction ,
                   A3::DataTypes::int8 nThreads )
{
    if ( m_bActive )
    {
        return;
    }

    m_bActive = true;

    auto nAvailableThreads = ( A3::DataTypes::int8 ) std::thread::hardware_concurrency();

    if ( ( nThreads == -1 ) || ( nThreads > nAvailableThreads ) )
    {
        nThreads = nAvailableThreads;
    }


    for ( int nThread = 0; nThread < nThreads; ++nThread )
    {
        std::thread oThread( [=] { run( oFunction ); } );

        oThread.detach();
    }
}

void
CProcessor::Add( CProcessorWorkload & roWorkload )
{
    oWorkloadQueue.push( roWorkload );
}

bool
CProcessor::try_get_results( std::vector<CProcessorResult> & oWorkload,
                             A3::DataTypes::uint64 nCurrentSize,
                             A3::DataTypes::int64 nMaxSize )
{
    return oResultQueue.try_pop_results( oWorkload, nCurrentSize, nMaxSize );
}

void
CProcessor::run( std::function< std::vector< CProcessorResult >( CProcessorWorkload ) > oFunction )
{
    while ( m_bActive )
    {
        CProcessorWorkload oWorkLoad = oWorkloadQueue.pop();

        std::vector< CProcessorResult > oResults = oFunction( oWorkLoad );

        for ( CProcessorResult & result : oResults )
        {
            oResultQueue.push( result );
        }
    }
}

}; // end namespace Processor
}; // end namespace Extension
}; // end namespace A3