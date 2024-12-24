#include <SFML/Graphics.hpp>
// #include "mazegen.hpp"
#include <vector>
#include <cmath>

// Constants for the octagon
#define HEXAGON_DISTANCE 90.0f // Distance of circles from player
#define CIRCLE_RADIUS 2.0f     // Radius of each circle
#define GRID_SPACING 10 // Size of each cell in the grid
#define WALL 0
#define PATH 1

struct Limb {
    sf::Vector2f start;   // Starting position (player position)
    sf::Vector2f end;     // Current endpoint (animated)
    sf::Vector2f target;  // Final endpoint (wall position)
    float progress;       // Progress of the animation (0 to 1)
    bool active;          // Whether the limb should animate

    Limb(const sf::Vector2f& start, const sf::Vector2f& target)
        : start(start), end(start), target(target), progress(0.0f), active(true) {
        // Calculate the distance from start to target
        float distance = std::sqrt(std::pow(target.x - start.x, 2) + std::pow(target.y - start.y, 2));
        if (distance > HEXAGON_DISTANCE) {
            active = false; // Disable animation if the target is too far
        }
    }

    void animate(float deltaTime) {
        if (active && progress < 1.0f) {
            progress += deltaTime * 1.f; // Adjust speed here
            end = start + progress * (target - start);
        }
    }
};

std::vector<sf::Vector2f> getHexagonalPoints(const sf::Vector2f& playerPosition);
sf::Vector2f findClosestWall(const sf::Vector2f& start, const sf::Vector2f& direction, const std::vector<std::vector<int>>& gridColors, int rows, int cols);
sf::Vector2f findStartingPosition(const std::vector<std::vector<int>>& gridColors, int rows, int cols);