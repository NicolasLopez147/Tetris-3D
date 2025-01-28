#ifndef GRID_H
#define GRID_H

#include "Tetromino.h"

class Grid{
    private:
        GLuint VAO;
        int vertexCount;
        int width, height, depth;

        std::vector<std::vector<std::vector<bool>>> cells;
        std::vector<std::vector<std::vector<glm::vec3>>> cellColors;


        std::vector<int> lineCounters;

        // Generate vertices for a 3D grid based on width, height, and depth
        std::vector<float> generateGridVertices(int width, int height, int depth) {
            std::vector<float> vertices;

            for (int y = 0; y <= height; ++y) {
                // Líneas horizontales
                vertices.insert(vertices.end(), { 0, (float)y, 0, (float)width, (float)y, 0 });
                vertices.insert(vertices.end(), { 0, (float)y, 0, 0, (float)y, (float)depth });
            }

            for (int z = 0; z <= depth; ++z) {
                // Líneas verticales
                vertices.insert(vertices.end(), { 0, 0, (float)z, 0, (float)height, (float)z });
                vertices.insert(vertices.end(), { 0, 0, (float)z, (float)width, 0, (float)z });
            }

            for (int x = 0; x <= width; ++x) {
                vertices.insert(vertices.end(), { (float)x, 0, 0, (float)x, (float)height, 0 });
                vertices.insert(vertices.end(), { (float)x, 0, 0, (float)x, 0, (float)depth });
            }

            return vertices;
        }


    public:
        Grid(){}
        Grid(int width, int height, int depth): width(width), height(height), depth(depth), cells(width, std::vector<std::vector<bool>>(height, std::vector<bool>(depth, false))), lineCounters(height, 0), cellColors(width, std::vector<std::vector<glm::vec3>>(height, std::vector<glm::vec3>(depth, glm::vec3(0.0f)))) {
            std::vector<float> vertices = generateGridVertices(width, height, depth);
            vertexCount = vertices.size() / 3;

            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            GLuint VBO;
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glm::vec3 getCellColor(int x, int y, int z) const {
            return cellColors[x][y][z];
        }

        // Checks if the given Tetromino collides with the boundaries or occupied cells in the grid
        bool checkCollision(const Tetromino& tetromino) const {
            for (const auto& block : tetromino.getBlocks()) {
                glm::vec3 pos = block.getPosition();
                int x = static_cast<int>(pos.x);
                int y = static_cast<int>(pos.y);
                int z = static_cast<int>(pos.z);

                // Check if the block is outside the grid boundaries
                if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
                    return true;
                }

                // Check if the block is colliding with an occupied cell
                if (cells[x][y][z]) {
                    return true;
                }
            }
            return false;
        }

        void cleanUp() {
            glDeleteVertexArrays(1, &VAO);
        }

        int getWidth() const { return width; }
        int getHeight() const { return height; }
        int getDepth() const { return depth; }

        // Places the given Tetromino onto the grid and updates the occupied cells and line counters
        void placeTetromino(const Tetromino& tetromino) {
            for (const auto& block : tetromino.getBlocks()) {
                glm::vec3 pos = block.getPosition();
                int x = static_cast<int>(pos.x);
                int y = static_cast<int>(pos.y);
                int z = static_cast<int>(pos.z);

                // Mark the cell as occupied and increment the line counter for the respective y-level
                if (!cells[x][y][z]) {
                    cells[x][y][z] = true;
                    cellColors[x][y][z] = block.getColor();
                    lineCounters[y]++;
                }
            }
        }

        // Clears any fully occupied lines (layers) and shifts the above layers down
        int clearLines() {
            int lines = 0;
            int y = 0;
            while(y < height){
                // Check if the layer is fully occupied
                if (lineCounters[y] == width * depth) {
                    // Clear the layer
                    for (int z = 0; z < depth; ++z) {
                        for (int x = 0; x < width; ++x) {
                            cells[x][y][z] = false;
                        }
                    }

                    // Shift the layers above this one down
                    for (int ny = y; ny < height - 1; ++ny) {
                        for (int z = 0; z < depth; ++z) {
                            for (int x = 0; x < width; ++x) {
                                cells[x][ny][z] = cells[x][ny + 1][z];
                            }
                        }
                        // Update the line counter for the shifted layer
                        lineCounters[ny] = lineCounters[ny + 1];
                    }

                    // Clear the topmost layer
                    lineCounters[height - 1] = 0;
                    lines+=1;
                    y--;
                }
                y++;
            }
            return lines;
        }

        void draw(const Shader& shader) const {
            shader.setUniform1i("isGRID", true);
            glm::mat4 identityModel = glm::mat4(1.0f);
            shader.setUniformMatrix4fv("model", identityModel);
            glBindVertexArray(VAO);
            glDrawArrays(GL_LINES, 0, vertexCount);
            glBindVertexArray(0);
        }

        bool isCellOccupied(int x, int y, int z) const {
            return cells[x][y][z];
        }
};
#endif