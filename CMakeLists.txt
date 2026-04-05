cmake_minimum_required(VERSION 4.3.0)

include(CppCore/CppCore.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ..." FORCE)
endif()

if(NOT "${USE_ALTERNATE_COMPILER}" STREQUAL "")
  set_alternate_compiler("${USE_ALTERNATE_COMPILER}")
endif()

if(NOT "${USE_ALTERNATE_LINKER}" STREQUAL "")
  set_alternate_linker("${USE_ALTERNATE_LINKER}")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CppCoreFlags}")

project(Cidmol LANGUAGES CXX ASM)

add_subdirectory(CppCore)
add_subdirectory(src)