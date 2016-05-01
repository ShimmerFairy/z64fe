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

# now for compiler-specific flags (feel free to add a compiler here if your
# compiler isn't here. Note that your compiler must be supported by
# CMAKE_CXX_COMPILER_ID, and this new variable must be of the format
# Z64FE_CXX_${your-compiler's-id}_FLAGS)
set(Z64FE_CXX_FLAGS "-Wall" CACHE STRING "Compile flags used by Z64Fe for any compiler")
set(Z64FE_CXX_GNU_FLAGS "-Wextra -pedantic" CACHE STRING "Compile flags used by Z64Fe with g++")
set(Z64FE_CXX_Clang_FLAGS "-Wextra -pedantic" CACHE STRING "Compile flags used by Z64Fe with clang++")
set(Z64FE_CXX_MSVC_FLAGS "" CACHE STRING "Compile flags used by Z64Fe with MSVC")
set(Z64FE_CXX_AppleClang_FLAGS "-Wextra -pedantic" CACHE STRING "Compile flags used by Z64FE with Apple's version of clang++ (e.g. from XCode)") # XXX verify it works

# this will take those compiler-specific flags above and set them up as needed
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Z64FE_CXX_FLAGS} ${Z64FE_CXX_${CMAKE_CXX_COMPILER_ID}_FLAGS}")

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