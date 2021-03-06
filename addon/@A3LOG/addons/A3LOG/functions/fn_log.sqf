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

if ( ( _data isEqualTo "" ) || ( uiNamespace getVariable [ "A3LOG_FAILURE", false ] ) ) exitWith
{
    false;
};

private _result = "A3LOG" callExtension [ "LOG", [ format [ "-1%1%2%1%3%1%4", toString [ 29 ], _data, _category, _level] ] ];

if ( ( ( _result param [ 0 ] ) isEqualTo "[]" ) &&
     ( ( _result param [ 1 ] ) isEqualTo 0 ) &&
     ( ( _result param [ 2 ] ) isEqualTo 0 ) ) exitWith
{
    true;
};

diag_log format[ "A3LOG: %1", ( _result param [ 0 ] ) ];

with uiNamespace do 
{
	A3LOG_FAILURE = true;
};

false;