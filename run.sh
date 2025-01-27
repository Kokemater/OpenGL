#!/bin/bash

# Salir si ocurre algún error
set -e

# Directorio de construcción
BUILD_DIR="./build"

# Verificar si la carpeta build existe, si no, crearla
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creando directorio $BUILD_DIR..."
    mkdir -p "$BUILD_DIR"
fi

# Entrar al directorio build
cd "$BUILD_DIR"

# Configurar el proyecto con CMake usando Ninja
echo "Configurando el proyecto con CMake y Ninja..."
cmake .. -G "Ninja"

# Compilar el proyecto
echo "Compilando el proyecto..."
ninja

echo "Compilación exitosa. El binario está en $BUILD_DIR."

echo "Ejecutando el binario..."
./OpenGLTest
