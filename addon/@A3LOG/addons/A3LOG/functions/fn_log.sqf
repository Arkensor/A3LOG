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

params
[
    [ "_data", "", [ "" ] ],
	[ "_category", "", [ "" ] ],
	[ "_level", "", [ "" ] ]
];

if( _data isEqualTo "" ) exitWith { false; };

"A3LOG" callExtension [ "LOG", [ format [ "-1%1%2%1%3%1%4", toString [ 29 ], _data, _category, _level] ] ];

true;