#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For seeding rand()
#include <iostream>
#include <stack>
#include <algorithm> 
#include <unordered_map>
#include <queue>
#include <unistd.h>
#include <cmath>

#define GRID_SPACING 10 // Size of each cell (40x40 pixels)
#define WALL_PROBABILITY 0.36 // Probability of a cell being a wall
#define CA_STEPS 5 // Number of Cellular Automata steps
#define DRUNK_WALK_STEPS 10000 // Number of steps for Drunk Walk generation
#define L_SYSTEM_ITERATIONS 4 // Number of iterations for L-System generation
#define L_SYSTEM_STARTPOINTS 7 // Number of starting points for L-System generation
#define WALL 0
#define PATH 1
#define LIGHT 2

struct Cell {
    int row, col;
    Cell(int row, int col) : row(row), col(col) {}
    bool isInsideGrid(int rows, int cols) const {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    }
};

bool isInBounds(int row, int col, int rows, int cols);

class MazeGenerator {
public:
    virtual void generateMaze(std::vector<std::vector<int>>& grid, int rows, int cols) = 0;
};

class CellularAutomataGenerator : public MazeGenerator {
public:
    CellularAutomataGenerator(float wallProbability, int steps) : wallProbability(wallProbability), steps(steps) {}

    void generateMaze(std::vector<std::vector<int>>& grid, int rows, int cols) override;

private:
    float wallProbability;
    int steps;

    int countWallNeighbors(const std::vector<std::vector<int>>& grid, int row, int col);
    std::vector<std::vector<int>> applyCARules(const std::vector<std::vector<int>>& grid);
    void initializeGrid(std::vector<std::vector<int>>& grid);
};

class PrimGenerator : public MazeGenerator {
public:
    void generateMaze(std::vector<std::vector<int>>& grid, int rows, int cols) override;

private:
    void addFrontier(const Cell& cell, const std::vector<std::vector<int>>& grid, std::vector<Cell>& frontier);
};

class LSystemGenerator : public MazeGenerator {
public:
    LSystemGenerator(int iterations, int startpoints) : iterations(iterations), startpoints(startpoints) {
        rules['F'] = {"F+F-F-F+F", "F-F+F+F-F", "F-F-F+F+F"}; // Multiple rules for randomness
        // rules['+'] = {"+", "-"}; // Turn right or left
        // rules['-'] = {"-", "+"}; // Turn left or right

        // Generate the L-system string
        axiom = "F";
    }

    void generateMaze(std::vector<std::vector<int>>& grid, int rows, int cols) override;

private:
    int iterations;
    int startpoints;
    std::unordered_map<char, std::vector<std::string>> rules;
    std::string axiom;

    std::string evolveLSystem() const;
    void interpretLSystem(const std::string& instructions, std::vector<std::vector<int>>& grid, int startRow, int startCol) const;
};

class DrunkWalkGenerator : public MazeGenerator {
public:
    DrunkWalkGenerator(int steps) : steps(steps) {}

    void generateMaze(std::vector<std::vector<int>>& grid, int rows, int cols) override;

private:
    int steps;
};

