#ifndef BLOCK_H
#define BLOCK_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./Shader.h"


class Block{
    private:
        glm::vec3 positionVector3D;
        glm::vec3 colorVector3D;
        glm::vec3 position;
        glm::vec3 color;
        GLuint sharedVAO, sharedVBO, sharedEBO;

        std::vector<float> blockVertices = {
            // Posiciones de los vértices
            0.0f, 0.0f, 0.0f,  // 0: Bottom-left-back
            1.0f, 0.0f, 0.0f,  // 1: Bottom-right-back
            1.0f,  1.0f, 0.0f,  // 2: Top-right-back
            0.0f,  1.0f, 0.0f,  // 3: Top-left-back
            0.0f, 0.0f,  1.0f,  // 4: Bottom-left-front
            1.0f, 0.0f,  1.0f,  // 5: Bottom-right-front
            1.0f,  1.0f,  1.0f,  // 6: Top-right-front
            0.0f,  1.0f,  1.0f   // 7: Top-left-front
        };

        std::vector<unsigned int> blockIndices = {
            // Back face
            0, 1, 2,
            2, 3, 0,
            // Front face
            4, 5, 6,
            6, 7, 4,
            // Left face
            0, 4, 7,
            7, 3, 0,
            // Right face
            1, 5, 6,
            6, 2, 1,
            // Bottom face
            0, 1, 5,
            5, 4, 0,
            // Top face
            3, 2, 6,
            6, 7, 3
        };

        void initializeSharedResources() {
            if (sharedVAO == 0) { // Solo inicializar una vez
                glGenVertexArrays(1, &sharedVAO);
                glGenBuffers(1, &sharedVBO);
                glGenBuffers(1, &sharedEBO);

                glBindVertexArray(sharedVAO);

                glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
                glBufferData(GL_ARRAY_BUFFER, blockVertices.size() * sizeof(float), blockVertices.data(), GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, blockIndices.size() * sizeof(unsigned int), blockIndices.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }

    public:
        Block(const glm::vec3& positionVector3D, const glm::vec3& colorVector3D): positionVector3D(positionVector3D), colorVector3D(colorVector3D), sharedVAO(0), sharedVBO(0), sharedEBO(0) {
            position = glm::vec3(positionVector3D.x, positionVector3D.y, positionVector3D.z);
            color = glm::vec3(colorVector3D.x, colorVector3D.y, colorVector3D.z);

            initializeSharedResources();
        }

        glm::vec3 getColor() const {
            return color;
        }

        void draw(const Shader& shader) const {
            shader.setUniform3f("blockColor", color.x, color.y, color.z);
            shader.setUniform1i("isGRID", false);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            shader.setUniformMatrix4fv("model", model);
            glBindVertexArray(sharedVAO);
            glDrawElements(GL_TRIANGLES, blockIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        void setPosition(glm::vec3 newPosition){
            position = newPosition;
        }

        glm::vec3 getPosition() const{
            return position;
        }

        void cleanUp() {
            glDeleteVertexArrays(1, &sharedVAO);
            glDeleteBuffers(1, &sharedVBO);
            glDeleteBuffers(1, &sharedEBO);
        }
};

#endif