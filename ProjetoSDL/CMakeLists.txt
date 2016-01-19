set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render.cpp
    ${CMAKE_SOURCE_DIR}/tinyobjloader/tiny_obj_loader.cc
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/render.h
)

include_directories(${PROJECT_SOURCE_DIR}/mathfu/include)
include_directories(${PROJECT_SOURCE_DIR}/vectorial/include)
include_directories(${PROJECT_SOURCE_DIR}/tinyobjloader)


set(EXECUTABLE_NAME "ProjetoSDL")
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})


include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
#TODO
#find_package(SDL2 REQUIRED sdl2)
if(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    if(APPLE)
        # nastyyyy
        set(SDL2_LIBRARIES /Library/Frameworks/SDL2.framework/SDL2)
    endif(APPLE)
    target_link_libraries(${EXECUTABLE_NAME} ${SDL2_LIBRARIES})
endif(SDL2_FOUND)

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
    target_link_libraries(${EXECUTABLE_NAME} m)
endif(OPENGL_FOUND)

if(NOT APPLE)
    find_package(GLEW REQUIRED)
    if (GLEW_FOUND)
        include_directories(${GLEW_INCLUDE_DIRS})
        target_link_libraries(${EXECUTABLE_NAME} ${GLEW_LIBRARIES})
    endif(GLEW_FOUND)
endif(NOT APPLE)


file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/fighter.obj
    ${CMAKE_CURRENT_SOURCE_DIR}/fighter.tga
    ${CMAKE_CURRENT_SOURCE_DIR}/fragment.fsh
    ${CMAKE_CURRENT_SOURCE_DIR}/vertex.vsh
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR})
