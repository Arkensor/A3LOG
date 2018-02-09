#ifndef A3DB_PROCESSORQUEUE_HPP
#define A3DB_PROCESSORQUEUE_HPP

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

#include "DataTypes.hpp"

#include <queue>
#include <mutex>
#include <condition_variable>

namespace A3
{
namespace Extension
{
namespace Processor
{

template < class T >
class CProcessorQueue
{
public:
    CProcessorQueue();
    virtual ~CProcessorQueue();

    void push( T oElement );

    T pop();

    bool try_pop_results( std::vector< T > &roElements,
                          A3::DataTypes::int64 nCurrentSize,
                          A3::DataTypes::int64 nMaxSize );

    size_t size();

protected:
    std::queue< T > m_oQueue;
    mutable std::mutex m_oQueueLock;
    std::condition_variable m_oConditionVariable;
};

}; // end namespace Processor
}; // end namespace Extension
}; // end namespace A3

#include "ProcessorQueue.tpp"

#endif //A3DB_PROCESSORQUEUE_HPP
