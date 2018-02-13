#ifndef A3EXT_CONFIGURATION_HPP
#define A3EXT_CONFIGURATION_HPP
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

/**
 * Extension configuration
 */
#define _EXTENSION_NAME    "A3LOG"
#define _EXTENSION_VERSION "1.8"

/**
 * Define the maximum amount of threads that will be created
 * If not defined, there will be a thread for each cpu core
 */
#define _EXTENSION_MAX_THREADS 4

/**
 * Do you want to print something to the console? [Windows only]
 */
#define _EXTENSION_USE_CONSOLE_LOGGING

/**
 * Using this provides you with a default log file that you can write to.
 * The default folder for the log files is @<Extensionname>/Logs/
 * The default name for the logfile is <Extensioname>.log
 */
//#define _EXTENSION_USE_DEFAULT_FILE_LOGGER

/**
 * Do you want to make use of the ini configuration system?
 */
#define _EXTENSION_USE_INI_CONFIGURATION

/**
 * You may specify the name of the config file
 * that will be searched in the entire arma directory
 * and all subfolders.
 *
 * The default value for this is <Extensionname>.ini
 *
 * NOTE: This option will be overwritten when the user adds a -<Extensionname>=some/path/to/config.ini in the start parameters.
 */
#define _EXTENSION_CONFIGFILE_NAME "A3LOG.ini"

#endif //A3EXT_CONFIGURATION_HPP