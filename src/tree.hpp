// tree.h
#ifndef TREE_H
#define TREE_H

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <SFML/Graphics.hpp>

void drawIKTree(sf::RenderWindow &window, sf::Vector2f start, float length, float angle, int depth, int branchingFactor, float swayOffset, float time);

#endif // TREE_H