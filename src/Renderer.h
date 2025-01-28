#ifndef RENDERER_H
#define RENDERER_H

#include "Game.h"
#include "Shader.h"
#include "TextShader.h"

class Renderer {
    private:
        Shader blockShader;
        TextShader textShader;
        GLuint cubeVAO = 0, cubeVBO = 0, cubeEBO = 0;
        int cubeIndexCount = 36; // 6 caras * 2 triángulos por cara * 3 vértices por triángulo


        void initializeCubeVAO() {
            if (cubeVAO == 0) {
                // Vértices e índices del cubo
                float vertices[] = {
                    // Posición
                    0.0f, 0.0f, 0.0f, // 0
                    1.0f, 0.0f, 0.0f, // 1
                    1.0f, 1.0f, 0.0f, // 2
                    0.0f, 1.0f, 0.0f, // 3
                    0.0f, 0.0f, 1.0f, // 4
                    1.0f, 0.0f, 1.0f, // 5
                    1.0f, 1.0f, 1.0f, // 6
                    0.0f, 1.0f, 1.0f  // 7
                };

                unsigned int indices[] = {
                    // Cara trasera
                    0, 1, 2, 2, 3, 0,
                    // Cara delantera
                    4, 5, 6, 6, 7, 4,
                    // Cara izquierda
                    0, 3, 7, 7, 4, 0,
                    // Cara derecha
                    1, 2, 6, 6, 5, 1,
                    // Cara inferior
                    0, 1, 5, 5, 4, 0,
                    // Cara superior
                    3, 2, 6, 6, 7, 3
                };

                glGenVertexArrays(1, &cubeVAO);
                glGenBuffers(1, &cubeVBO);
                glGenBuffers(1, &cubeEBO);

                glBindVertexArray(cubeVAO);

                glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }

        void renderBlocksInGrille(const Grid& grid, const glm::mat4& projection, const glm::mat4& view) {
            initializeCubeVAO();
            blockShader.use();
            blockShader.setUniformMatrix4fv("projection", projection);
            blockShader.setUniformMatrix4fv("view", view);

            for (int x = 0; x < grid.getWidth(); ++x) {
                for (int y = 0; y < grid.getHeight(); ++y) {
                    for (int z = 0; z < grid.getDepth(); ++z) {
                        if (grid.isCellOccupied(x, y, z)) {
                            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                            blockShader.setUniformMatrix4fv("model", model);

                            glm::vec3 color = grid.getCellColor(x, y, z); 

                            blockShader.setUniform3f("blockColor", color.x, color.y, color.z);
                            blockShader.setUniform1i("isGRID", false);

                            // Renderizar un cubo en la posición actual
                            glBindVertexArray(cubeVAO); // Asegúrate de usar un VAO compartido o inicializado para cubos
                            glDrawElements(GL_TRIANGLES, cubeIndexCount, GL_UNSIGNED_INT, 0);
                            glBindVertexArray(0);
                        }
                    }
                }
            }
        }

        void renderTetromino(const Tetromino& tetromino, const glm::mat4& projection, const glm::mat4& view) {
            blockShader.use();
            blockShader.setUniformMatrix4fv("projection", projection);
            blockShader.setUniformMatrix4fv("view", view);

            for (const Block& block : tetromino.getBlocks()) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(block.getPosition()));
                blockShader.setUniformMatrix4fv("model", model);
                block.draw(blockShader);
            }
        }

        void renderGrid(const Grid& grid, const glm::mat4& projection, const glm::mat4& view) {
            blockShader.use();
            blockShader.setUniformMatrix4fv("projection", projection);
            blockShader.setUniformMatrix4fv("view", view);

            // Renderizar la grilla aquí
            grid.draw(blockShader);

        }

        void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
            textShader.use();
            textShader.setMat4("projection", glm::ortho(0.0f, 1600.0f, 0.0f, 1200.0f));
            textShader.setVec3("textColor", color);
            textShader.renderText(text, x, y, scale, color);
        }

    public:
        Renderer(): blockShader(), textShader() {}


        void renderGame(const Game& game, const glm::mat4& projection, const glm::mat4& view) {
            // Renderizar la grilla
            renderGrid(game.getGrid(), projection, view);

            renderBlocksInGrille(game.getGrid(), projection, view);

            // Renderizar el Tetromino actual
            renderTetromino(game.getCurrentTetromino(), projection, view);

            // Renderizar el siguiente Tetromino
            renderTetromino(game.getNextTetromino(), projection, view);

            // Renderizar el Tetromino proyectado
            renderTetromino(game.getProjectedTetromino(game.getCurrentTetromino()), projection, view);

            // Renderizar puntaje y nivel
            renderText("Score: " + std::to_string(game.getScore()), 1200.0f, 1100.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            renderText("Level: " + std::to_string(game.getLevel()), 1200.0f, 1000.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
};
#endif