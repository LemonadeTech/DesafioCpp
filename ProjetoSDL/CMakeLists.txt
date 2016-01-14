set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/tinyobjloader/tiny_obj_loader.cc
)

include_directories(${PROJECT_SOURCE_DIR}/mathfu/include)
include_directories(${PROJECT_SOURCE_DIR}/vectorial/include)
include_directories(${PROJECT_SOURCE_DIR}/tinyobjloader)


set(EXECUTABLE_NAME "ProjetoSDL")
add_executable(${EXECUTABLE_NAME} ${SOURCES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})


include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
if(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} ${SDL2_LIBRARIES})
endif(SDL2_FOUND)

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
    target_link_libraries(${EXECUTABLE_NAME} m)
endif(OPENGL_FOUND)

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} ${GLEW_LIBRARIES})
endif(GLEW_FOUND)
