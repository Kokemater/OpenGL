cmake_minimum_required(VERSION 3.10)

project(OpenGLTest)

# Buscar dependencias
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Incluir directorios
include_directories(include)

# Agregar los archivos fuente
add_executable(OpenGLTest test/test.cpp src/glad.c)

# Linkear las bibliotecas necesarias
target_link_libraries(OpenGLTest OpenGL glfw)
