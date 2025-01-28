
#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

class Shader{
    private:
        // Vertex shader: Transforms vertex positions and passes fragment position to the fragment shader
        const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;


        uniform mat4 model;      // Model matrix
        uniform mat4 projection; // Projection matrix
        uniform mat4 view;       // View (camera) matrix

        out vec3 FragPos; // Pass the position to the fragment shader
        out float FragHeight; // Pass the height to the fragment shader

        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);  // Transform to clip space
            FragPos = vec3(model * vec4(aPos, 1.0));                   // Transform the position
            FragHeight = aPos.y;                                       // Set the height
        }
        )";

        // Fragment shader: Colors the grid lines with transparency based on their height
        const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;  // Output color of the fragment

        uniform vec3 blockColor; // Color of the Block
        uniform bool isGRID; // Color of the Block

        in vec3 FragPos;  // Position of the fragment
        in float FragHeight; // Height of the fragment

        void main() {
            if(isGRID){
                float alpha = 1.0 - clamp(abs(FragHeight) / 18.0, 0.0, 1.0);  // Calculate transparency based on height
                FragColor = vec4(1.0, 1.0, 1.0, alpha);  // Set the color of the fragment
            }else{
                FragColor = vec4(blockColor,1.0);  // Set the color of the fragment
            }
        }
        )";

        void checkCompileErrors(GLuint shader, std::string type) {
            GLint success;
            GLchar infoLog[1024];
            if (type != "PROGRAM") {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cerr << "[Error] Shader Compilation Error (" << type << "):\n" << infoLog << std::endl;
                }
            } else {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cerr << "[Error] Program Linking Error: " << infoLog << std::endl;
                }
            }
        }
    public:
        GLuint ID;

        Shader(){
            ID = createShaderProgram(vertexShaderSource, fragmentShaderSource);
        }
        GLuint getShaderID(){
            return ID;
        }

        GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
            // Compile the vertex shader
            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
            glCompileShader(vertexShader);
            checkCompileErrors(vertexShader, "VERTEX");

            // Compile the fragment shader
            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
            glCompileShader(fragmentShader);
            checkCompileErrors(fragmentShader, "FRAGMENT");

            // Link the shaders into a shader program
            GLuint shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);
            checkCompileErrors(shaderProgram, "PROGRAM");

            // Clean up the individual shaders as they are no longer needed
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            return shaderProgram;
        }

        void cleanUp() {
            glDeleteProgram(ID);
        }

        void use() {
            glUseProgram(ID);
        }

        void setUniformMatrix4fv(const std::string& name, glm::mat4 matrix) const {
            GLuint loc = glGetUniformLocation(ID, name.c_str());
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
        }

        void setUniform3f(const std::string& name, float x, float y, float z) const {
            GLuint loc = glGetUniformLocation(ID, name.c_str());
            glUniform3f(loc, x, y, z);
        }

        void setUniform1i(const std::string& name, int value) const {
            GLuint loc = glGetUniformLocation(ID, name.c_str());
            glUniform1i(loc, value);
        }
    
        ~Shader(){
            cleanUp();
        }
};

#endif