#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

// Callback para ajustar el viewport cuando la ventana cambia de tamaño
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Código fuente del shader de vértices
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "}\0";

// Código fuente del shader de fragmentos
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec2 uResolution;\n"
    "uniform vec2 uSphereCenter;\n"
    "uniform float uSphereRadius;\n"
    "uniform vec3 uSphereColor;\n"
    "void main() {\n"
    "   // Coordenadas de píxel normalizadas (-1 a 1 en espacio de NDC)\n"
    "   vec2 uv = (gl_FragCoord.xy / uResolution) * 2.0 - 1.0;\n"
    "   uv.x *= uResolution.x / uResolution.y;\n"
    "\n"
    "   // Origen y dirección del rayo (simple cámara ortográfica)\n"
    "   vec3 rayOrigin = vec3(uv, -1.0);\n"
    "   vec3 rayDir = vec3(0.0, 0.0, 1.0);\n"
    "\n"
    "   // Ecuación de la esfera: (P - C)^2 = R^2\n"
    "   vec3 sphereToRay = rayOrigin - vec3(uSphereCenter, 0.0);\n"
    "   float a = dot(rayDir, rayDir);\n"
    "   float b = 2.0 * dot(rayDir, sphereToRay);\n"
    "   float c = dot(sphereToRay, sphereToRay) - uSphereRadius * uSphereRadius;\n"
    "\n"
    "   // Resolver la ecuación cuadrática\n"
    "   float discriminant = b * b - 4.0 * a * c;\n"
    "   if (discriminant < 0.0) {\n"
    "       FragColor = vec4(0.1, 0.1, 0.1, 1.0); // Fondo oscuro\n"
    "   } else {\n"
    "       float t = (-b - sqrt(discriminant)) / (2.0 * a);\n"
    "       vec3 hitPoint = rayOrigin + t * rayDir;\n"
    "       vec3 normal = normalize(hitPoint - vec3(uSphereCenter, 0.0));\n"
    "\n"
    "       // Iluminación sencilla (Lambert)\n"
    "       vec3 lightDir = normalize(vec3(-0.5, 0.5, -1.0));\n"
    "       float diff = max(dot(normal, lightDir), 0.0);\n"
    "       vec3 color = uSphereColor * diff;\n"
    "       FragColor = vec4(color, 1.0);\n"
    "   }\n"
    "}\0";

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Error al inicializar GLFW\n");
        return -1;
    }

    // Configuración de la ventana GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crear una ventana GLFW
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ray Tracing Sphere", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Error al crear la ventana GLFW\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Cargar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Error al inicializar GLAD\n");
        return -1;
    }

    // Configurar el viewport
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Configurar un quad que cubrirá toda la pantalla
    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Compilar y enlazar shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Variables uniformes
    glUseProgram(shaderProgram);
    int resolutionLoc = glGetUniformLocation(shaderProgram, "uResolution");
    int centerLoc = glGetUniformLocation(shaderProgram, "uSphereCenter");
    int radiusLoc = glGetUniformLocation(shaderProgram, "uSphereRadius");
    int colorLoc = glGetUniformLocation(shaderProgram, "uSphereColor");

    glUniform2f(resolutionLoc, 800.0f, 600.0f);
    glUniform1f(radiusLoc, 0.5f);
    glUniform3f(colorLoc, 1.0f, 0.5f, 0.2f);

    float sphereX = 0.0f;
    float speed = 0.5f;

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        sphereX += speed * 0.01f;
        if (sphereX > 1.0f || sphereX < -1.0f) speed = -speed;

        glUniform2f(centerLoc, sphereX, 0.0f);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    // Liberar recursos
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
