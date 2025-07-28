cmake_minimum_required(VERSION 3.28)
project(Island CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_compile_definitions(NOMINMAX)
endif()

find_package(OpenGL REQUIRED)

add_executable(Island
        main.cpp
        Island.cpp
        Skybox.cpp
        Camera.cpp
        Windmill.cpp
)

target_include_directories(Island PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        /usr/include/stb
)

target_link_libraries(Island PRIVATE
        OpenGL::GL
        glfw
        GLEW
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})