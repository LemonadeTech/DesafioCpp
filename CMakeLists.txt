cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

ADD_DEFINITIONS(
    -Werror
    -Wall
    -std=c++11
)

project(ProjetoSDL)

add_subdirectory(ProjetoSDL)
