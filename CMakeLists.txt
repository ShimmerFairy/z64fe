cmake_minimum_required(VERSION 3.3.1)

project(Z64Fe
        VERSION 0.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)
find_package(Qt5Concurrent)

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Build Doxygen-generated API documentation" ${DOXYGEN_FOUND})

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

include_directories(include)

configure_file("${PROJECT_SOURCE_DIR}/include/projectinfo.hpp.in"
               "${PROJECT_BINARY_DIR}/include/projectinfo.hpp")

include_directories("${PROJECT_BINARY_DIR}/include")

add_subdirectory(src)