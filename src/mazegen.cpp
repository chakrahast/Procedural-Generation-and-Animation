#include "mazegen.hpp"

bool isInBounds(int row, int col, int rows, int cols) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

void CellularAutomataGenerator::generateMaze(std::vector<std::vector<int>>& grid, int rows, int cols) {
    initializeGrid(grid);

    for (int i = 0; i < steps; i++) {
        grid = applyCARules(grid);
    }
}

void CellularAutomataGenerator::initializeGrid(std::vector<std::vector<int>>& grid) {
    int rows = grid.size();
    int cols = grid[0].size();

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            grid[row][col] = (rand() / static_cast<float>(RAND_MAX)) < wallProbability ? WALL : PATH;
        }
    }

    // for (int i = 0; i < rows; ++i) {
    //     grid[i][0] = WALL;
    //     grid[i][cols - 1] = WALL;
    // }
    // for (int i = 0; i < cols; ++i) {
    //     grid[0][i] = WALL;
    //     grid[rows - 1][i] = WALL;
    // }
}

std::vector<std::vector<int>> CellularAutomataGenerator::applyCARules(const std::vector<std::vector<int>> &grid) {
    std::vector<std::vector<int>> newGrid = grid;

    int rows = grid.size();
    int cols = grid[0].size();

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int wallNeighbors = countWallNeighbors(grid, row, col);

            if (wallNeighbors >= 5) newGrid[row][col] = 1;
            else newGrid[row][col] = 0;
        }
    }
    return newGrid;
}

int CellularAutomataGenerator::countWallNeighbors(const std::vector<std::vector<int>>& grid, int row, int col) {
    int wallCount = 0;
    int rows = grid.size();
    int cols = grid[0].size();

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            int newRow = row + i;
            int newCol = col + j;
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                wallCount += grid[newRow][newCol];
            }
        }
    }
    return wallCount;
}

void PrimGenerator::generateMaze(std::vector<std::vector<int>>& grid, int rows, int cols){
    std::vector<Cell> frontier;

    // Start at a random cell and mark it as a path
    Cell start(1, 1);
    grid[start.row][start.col] = PATH;
    addFrontier(start, grid, frontier);

    // Randomly process frontier cells
    while (!frontier.empty()) {
        int idx = rand() % frontier.size();
        Cell cell = frontier[idx];
        frontier.erase(frontier.begin() + idx);

        // Get neighboring path cells
        std::vector<Cell> neighbors;
        if (isInBounds(cell.row - 2, cell.col, rows, cols) && grid[cell.row - 2][cell.col] == PATH)
            neighbors.push_back({cell.row - 2, cell.col});
        if (isInBounds(cell.row + 2, cell.col, rows, cols) && grid[cell.row + 2][cell.col] == PATH)
            neighbors.push_back({cell.row + 2, cell.col});
        if (isInBounds(cell.row, cell.col - 2, rows, cols) && grid[cell.row][cell.col - 2] == PATH)
            neighbors.push_back({cell.row, cell.col - 2});
        if (isInBounds(cell.row, cell.col + 2, rows, cols) && grid[cell.row][cell.col + 2] == PATH)
            neighbors.push_back({cell.row, cell.col + 2});

        // If there is a neighboring path, carve a passage
        if (!neighbors.empty()) {
            Cell neighbor = neighbors[rand() % neighbors.size()];
            grid[cell.row][cell.col] = PATH;
            grid[(cell.row + neighbor.row) / 2][(cell.col + neighbor.col) / 2] = PATH;
            addFrontier(cell, grid, frontier);
        }
    }
}

void PrimGenerator::addFrontier(const Cell& cell, const std::vector<std::vector<int>>& grid, std::vector<Cell>& frontier) {
    int rows = grid.size();
    int cols = grid[0].size();
    std::vector<Cell> neighbors = {
        {cell.row - 2, cell.col},
        {cell.row + 2, cell.col},
        {cell.row, cell.col - 2},
        {cell.row, cell.col + 2}
    };

    for (const Cell& neighbor : neighbors) {
        if (neighbor.isInsideGrid(rows, cols) && grid[neighbor.row][neighbor.col] == WALL) {
            frontier.push_back(neighbor);
        }
    }
}

void DrunkWalkGenerator::generateMaze(std::vector<std::vector<int>>& grid, int rows, int cols){
    int row = rows - 1;
    int col = 0;

    for (int i = 0; i < steps; ++i) {
        grid[row][col] = PATH;

        int direction = rand() % 4;

        if (direction == 0 && row > 0) row--;           // Up
        else if (direction == 1 && row < rows - 1) row++; // Down
        else if (direction == 2 && col > 0) col--;        // Left
        else if (direction == 3 && col < cols - 1) col++; // Right
    }
}

void LSystemGenerator::generateMaze(std::vector<std::vector<int>>& grid, int rows, int cols){
    for (int i = 0; i < startpoints; i++) {
        std::string instructions = evolveLSystem();
        if (i == 0) {
            interpretLSystem(instructions, grid, rows - 1, 0);
        } else {
            // Choose start randomly
            int row = rand() % rows;
            int col = rand() % cols;
            interpretLSystem(instructions, grid, row, col);
        }
    }
}

std::string LSystemGenerator:: evolveLSystem() const {
    std::string result = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::string next;
        for (char c : result) {
            if (rules.find(c) != rules.end()) {
                const std::vector<std::string>& possibleRules = rules.at(c);
                next += possibleRules[rand() % possibleRules.size()]; // Randomly select a rule
            } else {
                next += c;
            }
        }
        result = next;
    }
    return result;
}

void LSystemGenerator::interpretLSystem(const std::string& instructions, std::vector<std::vector<int>>& grid, int startRow, int startCol) const {
    int rows = grid.size();
    int cols = grid[0].size();
    int direction = 0; // 0=up, 1=right, 2=down, 3=left
    int row = startRow;
    int col = startCol;

    grid[row][col] = PATH;

    for (char command : instructions) {
        if (command == 'F') {
            // Move in the current direction
            if (direction == 0 && row > 0) row--;       // Move up
            else if (direction == 1 && col < cols - 1) col++; // Move right
            else if (direction == 2 && row < rows - 1) row++; // Move down
            else if (direction == 3 && col > 0) col--;       // Move left
            grid[row][col] = PATH;
        } else if (command == '+') {
            direction = (direction + 1) % 4; // Turn right
        } else if (command == '-') {
            direction = (direction + 3) % 4; // Turn left
        }
    }
}
