#ifndef A3DB_PROCESSORRESULT_HPP
#define A3DB_PROCESSORRESULT_HPP

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

#include "../Configuration.hpp"
#include "DataTypes.hpp"

#include <string>

namespace A3
{
namespace Extension
{
namespace Processor
{

class CProcessorResult
{
public:
    CProcessorResult( const std::string & rstrID,
                      const std::string & rstrData );

    CProcessorResult(const std::string & rstrID,
                     const std::string & rstrData,
                     bool bIsMultiPart ,
                     A3::DataTypes::uint64 nPartIndex );

    virtual ~CProcessorResult();

    std::string m_strID;
    std::string m_strData;
    bool m_bIsMultiPart;
    A3::DataTypes::uint64 m_nPartIndex;
};

}; // end namespace CProcessor
}; // end namespace Extension
}; // end namespace A3

#endif //A3DB_PROCESSORRESULT_HPP