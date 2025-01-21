#include "Tetromino.cpp"

class Grid {
private:
    // Dimensions of the grid
    int width, height, depth;

    // 3D matrix to track the state of each cell in the grid (true = occupied, false = empty)
    std::vector<std::vector<std::vector<bool>>> cells;

    // Array to count the number of occupied cells in each horizontal layer (y-level)
    std::vector<int> lineCounters;

public:
    // Constructor: Initializes the grid with the given dimensions and clears all cells
    Grid(int w, int h, int d)
        : width(w), height(h), depth(d),
          cells(w, std::vector<std::vector<bool>>(h, std::vector<bool>(d, false))),
          lineCounters(h, 0) {}

    // Checks if the given Tetromino collides with the boundaries or occupied cells in the grid
    bool checkCollision(const Tetromino& tetromino) const {
        for (const auto& block : tetromino.getBlocks()) {
            Vector3D pos = block.getPosition();
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

    // Places the given Tetromino onto the grid and updates the occupied cells and line counters
    void placeTetromino(const Tetromino& tetromino) {
        for (const auto& block : tetromino.getBlocks()) {
            Vector3D pos = block.getPosition();
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int z = static_cast<int>(pos.z);

            // Mark the cell as occupied and increment the line counter for the respective y-level
            if (!cells[x][y][z]) {
                cells[x][y][z] = true;
                lineCounters[y]++;
            }
        }
    }

    // Clears any fully occupied lines (layers) and shifts the above layers down
    void clearLines() {
        for (int y = 0; y < height; ++y) {
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
            }
        }
    }

    // Draws the grid (implementation depends on the graphics or console setup)
    void draw() const;
};
