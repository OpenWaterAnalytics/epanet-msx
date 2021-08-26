# EPANETMSX COMMAND LINE EXECUTABLE
cmake_minimum_required (VERSION 3.9)


# Sets for output directory for executables and libraries.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Sets the position independent code property for all targets.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Link to multi-threaded static runtime library
IF (MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -MT)
ENDIF (MSVC)


# Set up file groups for exe target
set(MSX_CLI_SOURCES msxmain.c)
#include_directories(include)

source_group("CLI" FILES ${MSX_CLI_SOURCES})


# Creates the EPANET-MSX command line executable
add_executable(runepanetmsx ${MSX_CLI_SOURCES})

if (MSVC)
  find_library(LEGACYMSX_LIB legacymsx NAMES legacymsx PATHS ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/build/lib/* NO_DEFAULT_PATH)
  find_library(MSXCORE_LIB msxcore NAMES msxcore PATHS ${PROJECT_SOURCE_DIR}/../MSX\ Core/build/lib/* NO_DEFAULT_PATH)
  find_library(EXAMPLES_LIB examples NAMES examples PATHS ${PROJECT_SOURCE_DIR}/../example-api/build/lib/* NO_DEFAULT_PATH)
  target_link_libraries(runepanetmsx LINK_PUBLIC ${LEGACYMSX_LIB} ${MSXCORE_LIB} ${EXAMPLES_LIB})
else(TRUE)
  find_library(LEGACYMSX_LIB legacymsx NAMES legacymsx PATHS ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/build/lib NO_DEFAULT_PATH)
  find_library(MSXCORE_LIB msxcore NAMES msxcore PATHS ${PROJECT_SOURCE_DIR}/../MSX\ Core/build/lib NO_DEFAULT_PATH)
  find_library(EXAMPLES_LIB examples NAMES examples PATHS ${PROJECT_SOURCE_DIR}/../example-api/build/lib NO_DEFAULT_PATH)
  target_link_libraries(runepanetmsx LINK_PUBLIC ${LEGACYMSX_LIB} ${MSXCORE_LIB} ${EXAMPLES_LIB})
endif(MSVC)

