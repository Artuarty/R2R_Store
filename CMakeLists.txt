cmake_minimum_required(VERSION 3.16)
project(R2RStore CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)

add_executable(R2RStore src/main.cpp)

target_include_directories(R2RStore PRIVATE src examples)

target_link_libraries(R2RStore PRIVATE
    OpenGL::GL
    glfw
    GLEW::GLEW
    assimp
)

# Asegura que el ejecutable quede en build/
set_target_properties(R2RStore PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
