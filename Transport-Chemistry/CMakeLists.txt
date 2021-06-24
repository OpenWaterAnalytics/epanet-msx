# CMakeLists.txt - CMake configuration file for EPANETMSX

cmake_minimum_required(VERSION 3.12)
project(TRANSPORT_LIB LANGUAGES C)

#include_directories(
#  ../Legacy\ DLL/include
#)

# Append local dir to module search path
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Add building the command line executable from the run directory
# add_subdirectory(run)

# Sets the output directory for executables and libraries.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

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
file(GLOB TRANSPORT_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} src/*.c)

include(FindOpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(OpenMP_C_LIB_NAMES "libomp")
else(OPENMP_FOUND)
  message("ERROR: OpenMP could not be found.")
endif(OPENMP_FOUND)

#find_library(MSX_LIB epanetmsx NAMES epanetmsx PATHS ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/build/lib/* ${PROJECT_SOURCE_DIR}/../Legacy\ DLL/build/lib)

add_library(transport SHARED ${TRANSPORT_SOURCES})
add_library(transport_lib STATIC ${TRANSPORT_SOURCES})
target_link_libraries(transport ${TRANSPORT_LIB} OpenMP::OpenMP_C)

target_include_directories(transport PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(transport_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
#target_include_directories(transport PUBLIC ${PROJECT_SOURCE_DIR}/..Legacy\ DLL/include)
