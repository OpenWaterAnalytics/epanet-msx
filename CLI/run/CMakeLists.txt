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
if(NOT WIN32)
  target_link_libraries(runepanetmsx LINK_PUBLIC legacy m)
else(NOT WIN32)
  if ("${CMAKE_VS_PLATFORM_NAME}" MATCHES "(x64)")
    find_library(LEGACY_LIB legacy NAMES legacy PATHS ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/build/lib/* NO_DEFAULT_PATH)
    find_library(CORE_LIB core NAMES core PATHS ${PROJECT_SOURCE_DIR}/../MSX\ Core/build/lib/* NO_DEFAULT_PATH)
    target_link_libraries(runepanetmsx LINK_PUBLIC ${LEGACY_LIB} ${CORE_LIB})
  else(TRUE)
    find_library(LEGACY_LIB legacy NAMES legacy PATHS ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/build/lib NO_DEFAULT_PATH)
    find_library(CORE_LIB core NAMES core PATHS ${PROJECT_SOURCE_DIR}/../MSX\ Core/build/lib NO_DEFAULT_PATH)
    target_link_libraries(runepanetmsx LINK_PUBLIC ${LEGACY_LIB} ${CORE_LIB})
  endif("${CMAKE_VS_PLATFORM_NAME}" MATCHES "(x64)")
endif(NOT WIN32)
