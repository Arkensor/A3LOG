# This is the important bit for you, if you want to change the name of your extension in the build system
set( EXTENSION_NAME A3LOG )

# Only edit below if you need to
cmake_minimum_required( VERSION 3.8 )

project( ${EXTENSION_NAME} )

SOURCE_GROUP( "Template Files" REGULAR_EXPRESSION ".*tpp" )
SOURCE_GROUP( "Resouce Files" REGULAR_EXPRESSION ".*rc" )

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin/release )

if( MSVC )
#    add_compile_options( /W4 )
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
#    add_compile_options( -Wall -Wextra )
endif()

file( GLOB TARGET_SOURCES Framework/*.hpp Framework/*.tpp Framework/*.cpp *.cpp *.hpp )

set( SOURCE_FILES ${TARGET_SOURCES} )

add_library( ${EXTENSION_NAME} SHARED ${SOURCE_FILES} )

target_include_directories( ${EXTENSION_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Framework/ThridParty )