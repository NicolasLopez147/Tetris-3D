#include "Tetromino.cpp"
class Grid {
private:
    int width, height, depth;
    std::vector<std::vector<std::vector<bool>>> cells;
    std::vector<int> lineCounters;
public:
    Grid(int w, int h, int d) : width(w), height(h), depth(d), cells(w, std::vector<std::vector<bool>>(h, std::vector<bool>(d, false))), lineCounters(h, 0) {}

    bool checkCollision(const Tetromino& tetromino) const {
        for (const auto& block : tetromino.getBlocks()) {
            Vector3D pos = block.getPosition();
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int z = static_cast<int>(pos.z);

            // std:: cout << "x: " << pos.x << " y: " << pos.y << " z: " << pos.z << std::endl;
            if (x <= 0 || x >= width || y <= 0 || y >= height || z <= 0 || z >= depth) {
                return true;
            }
            if (cells[x][y][z]) {
                return true;
            }
        }
        // std:: cout << "Hola mundo" << std::endl;
        return false;
    }

    void placeTetromino(const Tetromino& tetromino) {
        for (const auto& block : tetromino.getBlocks()) {
            Vector3D pos = block.getPosition();
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int z = static_cast<int>(pos.z);
            if (!cells[x][y][z]) {
                cells[x][y][z] = true;
                lineCounters[y]++;
            }
        }
    }

    void clearLines() {
        for (int y = 0; y < height; ++y) {
            if (lineCounters[y] == width * depth) {
                for (int z = 0; z < depth; ++z) {
                    for (int x = 0; x < width; ++x) {
                        cells[x][y][z] = false;
                    }
                }
                for (int ny = y; ny < height - 1; ++ny) {
                    for (int z = 0; z < depth; ++z) {
                        for (int x = 0; x < width; ++x) {
                            cells[x][ny][z] = cells[x][ny + 1][z];
                        }
                    }
                    lineCounters[ny] = lineCounters[ny + 1];
                }
                lineCounters[height - 1] = 0;
            }
        }
    }

    void draw() const;
};
