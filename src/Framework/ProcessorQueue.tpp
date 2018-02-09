/**********************************************************************************************************************\

    DESCRIPTION: Thread safe locking queue

------------------------------------------------------------------------------------------------------------------------

    CREATION DATE:  31.12.2017

------------------------------------------------------------------------------------------------------------------------

    Copyright © 2018
    Paul L. (https://github.com/Arkensor)
    Ollie W. (https://github.com/BoGuu)
    All rights reserved!

\**********************************************************************************************************************/

namespace A3
{
namespace Extension
{
namespace Processor
{

template < class T >
CProcessorQueue< T >::CProcessorQueue()
{
}

template < class T >
CProcessorQueue< T >::~CProcessorQueue()
{
}

template < class T >
void
CProcessorQueue< T >::push( T oElement )
{
    std::lock_guard< std::mutex > lock( m_oQueueLock );
    m_oQueue.push( oElement );
    m_oConditionVariable.notify_one();
}

template < class T >
T
CProcessorQueue< T >::pop()
{
    std::unique_lock< std::mutex > lock( m_oQueueLock );

    while ( m_oQueue.empty() )
    {
        m_oConditionVariable.wait(lock);
    }

    T oElement = m_oQueue.front();

    m_oQueue.pop();

    return oElement;
}

template < class T >
bool
CProcessorQueue< T >::try_pop_results( std::vector< T > & roElements,
                                       A3::DataTypes::int64 nCurrentSize,
                                       A3::DataTypes::int64 nMaxSize )
{
    --nMaxSize;

    if ( nCurrentSize >= nMaxSize )
    {
        return false;
    }

    std::lock_guard< std::mutex > lock( m_oQueueLock );

    if ( m_oQueue.empty() )
    {
        return false;
    }

    while ( !m_oQueue.empty() )
    {
        T oElement = m_oQueue.front();

        A3::DataTypes::int64 nLength = oElement.m_strData.size() + oElement.m_strID.size() + 12; //Extra characters in ARMA array e.g [] & str esc.

        if ( ( nLength + nCurrentSize ) > nMaxSize )
        {
            break;
        }

        m_oQueue.pop();

        roElements.push_back( oElement );

        nCurrentSize += nLength;
    }

    return !roElements.empty();
}

template < class T >
size_t
CProcessorQueue< T >::size()
{
    std::lock_guard< std::mutex > lock( m_oQueueLock );
    return m_oQueue.size();
}

}; // end namespace Processor
}; // end namespace Extension
}; // end namespace A3