#include "mazegen.hpp"
#include "spider.hpp"
#include "tree.hpp"

#define MOVE_DURATION 60000 // Duration of player movement in microseconds

// Function to find the first open cell (PATH) from the bottom-left of the grid
sf::Vector2f findStartingPosition(const std::vector<std::vector<int>>& gridColors, int rows, int cols) {  //BFS
    std::queue<Cell> q;
    q.push(Cell(rows - 1, 0)); // Start from the bottom-left corner

    while (!q.empty()) {
        Cell current = q.front();
        q.pop();

        if (gridColors[current.row][current.col] == PATH) {
            return sf::Vector2f(current.col * GRID_SPACING, current.row * GRID_SPACING);
        }

        std::vector<Cell> neighbors = {
            {current.row - 1, current.col}, 
            {current.row + 1, current.col}, 
            {current.row, current.col - 1}, 
            {current.row, current.col + 1}  
        };

        for (const Cell& neighbor : neighbors) {
            if (neighbor.isInsideGrid(rows, cols)) {
                q.push(neighbor);
            }
        }
    }
    // Default to top-left if no open cell is found
    return sf::Vector2f(0, 0);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Maze spider");

    int rows = window.getSize().y / GRID_SPACING;
    int cols = window.getSize().x / GRID_SPACING;

    std::vector<std::vector<int>> gridColors(rows, std::vector<int>(cols, 0));

    srand(static_cast<unsigned>(time(0)));

    // ------------------------------------ Cellular Automata Maze Generation ------------------------------------
    MazeGenerator* generator = new CellularAutomataGenerator(WALL_PROBABILITY, CA_STEPS);

    // ------------------------------------ Drunk Walk Maze Generation ------------------------------------
    // MazeGenerator* generator = new DrunkWalkGenerator(DRUNK_WALK_STEPS);

    // ------------------------------------ Prim's Maze Generation ------------------------------------
    // MazeGenerator* generator = new PrimGenerator();

    // ------------------------------------ L-System Maze Generation ------------------------------------
    // MazeGenerator* generator = new LSystemGenerator(L_SYSTEM_ITERATIONS, L_SYSTEM_STARTPOINTS);


    generator->generateMaze(gridColors, rows, cols);
    std::cout << "Maze generated!" << std::endl;

    // with 1.25% chance, make a wall a light
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (gridColors[row][col] == WALL && rand() % 80 == 0) {
                gridColors[row][col] = LIGHT;
            }
        }
    }

    sf::Texture wallTexture;
    if(!wallTexture.loadFromFile("assets/grey-wall.png")) {
        std::cerr << "Failed to load wall texture!" << std::endl;
        return 1;
    }

    sf::Texture surfaceTexture;
    if(!surfaceTexture.loadFromFile("assets/grey-surface.png")) {
        std::cerr << "Failed to load surface texture!" << std::endl;
        return 1;
    }

    sf::Texture backgroundTexture;
    if(!backgroundTexture.loadFromFile("assets/background.png")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return 1;
    }

    sf::Texture lightTexture;
    if(!lightTexture.loadFromFile("assets/light2.png")) {
        std::cerr << "Failed to load light texture!" << std::endl;
        return 1;
    }

    wallTexture.setSmooth(true);
    surfaceTexture.setSmooth(true);
    backgroundTexture.setSmooth(true);
    lightTexture.setSmooth(true);

    sf::Sprite wallSprite;
    wallSprite.setTexture(wallTexture);
    wallSprite.setScale(
        GRID_SPACING / static_cast<float>(wallSprite.getTexture()->getSize().x),
        GRID_SPACING / static_cast<float>(wallSprite.getTexture()->getSize().y)
    );

    sf::Sprite surfaceSprite;
    surfaceSprite.setTexture(surfaceTexture);
    surfaceSprite.setScale(
        GRID_SPACING / static_cast<float>(surfaceSprite.getTexture()->getSize().x),
        GRID_SPACING / static_cast<float>(surfaceSprite.getTexture()->getSize().y)
    );

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
        GRID_SPACING / static_cast<float>(backgroundSprite.getTexture()->getSize().x),
        GRID_SPACING / static_cast<float>(backgroundSprite.getTexture()->getSize().y)
    );

    sf::Sprite lightSprite;
    lightSprite.setTexture(lightTexture);
    lightSprite.setScale(
        GRID_SPACING / static_cast<float>(lightSprite.getTexture()->getSize().x),
        GRID_SPACING / static_cast<float>(lightSprite.getTexture()->getSize().y)
    );

    std::vector<sf::Vector2f> lightSources;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (gridColors[row][col] == LIGHT) {
                lightSources.emplace_back(col, row);
            }
        }
    }

    // -------------------------------make the whole maze a path---------------------------------------
    // for (int row = 1; row < rows - 1; ++row) {
    //     for (int col = 1; col < cols - 1; ++col) {
    //         gridColors[row][col] = PATH;
    //     }
    // }


    //------------------------------------Tree----------------------------------------------------------

    std::vector<sf::Vector2f> treeGridArray; 

    for (int row = 2; row < rows; ++row) { 
        for (int col = 1; col < cols - 1; ++col) { 
            if (gridColors[row][col] == WALL) {
                bool validPosition = 
                    gridColors[row - 1][col] == PATH &&
                    gridColors[row - 1][col - 1] == PATH &&
                    gridColors[row - 1][col + 1] == PATH &&
                    gridColors[row - 2][col] == PATH &&
                    gridColors[row - 2][col - 1] == PATH &&
                    gridColors[row - 2][col + 1] == PATH;

                if (validPosition) {
                    if (validPosition && (rand() % 5 == 0)) { // Randomly decide whether to add the tree
                        treeGridArray.push_back(sf::Vector2f(col * GRID_SPACING + GRID_SPACING / 2, row * GRID_SPACING));

                        lightSources.emplace_back(col - 1, row);
                        lightSources.emplace_back(col + 1, row);
                        lightSources.emplace_back(col, row - 1);
                        lightSources.emplace_back(col - 1, row - 1);
                        lightSources.emplace_back(col + 1, row - 1);

                    }
                }
            }
        }
    }

    sf::Vector2f rootPosition;
    // Root position for the tree
    if(treeGridArray.size() > 0) {
        sf::Vector2f rootPosition(treeGridArray[0]);
        printf("Root position: %f, %f\n", rootPosition.x, rootPosition.y);

    }
    // sf::Vector2f rootPosition(treeGridArray[0]);

    // Parameters for the tree
    float swayAmplitude = 10.0f; // Amplitude of sway in degrees
    float swaySpeed = 1.5f; // Speed of sway
    sf::Clock clock;


    // ------------------------------------ Player Movement ------------------------------------
    sf::RectangleShape player(sf::Vector2f(GRID_SPACING, GRID_SPACING));
    player.setFillColor(sf::Color::Red);

    sf::Vector2f playerPos = findStartingPosition(gridColors, rows, cols);
    player.setPosition(playerPos);

    std::cout << "Player starting position: " << playerPos.x << ", " << playerPos.y << std::endl;

    bool isFalling = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // Linearly interpolate the position between old and new, such that speed is equal to 10000us
        static sf::Vector2f startPos;
        static sf::Vector2f endPos;
        static bool isMoving = false;
        static sf::Clock moveClock, droneWalkTimer;
        static float droneWalkInterval = 2.0f;
        static int droneMoving = 0;
        const sf::Time moveDuration = sf::microseconds(MOVE_DURATION);

        static float fallingSpeed = 0.0f; // Initialize falling speed
        const float GRAVITY = 0.01f; // Gravity acceleration per frame
        const float TERM_VELO = 5.0f; // Maximum falling speed
        const float LIGHT_RADIUS = 8.0f; // Radius of light effect
        

        // ---------------------------------------- Player Movement ----------------------------------------
        if (!isMoving) {
            sf::Vector2f playerNewPos = player.getPosition();

            bool keyPressed = false;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !isFalling) {
                playerNewPos.y -= GRID_SPACING;
                playerNewPos.x -= GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !isFalling) {
                playerNewPos.y -= GRID_SPACING;
                playerNewPos.x += GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                playerNewPos.y += GRID_SPACING;
                playerNewPos.x -= GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                playerNewPos.y += GRID_SPACING;
                playerNewPos.x += GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !isFalling) {
                playerNewPos.y -= GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                playerNewPos.y += GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                playerNewPos.x -= GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                playerNewPos.x += GRID_SPACING;
                keyPressed = true;
            }

            int newRow = static_cast<int>(playerNewPos.y / GRID_SPACING);
            int newCol = static_cast<int>(playerNewPos.x / GRID_SPACING);

            if (keyPressed && isInBounds(newRow, newCol, rows, cols) && gridColors[newRow][newCol] != WALL && gridColors[newRow][newCol] != LIGHT) {
                startPos = player.getPosition();
                endPos = sf::Vector2f(newCol * GRID_SPACING, newRow * GRID_SPACING);
                moveClock.restart();
                isMoving = true;
            }
        }

        if (isMoving) {
            float t = moveClock.getElapsedTime().asMicroseconds() / static_cast<float>(moveDuration.asMicroseconds());
            if (t >= 1.f) {
                t = 1.f;
                isMoving = false;
            }
            sf::Vector2f currentPos = startPos + t * (endPos - startPos);
            player.setPosition(currentPos);
        }

        std::vector<Limb> limbs;
        std::vector<sf::Vector2f> hexagonPoints = getHexagonalPoints(player.getPosition());

        // ---------------------------------- Limb and Limb Guidelines Animation ----------------------------------
        for (const auto& point : hexagonPoints) {
            sf::Vector2f direction = point - player.getPosition() + sf::Vector2f(GRID_SPACING / 2, GRID_SPACING / 2);
            sf::Vector2f wallPos = findClosestWall(player.getPosition() + sf::Vector2f(GRID_SPACING / 2, GRID_SPACING / 2), direction, gridColors, rows, cols);
            limbs.emplace_back(player.getPosition() + sf::Vector2f(GRID_SPACING / 2, GRID_SPACING / 2), wallPos);
        }

        sf::VertexArray guideLines(sf::Lines);
        for (const auto& point : hexagonPoints) {
            guideLines.append(sf::Vertex(player.getPosition() + sf::Vector2f(player.getSize().x / 2, player.getSize().y / 2), sf::Color(225,135, 0)));
            guideLines.append(sf::Vertex(point, sf::Color(225, 135, 0))); // Orange color
        }

        for (Limb& limb : limbs) {
            limb.animate(1.f); // Animate the limb
        }

        // ---------------------------------------- Falling ----------------------------------------
        // Count active limbs
        int activeLimbs = 0;
        for (const auto& limb : limbs) {
            if (limb.active) {
                activeLimbs++;
            }
        }
        if (activeLimbs == 0) {
            // No limbs connected: full GRAVITY
            fallingSpeed += GRAVITY;
            isFalling = true;
        } else if (activeLimbs < 3) {
            // Less than 3 limbs: scaled GRAVITY
            fallingSpeed += GRAVITY / activeLimbs;
            isFalling = true;
        } else {
            fallingSpeed = 0.0f;
            isFalling = false;
        }
        if (fallingSpeed > TERM_VELO) {
            fallingSpeed = TERM_VELO;
        }
        player.move(0, fallingSpeed);

        // Prevent the player from falling below the grid
        if (player.getPosition().y > rows * GRID_SPACING) {
            player.setPosition(player.getPosition().x, rows * GRID_SPACING - GRID_SPACING);
            fallingSpeed = 0.0f; // Reset falling speed
            isFalling = false;
        }

        int playerPosition_x = static_cast<int>(player.getPosition().x / GRID_SPACING);
        int playerPosition_y = static_cast<int>(player.getPosition().y / GRID_SPACING);

        // ---------------------------------------- Drawing ----------------------------------------
        window.clear(sf::Color::White);

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                sf::Sprite cellSprite;

                if (gridColors[row][col] == WALL) {
                    cellSprite = (row > 0 && (gridColors[row - 1][col] == WALL || gridColors[row - 1][col] == LIGHT))
                                    ? wallSprite
                                    : surfaceSprite;
                } else if (gridColors[row][col] == PATH) {
                    cellSprite = backgroundSprite;
                } else if (gridColors[row][col] == LIGHT) {
                    cellSprite = lightSprite;
                }

                // Calculate attenuation based on light sources
                float lightbrightness = 0.05f; 

                for (const auto& lightPos : lightSources) { // `lightSources` contains all light positions
                    float localbrightness = 0.05f;
                    float dx = col - lightPos.x;
                    float dy = row - lightPos.y;
                    float distance = std::sqrt(dx * dx + dy * dy);

                    if (distance <= LIGHT_RADIUS) {
                        localbrightness += 1.0f / (1.4f + (distance / LIGHT_RADIUS) * (distance / LIGHT_RADIUS));   // Attenuation formula, contribution of one light source
                    }
                    lightbrightness = std::max(lightbrightness, localbrightness);   // Use the maximum brightness
                }

                float playerbrightness = 0.05f;

                // Include the player's light effect
                float playerDx = col - playerPosition_x; 
                float playerDy = row - playerPosition_y;
                float playerDistance = std::sqrt(playerDx * playerDx + playerDy * playerDy);

                if (playerDistance <= LIGHT_RADIUS) {
                    playerbrightness += 1.0f / (1.4f + (playerDistance / LIGHT_RADIUS) * (playerDistance / LIGHT_RADIUS));
                }

                float brightness = std::max(lightbrightness, playerbrightness); // Use the maximum brightness
                brightness = std::min(brightness, 1.0f); // Cap brightness to a maximum of 1.0

                // Adjust the sprite's color based on the brightness
                sf::Color color = sf::Color(255 * brightness, 255 * brightness, 255 * brightness);
                cellSprite.setColor(color);

                cellSprite.setPosition(col * GRID_SPACING, row * GRID_SPACING);
                window.draw(cellSprite);
            }
        }

        // window.draw(guideLines);
        window.draw(player);

        for (const auto& point : hexagonPoints) {
            sf::CircleShape circle(CIRCLE_RADIUS);
            circle.setFillColor(sf::Color::Blue);
            circle.setPosition(point.x - CIRCLE_RADIUS, point.y - CIRCLE_RADIUS);
            // window.draw(circle);
        }

        sf::VertexArray limbLines(sf::Lines);
        for (const auto& limb : limbs) {
            if(!limb.active) continue;
            limbLines.append(sf::Vertex(limb.start, sf::Color::Red));
            limbLines.append(sf::Vertex(limb.end, sf::Color::Red));
        }
        window.draw(limbLines);
        

        // -------------------------------------------Tree-------------------------------------------------------

        float initialLength = 17.0f;
        int maxDepth = 4; // Number of levels in the tree
        int branchingFactor = 2; // Number of branches at each node

        for (size_t i = 0; i < treeGridArray.size(); ++i) {
            // Vary parameters slightly for each tree
            float lengthVariation = initialLength + (i % 3) * 2.0f;
            int depthVariation = maxDepth + (i % 2);
            int branchingVariation = 2 + i%2;

            // Calculate sway offset based on time
            float time = clock.getElapsedTime().asSeconds();
            float swayOffset = swayAmplitude * sin(time * swaySpeed + i * 0.1f);

            drawIKTree(window, treeGridArray[i], lengthVariation, 90, depthVariation, branchingVariation, swayOffset, time);
        }

        window.display();
    }

    return 0;
}