#ifndef A3DB_STARTPARAMETERHANDLER_HPP
#define A3DB_STARTPARAMETERHANDLER_HPP
/**********************************************************************************************************************\

    DESCRIPTION: Handler to option a list of parameters, mods and server mods from the ArmA server

------------------------------------------------------------------------------------------------------------------------

    CREATION DATE:  30.12.2017

------------------------------------------------------------------------------------------------------------------------

    Copyright © 2018
    Paul L. (https://github.com/Arkensor)
    Ollie W. (https://github.com/BoGuu)
    All rights reserved!

\**********************************************************************************************************************/

#include <map>
#include <vector>

namespace A3
{
namespace Extension
{
namespace StartParameter
{

class CStartParameterHandler
{
public:
    CStartParameterHandler();
    virtual ~CStartParameterHandler();

public:
    std::map< std::string, std::string > m_oStartParamters;
    std::vector< std::string > m_oMods;
    std::vector< std::string > m_oServerMods;

#ifndef _WINDOWS
//Linux compatibility
protected:
    typedef char* LPSTR;
    LPSTR GetCommandLineA();
#endif
};

}; // end namespace StartParameter
}; // end namespace Extension
}; // end namespace A3

#endif //A3DB_STARTPARAMETERHANDLER_HPP
