cmake_minimum_required(VERSION 3.3.1)

project(Z64Fe
        VERSION 0.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

include_directories(include)

add_subdirectory(src)