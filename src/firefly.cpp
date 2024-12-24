#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For seeding rand()

bool isMoving = false;

// Function to create a gradient texture for a resting shape (original behavior)
sf::Texture createGradientTextureRest(float radius, sf::Color baseColor, float falloffRate) {
    sf::Image gradient;
    unsigned int size = static_cast<unsigned int>(radius * 2);
    gradient.create(size, size, sf::Color::Transparent);

    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            float dx = x - radius;
            float dy = y - radius;
            float distanceSquared = dx * dx + dy * dy;

            float falloff = std::exp(-distanceSquared / (falloffRate * radius * radius));
            sf::Color pixelColor(
                static_cast<sf::Uint8>(baseColor.r * falloff),
                static_cast<sf::Uint8>(baseColor.g * falloff),
                static_cast<sf::Uint8>(baseColor.b * falloff),
                static_cast<sf::Uint8>(255.0f * falloff)
            );

            if (distanceSquared <= radius * radius) {
                gradient.setPixel(x, y, pixelColor);
            }
        }
    }

    sf::Texture texture;
    texture.loadFromImage(gradient);
    return texture;
}

sf::Texture createGradientTextureMove(float radius, float falloffRate) {
    sf::Image gradient;
    unsigned int size = static_cast<unsigned int>(radius * 2);
    gradient.create(size, size, sf::Color::Transparent);

    // Core color (light blue) and edge color (dark blue or purple)
    sf::Color coreColor(0, 255, 255, 255);  // Light blue (cyan) for the center
    sf::Color edgeColor(0, 0, 139, 255);    // Dark blue (navy) for the edges

    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            float dx = x - radius;
            float dy = y - radius;
            float distanceSquared = dx * dx + dy * dy;
            float distance = std::sqrt(distanceSquared);

            // Non-linear falloff using an exponential decay
            float falloff = std::exp(-distance / (falloffRate * radius));

            // Apply a sinusoidal variation to simulate fire flicker at the edges
            float noiseFactor = 0.5f + 0.5f * std::sin(distance * 0.1f + (rand() % 100 / 100.0f)); // Noise for flickering

            // Apply random turbulence to distort the edges and make it look alive
            float turbulenceX = (rand() % 100 / 50.0f - 1.0f) * 0.1f;  // Horizontal randomness
            float turbulenceY = (rand() % 100 / 50.0f - 1.0f) * 0.1f;  // Vertical randomness

            // Stretch and bend edges using sinusoidal waves
            float bendFactor = 0.1f * std::sin(distance * 0.3f + turbulenceX);  // Bending effect

            // Add chaos to the position at the edges
            dx += bendFactor;  // Apply the bend distortion
            dy += bendFactor * 0.5f;  // Apply vertical stretch distortion

            // Color interpolation: smoother color transition for core to edge
            float interpolation = std::min(1.0f, distance / radius);
            sf::Color pixelColor(
                static_cast<sf::Uint8>(coreColor.r * (1.0f - interpolation) + edgeColor.r * interpolation * falloff * noiseFactor),
                static_cast<sf::Uint8>(coreColor.g * (1.0f - interpolation) + edgeColor.g * interpolation * falloff * noiseFactor),
                static_cast<sf::Uint8>(coreColor.b * (1.0f - interpolation) + edgeColor.b * interpolation * falloff * noiseFactor),
                static_cast<sf::Uint8>(255.0f * falloff * noiseFactor)  // Alpha fades based on distance
            );

            if (distanceSquared <= radius * radius) {
                gradient.setPixel(x, y, pixelColor);
            }
        }
    }

    // Load the generated gradient into a texture
    sf::Texture texture;
    texture.loadFromImage(gradient);
    return texture;
}



// Jitter motion utility
sf::Vector2f applyJitter(sf::Vector2f position, float intensity) {
    float dx = (rand() % 100 / 50.0f - 1.0f) * intensity;
    float dy = (rand() % 100 / 50.0f - 1.0f) * intensity;
    return position + sf::Vector2f(dx, dy);
}

// FadingCircle Class
class FadingCircle {
private:
    sf::Vector2f position;
    float radius;
    sf::Color baseColor;
    float falloffRate;
    sf::Texture texture;
    sf::Sprite sprite;

    void update(const std::string& direction, bool moving) {
        if (moving) {
            texture = createGradientTextureMove(radius, falloffRate);
        } else {
            texture = createGradientTextureMove(radius, falloffRate);
        }
        sprite.setTexture(texture);
        sprite.setOrigin(radius, radius);
        sprite.setPosition(position);
    }

public:
    FadingCircle(float x, float y, float r, sf::Color color, float falloff)
        : position(x, y), radius(r), baseColor(color), falloffRate(falloff) {
        update("default", false);
    }

    void move(float dx, float dy, const std::string& direction) {
        position.x += dx;
        position.y += dy;
        update(direction, true);  // Update texture when moving
    }

    void stop() {
        update("default", false);  // Update texture when not moving
    }

    void jitter(float intensity) {
        position = applyJitter(position, intensity);
        sprite.setPosition(position);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    float getRadius() const {
        return radius;
    }

    void setRadius(float newRadius) {
        radius = newRadius;
        update("default", false);  // Re-update the texture after radius change
    }
};




FadingCircle fadingCircle(400, 400, 200, sf::Color(255, 0, 0), 1.0f);


int main() {
    srand(static_cast<unsigned int>(time(0)));

    sf::RenderWindow window(sf::VideoMode(800, 800), "Fireball Movement");
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setPosition(sf::Vector2i((desktop.width - window.getSize().x) / 2, (desktop.height - window.getSize().y) / 2));

    FadingCircle fadingCircle(400, 400, 200, sf::Color(255, 0, 0), 1.0f);

    sf::Clock jitterClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Initialize direction and movement deltas
        std::string direction = "default";
        float dx = 0.0f, dy = 0.0f;

        // Update direction and deltas based on keypresses
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            direction = "down";
            dy = -0.5f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            direction = "up";
            dy = 0.5f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            direction = "right";
            dx = -0.5f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            direction = "left";
            dx = 0.5f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            if(fadingCircle.getRadius()<150){
                fadingCircle.setRadius(fadingCircle.getRadius()+1);
            }
        }
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
            if(fadingCircle.getRadius()>20){
                fadingCircle.setRadius(fadingCircle.getRadius()-1);
            }
        }

        // Move the circle and update texture only if moving
        if (direction != "default") {
            fadingCircle.move(dx, dy, direction);
        } else {
            // Reset to circular shape when not moving
            fadingCircle.stop();
        }

        // Apply jitter every 100ms
        if (jitterClock.getElapsedTime().asMilliseconds() > 100) {
            fadingCircle.jitter(5.0f);
            jitterClock.restart();
        }

        // Render
        window.clear(sf::Color::Black);
        fadingCircle.draw(window);
        window.display();
    }

    return 0;
}