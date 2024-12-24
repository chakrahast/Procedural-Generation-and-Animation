// tree.cpp
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>


float toRadians(float degrees) {
    return degrees * (M_PI / 180.0f);
}

// Recursive function to draw a tree with inverse kinematics
void drawIKTree(sf::RenderWindow &window, sf::Vector2f start, float length, float angle, int depth, int branchingFactor, float swayOffset, float time) {
    if (depth <= 0 || length <= 1) {
        return;
    }

    // Calculate sway based on time and depth for a layered animation effect
    float dynamicSway = swayOffset * std::sin(time + depth * 0.5f);

    // Calculate the end point of the current branch
    sf::Vector2f end(
        start.x + length * cos(toRadians(angle + dynamicSway)),
        start.y - length * sin(toRadians(angle + dynamicSway))
    );

    // Draw the line for the current branch
    sf::Vertex line[] = {
        sf::Vertex(start, sf::Color{ 100 , 95, 145 }),
        sf::Vertex(end, sf::Color{ 100 , 95, 145 })
    };
    window.draw(line, 2, sf::Lines);

//     std::srand(std::time(0));

// float randomAngle = 45.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) / (120.0f - 45.0f));
// float branchAngleIncrement = randomAngle / (branchingFactor - 1);


    // Calculate new branches
    float branchAngleIncrement = 60.0f / (branchingFactor - 1);
    for (int i = 0; i < branchingFactor; ++i) {
        float newAngle = angle - 30 + i * branchAngleIncrement;

        // Add sway to child branches for smoother motion
        float childSwayOffset = dynamicSway * 0.5f;

        // Recursively draw branches
        drawIKTree(window, end, length * 0.7f, newAngle, depth - 1, branchingFactor, childSwayOffset, time);
    }
}