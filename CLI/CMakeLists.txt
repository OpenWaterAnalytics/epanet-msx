# CMakeLists.txt - CMake configuration file for EPANETMSX 1.1

cmake_minimum_required(VERSION 3.12)
project(EPANETMSX LANGUAGES C)

#include_directories(
#  ../Legacy\ DLL/EPANET/include
#)
include_directories(
  ../Legacy\ DLL/include
)

include_directories(
  ../Transport-Chemistry/include
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

# configure file groups
file(GLOB MSX_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} src/*.c)

include(FindOpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(OpenMP_C_LIB_NAMES "libomp")
else(OPENMP_FOUND)
  message("ERROR: OpenMP could not be found.")
endif(OPENMP_FOUND)

#find_library(EPANET_LIB epanet2 NAMES epanet2 PATHS ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/EPANET/build/lib/* ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/EPANET/build/lib)
find_library(EPANETMSX_LIB epanetmsx NAMES epanetmsx PATHS ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/build/lib/* ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/EPANET/build/lib)

#target_link_libraries(${EPANETMSX_LIB} ${EPANET_LIB} OpenMP::OpenMP_C)


#target_include_directories(epanetmsx PUBLIC ${PROJECT_SOURCE_DIR}/include)

