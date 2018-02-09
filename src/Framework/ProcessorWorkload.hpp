#ifndef A3DB_WORKLOAD_HPP
#define A3DB_WORKLOAD_HPP
/**********************************************************************************************************************\

    DESCRIPTION: Workloads, parsed from the ArmA input

------------------------------------------------------------------------------------------------------------------------

    CREATION DATE:  31.12.2017

------------------------------------------------------------------------------------------------------------------------

    Copyright © 2018
    Paul L. (https://github.com/Arkensor)
    Ollie W. (https://github.com/BoGuu)
    All rights reserved!

\**********************************************************************************************************************/

#include "DataTypes.hpp"

#include <string>
#include <vector>

namespace A3
{
namespace Extension
{
namespace Processor
{

class CProcessorWorkload
{
public:
    CProcessorWorkload( const std::string & rstrID,
                        const std::string & rstrFunction,
                        const A3::DataTypes::TStrVector & roArguments );

    virtual ~CProcessorWorkload();

    std::string m_strID;
    std::string m_strFunction;
    A3::DataTypes::TStrVector m_oArguments;
};

}; // end namespace CProcessor
}; // end namespace Extension
}; // end namespace A3

#endif //A3DB_WORKLOAD_HPP
