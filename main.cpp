#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <filesystem>
#include <iomanip>

namespace fs = std::filesystem;

class KakuroPuzzle {
private:
    int width, height;
    std::vector<std::vector<std::string>> grid;
    std::vector<std::vector<std::string>> solutionGrid;

    void addBordersAndWalls() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, height - 2);
        std::uniform_int_distribution<> dis2(1, width - 2);

        // Add borders
        for (int i = 0; i < height; i++) {
            grid[i][0] = "#";              // Left border
            grid[i][width - 1] = "#";      // Right border
        }
        for (int j = 0; j < width; j++) {
            grid[0][j] = "#";              // Top border
            grid[height - 1][j] = "#";     // Bottom border
        }

        // Add random walls inside the grid
        int totalCells = (height - 2) * (width - 2);
        int wallCount = totalCells * 0.2;  // Adjust wall density as needed
        for (int i = 0; i < wallCount; i++) {
            int x = dis(gen);
            int y = dis2(gen);
            if (grid[x][y] == "_") {
                grid[x][y] = "#";
            }
        }
    }

    void generateSolution() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 9);

        for (int i = 1; i < height - 1; i++) {
            for (int j = 1; j < width - 1; j++) {
                if (grid[i][j] == "_") {
                    int num = dis(gen);
                    grid[i][j] = std::to_string(num);
                    solutionGrid[i][j] = std::to_string(num);
                }
            }
        }
    }

    void placeClues() {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (grid[i][j] == "#") {
                    int horizontalSum = getSum(i, j + 1, "h");
                    int verticalSum = getSum(i + 1, j, "v");

                    if (horizontalSum > 0 && verticalSum > 0) {
                        grid[i][j] = "(" + std::to_string(horizontalSum) + "-" + std::to_string(verticalSum) + ")";
                    }
                    else if (horizontalSum > 0) {
                        grid[i][j] = "(" + std::to_string(horizontalSum) + ")";
                    }
                    else if (verticalSum > 0) {
                        grid[i][j] = "(" + std::to_string(verticalSum) + ")";
                    }
                }
            }
        }

        // Copy clues into the solution grid
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (grid[i][j] != "_" && grid[i][j] != "#" && grid[i][j][0] == '(') {
                    solutionGrid[i][j] = grid[i][j];
                }
            }
        }
    }

    int getSum(int x, int y, const std::string& direction) {
        int total = 0;
        if (direction == "h") {  // Horizontal sum
            while (y < width && grid[x][y] != "#") {
                if (grid[x][y] != "_") {
                    try {
                        total += std::stoi(grid[x][y]);
                    }
                    catch (...) {
                        break;  // Ignore non-integer strings
                    }
                }
                y++;
            }
        }
        else if (direction == "v") {  // Vertical sum
            while (x < height && grid[x][y] != "#") {
                if (grid[x][y] != "_") {
                    try {
                        total += std::stoi(grid[x][y]);
                    }
                    catch (...) {
                        break;  // Ignore non-integer strings
                    }
                }
                x++;
            }
        }
        return total;
    }

    void initializeGrid() {
        grid = std::vector<std::vector<std::string>>(height, std::vector<std::string>(width, "_"));
        solutionGrid = grid;
    }

    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        // Write "Unsolved Kakuro Puzzle"
        file << "Unsolved Kakuro Puzzle:\n";
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (grid[i][j] == "#" || (grid[i][j][0] == '(' && grid[i][j][grid[i][j].size() - 1] == ')')) {
                    file << std::setw(6) << grid[i][j];
                }
                else {
                    file << std::setw(6) << "_";
                }
            }
            file << "\n";
        }

        file << "\nSolved Kakuro Puzzle:\n";
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                file << std::setw(6) << (cell == "_" ? "_" : cell);
            }
            file << "\n";
        }

        file.close();
    }

public:
    KakuroPuzzle(int w, int h) : width(w), height(h) {
        initializeGrid();
        addBordersAndWalls();
        generateSolution();
        placeClues();
    }

    void generateFile(const std::string& outputFolder) {
        if (!fs::exists(outputFolder)) {
            fs::create_directory(outputFolder);
        }

        std::string filename = outputFolder + "/kakuro_" + std::to_string(width) + "x" + std::to_string(height) + ".txt";
        saveToFile(filename);
    }
};

int main() {
    std::string outputFolder = "output";

    for (int size = 5; size <= 20; size++) {
        try {
            KakuroPuzzle puzzle(size, size);
            puzzle.generateFile(outputFolder);
        }
        catch (const std::exception& e) {
            std::cerr << "Error generating puzzle of size " << size << "x" << size << ": " << e.what() << std::endl;
        }
    }

    std::cout << "Kakuro puzzles have been generated in the '" << outputFolder << "' folder." << std::endl;
    return 0;
}
