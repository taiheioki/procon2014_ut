cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

if (POLICY CMP0043)
cmake_policy(SET CMP0043 OLD)
endif (POLICY CMP0043)

project(procon2014 CXX)


################################################################################
# Forbid in-source build
################################################################################

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()


################################################################################
# Set build types and cxx flags
################################################################################

# show compiler
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")

# build type (default is RelWithDebInfo)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo." FORCE)
endif()

if(NOT(CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo OR CMAKE_BUILD_TYPE STREQUAL Release))
  message(FATAL_ERROR "Choose the type of build, options are: Debug RelWithDebInfo Release.")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# add cxx flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")        # enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")     # enable all warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare") # disable some warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-type-limits")  # disable some warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")     # enable all SSE and AVX

# add some options to CMAKE_CXX_FLAGS_DEBUG
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DTBB_DO_ASSERT=1")

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_LIBCPP_DEBUG=0")
endif()

# override CMAKE_CXX_FLAGS_RELWITHDEBINFO
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g")

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Og")
else()
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2")
endif()

# for Intel C++ Compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL Intel)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -xHOST -ipo -no-prec-div -parallel -fno-alias -fp-model fast=2 -fimf-precision:low -no-prec-sqrt")
endif()

# add definitions
add_definitions(-D_USE_MATH_DEFINES) # enable math defines (M_PI)

option(ENABLE_TIMER "Enable Timer" ON)
if(ENABLE_TIMER)
  add_definitions(-DENABLE_TIMER)    # enable timer output
endif()

option(USE_FAST_BRANCH "Use branch that will be compiled faster but restricted" OFF)
if(USE_FAST_BRANCH)
  message(STATUS "Use FAST branch")
  add_definitions(-DUSE_FAST_BRANCH)
else()
  message(STATUS "Use FULL branch")
endif()

# print cxx flags
string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" CMAKE_CXX_FLAGS_BUILD_TYPE)
set(CMAKE_CXX_FLAGS_BUILD_TYPE "${${CMAKE_CXX_FLAGS_BUILD_TYPE}}")
message(STATUS "CXX flags:${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_BUILD_TYPE}")


################################################################################
# Check libraries
################################################################################

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Add cmake directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Remove 'lib' prefix for shared libraries on Windows
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

# boost
find_package(Boost REQUIRED COMPONENTS date_time filesystem log program_options serialization system thread timer unit_test_framework)
message(STATUS "Boost library dir = ${Boost_LIBRARY_DIR}")
message(STATUS "Boost include dir = ${Boost_INCLUDE_DIR}")

# Qt5Widgets
set(CMAKE_PREFIX_PATH /usr/local/opt/qt5)
find_package(Qt5Widgets)
if(Qt5Widgets_FOUND)
  message(STATUS "Found 'Qt5 library' ${Qt5Widgets_VERSION}")
else()
  message(STATUS "Qt5 library is not found")
endif()

# TBB
find_package(TBB REQUIRED)
message(STATUS "Found 'TBB library' ${TBB_VERSION} ${TBB_LIBRARIES}")

# TC Malloc (gpreftools)
option(USE_TC_MALLOC "Use TC Malloc" ON)

if(USE_TC_MALLOC)
  find_library(TC_Malloc_LIBRARY tcmalloc)
  if(TC_Malloc_LIBRARY STREQUAL TC_Malloc_LIBRARY-NOTFOUND)
    message(STATUS "TC Malloc is not found")
  else()
    message(STATUS "Found TC Malloc: ${TC_Malloc_LIBRARY}")
    link_libraries(${TC_Malloc_LIBRARY})
  endif()
endif()

# cpu profiler (gperftools)
option(USE_CPU_PROFILER "Use CPU profiler" OFF)

if(USE_CPU_PROFILER)
  find_library(CPU_PROFILER_LIBRARY profiler)
  if(CPU_PROFILER_LIBRARY STREQUAL CPU_PROFILER_LIBRARY-NOTFOUND)
    message(STATUS "CPU profiler is not found")
  else()
    message(STATUS "Found CPU profiler: ${CPU_PROFILER_LIBRARY}")
    link_libraries(${CPU_PROFILER_LIBRARY} "-Wl,-pie")
  endif()
endif()


################################################################################
# Add subprojects
################################################################################

# Set output directories
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Output directory for executables")
set(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib CACHE PATH "Output directory for libraries")

# Add external projects
include_directories(external/sparsehash)

# Add modules
add_subdirectory(modules)

# Add executables
add_subdirectory(exe)
