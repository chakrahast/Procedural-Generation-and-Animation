#include "spider.hpp"

std::vector<sf::Vector2f> getHexagonalPoints(const sf::Vector2f& playerPosition) {
    std::vector<sf::Vector2f> points;
    float angles[] = {0, 60, 120, 180, 240, 300}; // hexagon angles

    for (float angle : angles) {
        float radians = angle * (M_PI / 180.0f); 
        float offsetX = HEXAGON_DISTANCE * cos(radians);
        float offsetY = HEXAGON_DISTANCE * sin(radians);

        points.emplace_back(playerPosition.x + (GRID_SPACING / 2) + offsetX, playerPosition.y + (GRID_SPACING / 2) + offsetY);
    }
    return points;
}

sf::Vector2f findClosestWall(const sf::Vector2f& start, const sf::Vector2f& direction, 
                             const std::vector<std::vector<int>>& gridColors, int rows, int cols) {
    sf::Vector2f current = start;
    sf::Vector2f step = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y); // Normalize direction
    step *= GRID_SPACING / 10.0f; // Step size

    while (true) {
        int row = static_cast<int>(current.y / GRID_SPACING);
        int col = static_cast<int>(current.x / GRID_SPACING);

        // Check if out of bounds
        if (row < 0 || col < 0 || row >= rows || col >= cols) {
            break; // Out of grid, return the last position
        }

        // Check for a wall
        if (gridColors[row][col] == WALL) {
            return sf::Vector2f(col * GRID_SPACING + GRID_SPACING / 2, row * GRID_SPACING + GRID_SPACING / 2); // Center of wall cell
        }

        current += step;
    }

    return start; // No wall found, return the starting position
}