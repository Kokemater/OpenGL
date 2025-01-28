#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

// Vertex shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// Fragment shader
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

// Función para multiplicar matrices 4x4
void multiplyMatrices(const float* a, const float* b, float* result) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result[row * 4 + col] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result[row * 4 + col] += a[row * 4 + k] * b[k * 4 + col];
            }
        }
    }
}

// Función para generar una matriz de proyección perspectiva
void perspective(float fov, float aspect, float near, float far, float* matrix) {
    float tanHalfFov = tanf(fov / 2.0f);
    for (int i = 0; i < 16; i++) matrix[i] = 0.0f;
    matrix[0] = 1.0f / (aspect * tanHalfFov);
    matrix[5] = 1.0f / tanHalfFov;
    matrix[10] = -(far + near) / (far - near);
    matrix[11] = -1.0f;
    matrix[14] = -(2.0f * far * near) / (far - near);
}

// Función para generar una matriz de vista
void lookAt(const float* eye, const float* center, const float* up, float* matrix) {
    float forward[3], side[3], upVector[3];
    for (int i = 0; i < 3; i++) {
        forward[i] = center[i] - eye[i];
    }
    float forwardLength = sqrt(forward[0] * forward[0] + forward[1] * forward[1] + forward[2] * forward[2]);
    for (int i = 0; i < 3; i++) forward[i] /= forwardLength;

    side[0] = forward[1] * up[2] - forward[2] * up[1];
    side[1] = forward[2] * up[0] - forward[0] * up[2];
    side[2] = forward[0] * up[1] - forward[1] * up[0];
    float sideLength = sqrt(side[0] * side[0] + side[1] * side[1] + side[2] * side[2]);
    for (int i = 0; i < 3; i++) side[i] /= sideLength;

    upVector[0] = side[1] * forward[2] - side[2] * forward[1];
    upVector[1] = side[2] * forward[0] - side[0] * forward[2];
    upVector[2] = side[0] * forward[1] - side[1] * forward[0];

    for (int i = 0; i < 16; i++) matrix[i] = 0.0f;
    matrix[0] = side[0];
    matrix[1] = upVector[0];
    matrix[2] = -forward[0];
    matrix[4] = side[1];
    matrix[5] = upVector[1];
    matrix[6] = -forward[1];
    matrix[8] = side[2];
    matrix[9] = upVector[2];
    matrix[10] = -forward[2];
    matrix[15] = 1.0f;

    float translation[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -eye[0], -eye[1], -eye[2], 1.0f
    };
    float temp[16];
    multiplyMatrices(matrix, translation, temp);
    for (int i = 0; i < 16; i++) matrix[i] = temp[i];
}

// Función para generar una matriz de transformación (modelo)
void translate(float x, float y, float z, float* matrix) {
    for (int i = 0; i < 16; i++) matrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
}

// Vértices del cubo
float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
};
// Función de inicialización para los shaders
unsigned int compileShader(const char* source, GLenum type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Verificar errores
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Error al compilar el shader: " << infoLog << std::endl;
    }
    return shader;
}

// Función de inicialización de programa
unsigned int createShaderProgram() {
    unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Verificar errores
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Error al enlazar el programa: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Función para generar una matriz de rotación (en torno al eje Y)
void rotate(float angle, float* matrix) {
    float rad = angle * (3.14159f / 180.0f);
    matrix[0] = cos(rad);
    matrix[1] = 0.0f;
    matrix[2] = sin(rad);
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = 1.0f;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;

    matrix[8] = -sin(rad);
    matrix[9] = 0.0f;
    matrix[10] = cos(rad);
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}

int main() {
    // Inicialización de GLFW
    if (!glfwInit()) {
        std::cout << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Cubo en OpenGL", nullptr, nullptr);
    if (!window) {
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error al inicializar GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = createShaderProgram();

    // Configuración de buffers
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Configuración de matrices
    float model[16];
    translate(0.0f, 0.0f, 0.0f, model);

    float view[16];
    float eye[] = {0.0f, 0.0f, 3.0f};
    float center[] = {0.0f, 0.0f, 0.0f};
    float up[] = {0.0f, 1.0f, 0.0f};
    lookAt(eye, center, up, view);

    float projection[16];
    perspective(45.0f * (3.14159f / 180.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);

    float angle = 0.010f; // Ángulo de rotación inicial

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Rotar el cubo
        float rotationMatrix[16];
        rotate(angle, rotationMatrix);
        // Multiplicar la matriz de modelo con la matriz de rotación
        float temp[16];
        multiplyMatrices(model, rotationMatrix, temp);
        for (int i = 0; i < 16; i++) model[i] = temp[i];

        // Enviar matrices a los shaders
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

