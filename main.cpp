#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Callback para redimensionar la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);  // Ajusta el tamaño del viewport cuando la ventana cambia de tamaño
}



// Shader simple para el color de las líneas (vértices)
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos; // Definir el atributo de posición del vértice

uniform mat4 projection;  // Matriz de proyección
uniform mat4 view;        // Matriz de vista

out vec3 FragPos; // Pasar la posición al fragment shader

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);  // Calcular la posición del vértice en espacio recortado
    FragPos = aPos;  // Pasar la posición del vértice al fragment shader
}
)";

// Shader de fragmento para el color de las líneas con opacidad dependiente de la altura
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;  // Color del fragmento (píxel)

in vec3 FragPos;  // Posición del fragmento (vértice)

void main() {
    float alpha = 1.0 - abs(FragPos.y) / 20.0;  // Calcula la transparencia en función de la altura (y)
    FragColor = vec4(0.5, 0.5, 0.5, alpha); // Color gris con transparencia variable
}
)";

// Compilar shaders y crear un programa de shaders
GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    // Crear y compilar el shader de vértices
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);

    // Crear y compilar el shader de fragmentos
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);

    // Crear el programa de shaders y adjuntar los shaders compilados
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);  // Enlazar los shaders para crear el programa

    // Eliminar los shaders individuales ya que ahora están enlazados al programa
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;  // Devolver el programa de shaders compilado
}

// Generar datos para la grilla 3D
std::vector<float> generateGridVertices(int width, int height, int depth) {
    std::vector<float> vertices;

    // // Líneas paralelas al eje Z
    // for (int y = 0; y <= height; ++y) {
    //     for (int x = 0; x <= width; ++x) {
    //         vertices.push_back(x);
    //         vertices.push_back(y);
    //         vertices.push_back(0);  // Primer vértice en la base (Z=0)

    //         vertices.push_back(x);
    //         vertices.push_back(y);
    //         vertices.push_back(depth);  // Segundo vértice en la profundidad
    //     }
    // }

    // // Líneas paralelas al eje X
    // for (int y = 0; y <= height; ++y) {
    //     for (int z = 0; z <= depth; ++z) {
    //         vertices.push_back(0);  // Primer vértice en el eje X
    //         vertices.push_back(y);
    //         vertices.push_back(z);  // En el eje Z

    //         vertices.push_back(width);  // Segundo vértice en el eje X
    //         vertices.push_back(y);
    //         vertices.push_back(z);
    //     }
    // }

    // // Líneas paralelas al eje Y
    // for (int x = 0; x <= width; ++x) {
    //     for (int z = 0; z <= depth; z = z + depth/2) {
    //         vertices.push_back(x);  // Primer vértice en el eje X
    //         vertices.push_back(0);  // En el eje Y (cerca de la base)
    //         vertices.push_back(z);

    //         vertices.push_back(x);  // Segundo vértice en el eje X
    //         vertices.push_back(height);  // En el eje Y (arriba)
    //         vertices.push_back(z);
    //     }
    // }
    
    
    
    // // Horizontales pared derecha
    // // Líneas paralelas al eje X
    for (int y = 0; y <= height; ++y) {
            vertices.push_back(0);  // Primer vértice en el eje X
            vertices.push_back(y);
            vertices.push_back(0);  // En el eje Z

            vertices.push_back(width);  // Segundo vértice en el eje X
            vertices.push_back(y);
            vertices.push_back(0);

            vertices.push_back(0);
            vertices.push_back(y);
            vertices.push_back(0);  // Primer vértice en la base (Z=0)

            vertices.push_back(0);
            vertices.push_back(y);
            vertices.push_back(depth); 
    }
    //VErticales pared izquierda
    // Líneas paralelas al eje Y
    for (int z = 0; z <= depth; ++z) {
        vertices.push_back(0);  // Primer vértice en el eje X
        vertices.push_back(0);  // En el eje Y (cerca de la base)
        vertices.push_back(z);

        vertices.push_back(0);  // Segundo vértice en el eje X
        vertices.push_back(height);  // En el eje Y (arriba)
        vertices.push_back(z);

        vertices.push_back(0);  // Primer vértice en el eje X
        vertices.push_back(0);  // En el eje Y (cerca de la base)
        vertices.push_back(z);

        vertices.push_back(width);  // Segundo vértice en el eje X
        vertices.push_back(0);  // En el eje Y (arriba)
        vertices.push_back(z);
    }

    for (int x = 0; x <= width; ++x) {
        vertices.push_back(x);  // Primer vértice en el eje X
        vertices.push_back(0);  // En el eje Y (cerca de la base)
        vertices.push_back(0);

        vertices.push_back(x);  // Segundo vértice en el eje X
        vertices.push_back(height);  // En el eje Y (arriba)
        vertices.push_back(0);

        vertices.push_back(x);  // Primer vértice en el eje X
        vertices.push_back(0);  // En el eje Y (cerca de la base)
        vertices.push_back(0);

        vertices.push_back(x);  // Segundo vértice en el eje X
        vertices.push_back(0);  // En el eje Y (arriba)
        vertices.push_back(depth);
    }

    return vertices;  // Retornar los vértices generados
}

std::vector<float> generateCubeVertices(float x, float y, float z, float size) {
    std::vector<float> vertices = {
        // Cara frontal
        x, y, z,
        x + size, y, z,
        x + size, y + size, z,
        x, y + size, z,
        // Cara trasera
        x, y, z - size,
        x + size, y, z - size,
        x + size, y + size, z - size,
        x, y + size, z - size,
        // Cara izquierda
        x, y, z - size,
        x, y, z,
        x, y + size, z,
        x, y + size, z - size,
        // Cara derecha
        x + size, y, z - size,
        x + size, y, z,
        x + size, y + size, z,
        x + size, y + size, z - size,
        // Cara superior
        x, y + size, z,
        x + size, y + size, z,
        x + size, y + size, z - size,
        x, y + size, z - size,
        // Cara inferior
        x, y, z,
        x + size, y, z,
        x + size, y, z - size,
        x, y, z - size
    };
    return vertices;
}
void processInput(GLFWwindow* window, glm::vec3& cubePosition) {
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cubePosition.x -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cubePosition.x += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cubePosition.y += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cubePosition.y -= 0.1f;
}



int main() {
    // Inicializa GLFW
    if (!glfwInit()) {
        std::cerr << "Error: No se pudo inicializar GLFW" << std::endl;
        return -1;
    }

    // Configuración de GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // Usar OpenGL versión 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Core profile (sin funciones obsoletas)

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(1600, 1200, "Grilla 3D - OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error: No se pudo crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);  // Hacer el contexto de OpenGL actual para esta ventana

    // Inicializa GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error: No se pudo inicializar GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Configurar viewport y callback
    int viewportWidth, viewportHeight;
    glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);  // Obtener el tamaño del framebuffer
    glViewport(0, 0, viewportWidth, viewportHeight);  // Configurar el tamaño del viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  // Registrar el callback de redimensionado

    // Configuración de OpenGL
    glEnable(GL_DEPTH_TEST);  // Habilitar el test de profundidad (para renderizado 3D)
    glEnable(GL_BLEND);  // Habilitar el blending para permitir la transparencia
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Configurar el tipo de blending

    // Crear programa de shaders
    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Generar datos de la grilla (en este caso, 10x20x10)
    std::vector<float> gridVertices = generateGridVertices(6, 15, 6);
    
    glm::vec3 cubePosition(0.0f, 0.0f, 1.0f);
    float cubeSize = 1.0f;
    std::vector<float> cubeVertices = generateCubeVertices(cubePosition.x, cubePosition.y, cubePosition.z, cubeSize);

    GLuint VAO, VBO;

    
    glGenVertexArrays(1, &VAO);  // Generar el Vertex Array Object
    glGenBuffers(1, &VBO);  // Generar el Vertex Buffer Object

    glBindVertexArray(VAO);  // Enlazar el VAO

    glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Enlazar el VBO
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);  // Cargar datos al VBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);  // Especificar cómo leer los datos del VBO
    glEnableVertexAttribArray(0);  // Habilitar el atributo de vértices (posición)

    glBindBuffer(GL_ARRAY_BUFFER, 0);  // Desenlazar el VBO
    glBindVertexArray(0);  // Desenlazar el VAO

    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Matrices de proyección y vista
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / viewportHeight, 0.1f, 100.0f);  // Configurar proyección 3D (perspectiva)
    glm::mat4 view = glm::lookAt(glm::vec3(15, 25, 15), glm::vec3(5, 10, 5), glm::vec3(0, 1, 0));  // Configurar vista (cámara)

    // Ciclo principal de renderizado
    while (!glfwWindowShouldClose(window)) {
        processInput(window, cubePosition);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Limpiar el buffer de color y el buffer de profundidad

        glUseProgram(shaderProgram);  // Usar el programa de shaders

        glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePosition);

        // Enviar las matrices de proyección y vista al shader
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");

        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);  // Enlazar el VAO
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);  // Dibujar la grilla (como líneas)
        glBindVertexArray(0);  // Desenlazar el VAO

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size() / 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);  // Intercambiar los buffers para renderizar
        glfwPollEvents();  // Procesar eventos
    }

    // Limpiar recursos de OpenGL
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);  // Destruir la ventana
    glfwTerminate();  // Terminar GLFW
    return 0;
}
