cmake_minimum_required(VERSION 3.3.1)

project(Z64Fe
        VERSION 0.1.0
        LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# use CMake's provided functionality for setting the standard we want
# (non-dialect C++14). If ever we decide to switch o'er to C++17 and CMake
# doesn't handle that yet, we'll have to (temporarily?) do this manually.
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 14)

# now for compiler-specific flags. If your compiler isn't listed here, but is
# supported by ${CMAKE_CXX_COMPILER_ID}, then feel free to add its flag
# variables, following the format given of course. Note that variables are
# case-sensitive, so the part identifying the compiler must match the case given
# by ${CMAKE_CXX_COMPILER_ID}.

# also, the reason for the weird double-underscore on the non-compiler-specific
# entries is purely a hack to get those to group together nicely; otherwise,
# you'll inevitably have them weirdly spread out between the compiler-specific
# variables. Think of the double-underscore as really holding an "empty"
# compiler name, which means compiler-independent.
set(Z64FE_CXX_FLAGS__            "-Wall"             CACHE STRING "Compile flags used by Z64Fe for any compiler")
set(Z64FE_CXX_FLAGS_GNU        "-Wextra -pedantic" CACHE STRING "Compile flags used by Z64Fe with g++")
set(Z64FE_CXX_FLAGS_Clang      "-Wextra -pedantic" CACHE STRING "Compile flags used by Z64Fe with non-Apple clang++")
set(Z64FE_CXX_FLAGS_MSVC       ""                  CACHE STRING "Compile flags used by Z64Fe with MSVC")
set(Z64FE_CXX_FLAGS_AppleClang "-Wextra -pedantic" CACHE STRING "Compile flags used by Z64FE with Apple's version of clang++ (e.g. from XCode)")

set(Z64FE_CXX_FLAGS__DEBUG            "" CACHE STRING "Compile flags used by Z64Fe for any compiler, for debug builds")
set(Z64FE_CXX_FLAGS_GNU_DEBUG        "" CACHE STRING "Compile flags used by Z64Fe when compiling a debug build with g++")
set(Z64FE_CXX_FLAGS_Clang_DEBUG      "" CACHE STRING "Compile flags used by Z64Fe when compiling a debug build with non-Apple clang++")
set(Z64FE_CXX_FLAGS_MSVC_DEBUG       "" CACHE STRING "Compile flags used by Z64Fe when compiling a debug build with MSVC")
set(Z64FE_CXX_FLAGS_AppleClang_DEBUG "" CACHE STRING "Compile flags used by Z64Fe when compiling a debug build with Apple's version of clang++")

set(Z64FE_CXX_FLAGS__MINSIZEREL            "" CACHE STRING "Compile flags used by Z64Fe for any compiler, for minimum-size builds")
set(Z64FE_CXX_FLAGS_GNU_MINSIZEREL        "" CACHE STRING "Compile flags used by Z64Fe when compiling a minimum-size build with g++")
set(Z64FE_CXX_FLAGS_Clang_MINSIZEREL      "" CACHE STRING "Compile flags used by Z64Fe when compiling a minimum-size build with non-Apple clang++")
set(Z64FE_CXX_FLAGS_MSVC_MINSIZEREL       "" CACHE STRING "Compile flags used by Z64Fe when compiling a minimum-size build with MSVC")
set(Z64FE_CXX_FLAGS_AppleClang_MINSIZEREL "" CACHE STRING "Compile flags used by Z64Fe when compiling a minimum-size build with Apple's version of clang++")

set(Z64FE_CXX_FLAGS__RELEASE            "" CACHE STRING "Compile flags used by Z64Fe for any compiler, for release builds")
set(Z64FE_CXX_FLAGS_GNU_RELEASE        "" CACHE STRING "Compile flags used by Z64Fe when compiling a release build with g++")
set(Z64FE_CXX_FLAGS_Clang_RELEASE      "" CACHE STRING "Compile flags used by Z64Fe when compiling a release build with non-Apple clang++")
set(Z64FE_CXX_FLAGS_MSVC_RELEASE       "" CACHE STRING "Compile flags used by Z64Fe when compiling a release build with MSVC")
set(Z64FE_CXX_FLAGS_AppleClang_RELEASE "" CACHE STRING "Compile flags used by Z64Fe when compiling a release build with Apple's version of clang++")

set(Z64FE_CXX_FLAGS__RELWITHDEBINFO            "" CACHE STRING "Compile flags used by Z64Fe for any compiler, for release builds with debug info")
set(Z64FE_CXX_FLAGS_GNU_RELWITHDEBINFO        "" CACHE STRING "Compile flags used by Z64Fe when compiling a release build (with debug info) with g++")
set(Z64FE_CXX_FLAGS_Clang_RELWITHDEBINFO      "" CACHE STRING "Compile flags used by Z64Fe when compiling a release build (with debug info) with non-Apple clang++")
set(Z64FE_CXX_FLAGS_MSVC_RELWITHDEBINFO       "" CACHE STRING "Compile flags used by Z64Fe when compiling a release build (with debug info) with MSVC")
set(Z64FE_CXX_FLAGS_AppleClang_RELWITHDEBINFO "" CACHE STRING "Compile flags used by Z64Fe when compiling a release build (with debug info) with Apple's version of clang++")

# mark all of the above as advanced, just like their CMAKE_CXX "cousins".
mark_as_advanced(FORCE Z64FE_CXX_FLAGS__
                       Z64FE_CXX_FLAGS_GNU
                       Z64FE_CXX_FLAGS_Clang
                       Z64FE_CXX_FLAGS_MSVC
                       Z64FE_CXX_FLAGS_AppleClang

                       Z64FE_CXX_FLAGS__DEBUG
                       Z64FE_CXX_FLAGS_GNU_DEBUG
                       Z64FE_CXX_FLAGS_Clang_DEBUG
                       Z64FE_CXX_FLAGS_MSVC_DEBUG
                       Z64FE_CXX_FLAGS_AppleClang_DEBUG

                       Z64FE_CXX_FLAGS__MINSIZEREL
                       Z64FE_CXX_FLAGS_GNU_MINSIZEREL
                       Z64FE_CXX_FLAGS_Clang_MINSIZEREL
                       Z64FE_CXX_FLAGS_MSVC_MINSIZEREL
                       Z64FE_CXX_FLAGS_AppleClang_MINSIZEREL

                       Z64FE_CXX_FLAGS__RELEASE
                       Z64FE_CXX_FLAGS_GNU_RELEASE
                       Z64FE_CXX_FLAGS_Clang_RELEASE
                       Z64FE_CXX_FLAGS_MSVC_RELEASE
                       Z64FE_CXX_FLAGS_AppleClang_RELEASE

                       Z64FE_CXX_FLAGS__RELWITHDEBINFO
                       Z64FE_CXX_FLAGS_GNU_RELWITHDEBINFO
                       Z64FE_CXX_FLAGS_Clang_RELWITHDEBINFO
                       Z64FE_CXX_FLAGS_MSVC_RELWITHDEBINFO
                       Z64FE_CXX_FLAGS_AppleClang_RELWITHDEBINFO)

# this will take those compiler-specific flags above and set them up as needed
set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} ${Z64FE_CXX_FLAGS__} ${Z64FE_CXX_FLAGS_${CMAKE_CXX_COMPILER_ID}}")
set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} ${Z64FE_CXX_FLAGS__DEBUG} ${Z64FE_CXX_FLAGS_${CMAKE_CXX_COMPILER_ID}_DEBUG}")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} ${Z64FE_CXX_FLAGS__MINSIZEREL} ${Z64FE_CXX_FLAGS_${CMAKE_CXX_COMPILER_ID}_MINSIZEREL}")
set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} ${Z64FE_CXX_FLAGS__RELEASE} ${Z64FE_CXX_FLAGS_${CMAKE_CXX_COMPILER_ID}_RELEASE}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${Z64FE_CXX_FLAGS__RELWITHDEBINFO} ${Z64FE_CXX_FLAGS_${CMAKE_CXX_COMPILER_ID}_RELWITHDEBINFO}")

# now let's find Qt5 and other needed packages!
find_package(Qt5Widgets)
find_package(Qt5Concurrent)

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Build Doxygen-generated API documentation" ${DOXYGEN_FOUND})

# block for handling Doxygen stuff
if(BUILD_DOCUMENTATION)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen not found on your system, which is required for building API documentation.")
  endif()

  set(doxy_in  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(doxy_out ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${doxy_in} ${doxy_out} @ONLY)

  add_custom_target(doc
                    COMMAND ${DOXYGEN_EXECUTABLE} ${doxy_out}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API docs with Doxygen"
                    VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()
# end of block for handling Doxygen

# set up our include directory business
include_directories(include)

configure_file("${PROJECT_SOURCE_DIR}/include/projectinfo.hpp.in"
               "${PROJECT_BINARY_DIR}/include/projectinfo.hpp")

include_directories("${PROJECT_BINARY_DIR}/include")

# this includes the subdirectory wherein we compile stuff
add_subdirectory(src)