# CMakeLists.txt - CMake configuration file for EPANETMSX 1.1

cmake_minimum_required(VERSION 3.12)
project(CLI LANGUAGES C)

include_directories(
  ../Legacy\ DLL/include
)

include_directories(
  ../MSX\ Core/include
)

include_directories(
  ../example-api/include
)

# Append local dir to module search path
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Add building the command line executable from the run directory
add_subdirectory(run)

# Sets the output directory for executables and libraries.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Sets the position independent code property for all targets
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

IF (APPLE)
  set(INSTALL_NAME_DIR @executable_path/../lib)
  set(CMAKE_MACOSX_RPATH 1)
ENDIF (APPLE)

IF (MSVC)
  set(CMAKE_C_FLAGS_RELEASE "/GL")
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -MT)
ENDIF (MSVC)

include(FindOpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(OpenMP_C_LIB_NAMES "libomp")
else(OPENMP_FOUND)
  message("ERROR: OpenMP could not be found.")
endif(OPENMP_FOUND)

find_library(LEGACYMSX_LIB legacymsx NAMES legacymsx PATHS ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/build/lib/* ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/build/lib)
find_library(MSXCORE_LIB msxcore NAMES msxcore PATHS ${PROJECT_SOURCE_DIR}/../MSX\ Core/build/lib/* ${PROJECT_SOURCE_DIR}/../MSX\ Core/build/lib)
find_library(EXAMPLES_LIB examples NAMES examples PATHS ${PROJECT_SOURCE_DIR}/../example-api/build/lib/* ${PROJECT_SOURCE_DIR}/../example-api/build/lib)


