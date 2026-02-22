#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Entities.hpp"

class Engine {
public:
    Engine(unsigned int w, unsigned int h);
    void init();
    void run();

private:
    unsigned int width;
    unsigned int height;
    sf::RenderWindow window;
    sf::Clock clock;
    bool paused = false;

    sf::Vector2f screenCenter;   // center of the window in pixels
    float screenScale;

    std::vector<Ray> rays;
    std::vector<BlackHole> blackHoles;

    // physics params
    float G = 1.0f;
    float softening = 1e-1f;
    double lightspeed = 1.0;
    // core loop pieces
    void processInput();
    void update(float dt);
    void render();
    void spawnTestScene(); // helper to populate rays and BHs
	void spawnRayFromScreen(float sx, float sy, double speedOverride /*optional*/);
	inline void screenToPolar(const sf::Vector2f& screenPos, double& out_r, double& out_phi);
	inline sf::Vector2f polarToScreen(double r, double phi);
};

