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
    class A3LOG
    {
        requiredVersion = 0.1;
        fileName = "A3LOG.pbo";
        author = "Arkensor";
    };
};

class CfgFunctions
{
    class A3LOG
    {
        tag = "A3LOG";

        class Functions 
        {
            file = "\A3LOG\functions";

            class log { headerType = -1; };
        };
		
		class Benchmark
        {
            file = "\A3LOG"
            class benchmark { postInit = 1; headerType = -1; };
        };
    };
};