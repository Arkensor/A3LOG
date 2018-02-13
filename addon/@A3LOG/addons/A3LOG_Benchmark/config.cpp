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

class CfgPatches
{
    class A3LOG_Benchmark
    {
        requiredVersion = 1.67;
		requiredAddons[] = { "A3LOG" };
        fileName = "A3LOG_Benchmark.pbo";
        author = "Arkensor";
    };
};

class CfgFunctions
{
    class A3LOG_Benchmark
    {
        tag = "A3LOG_Benchmark";

        class Functions 
        {
            file = "\A3LOG_Benchmark\functions";
			class benchmark { postInit = 1; headerType = -1; };
        };
    };
};