#include "Engine.hpp"
#include "Physics.hpp"
#include <SFML/Window/Event.hpp>

Engine::Engine(unsigned int w, unsigned int h)
    : width(w), height(h)
{
    window.create(sf::VideoMode({ width, height }), "Ray Bending Sim");

    // Center of the screen (in pixels)
    screenCenter = sf::Vector2f(width * 0.5f, height * 0.5f);

    // Scaling factor: how many pixels = 1 geometric unit
    // Tweak this later based on how big you want the horizon
    screenScale = 40.0f;
}

void Engine::init() {
    window.setFramerateLimit(120);
    // ensure we have something to see
    spawnTestScene();   // make sure this is called once on init
    // also spawn a single demonstrator near left
	screenScale = 40.f; // just to be sure
   }

void Engine::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processInput();
        if (!paused) update(dt);
        render();
    }
}

void Engine::processInput() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        // Window close
        if (event->is<sf::Event::Closed>()) {
            window.close();
            continue;
        }

        // Key pressed
        if (event->is<sf::Event::KeyPressed>()) {
            const auto* key = event->getIf<sf::Event::KeyPressed>();
            if (key->code == sf::Keyboard::Key::Space) paused = !paused;
            if (key->code == sf::Keyboard::Key::R) spawnTestScene();
            continue;
        }

        // Mouse button pressed (SFML 3 style)
        //if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
        //    if (mb->button == sf::Mouse::Button::Left) {
        //        Ray r;
        //        // position is a Vector2i in SFML 3
        //        r.r = sf::Vector2f(static_cast<float>(mb->position.x),
        //            static_cast<float>(mb->position.y));
        //        r.phi = sf::Vector2f(1.f, 0.f);
        //        rays.push_back(r);
        //    }
        //    continue;
        //}
    }
}


void Engine::update(float dt) {
    // choose substeps; increase if traces look jittery
    const int substeps = 8;
    stepSimulation(rays, blackHoles, static_cast<double>(dt), substeps);

    // then push screen-space trail points and prune
    for (int i = static_cast<int>(rays.size()) - 1; i >= 0; --i) {
        Ray& r = rays[i];

        // push screen-space point to trail for rendering
        r.trail.push_back(polarToScreen(r.r, r.phi));
        if (r.trail.size() > 700) {
            r.trail.erase(r.trail.begin(), r.trail.begin() + (r.trail.size() - 650));
        }

        // optionally remove rays far off-screen (prune)
        sf::Vector2f sp = r.trail.back();
        if (sp.x < -500 || sp.x > width + 500 || sp.y < -500 || sp.y > height + 500) {
            rays.erase(rays.begin() + i);
        }
    }

}

void Engine::render() {
    window.clear(sf::Color(5,20,40));

    // --- draw black holes (compute pixel radius every frame)
    for (const auto& bh : blackHoles) {
        // Physical Schwarzschild radius in geometric units (Rs = 2 * M)
        double Rs_geom = 2.0 * static_cast<double>(bh.mass);

        // Convert to pixels using current screenScale
        float Rs_px = static_cast<float>(Rs_geom * screenScale);

        // Minimal visible size so tiny BHs don't disappear:
        //Rs_px = std::max(Rs_px, 6.0f);

        // If you store bh.pos in polar (r,phi), use polarToScreen; if bh.pos is screen coords use directly.
    
        sf::Vector2f bhScreen = polarToScreen(bh.pos.x, bh.pos.y);

        sf::CircleShape horizon(Rs_px);
        horizon.setOrigin({ Rs_px, Rs_px });
        horizon.setPosition(bhScreen);
        horizon.setFillColor(sf::Color::Black);
        window.draw(horizon);

        // outline for visibility
        float outlineR = Rs_px * 1.25f;
        sf::CircleShape outline(outlineR);
        outline.setOrigin({ outlineR, outlineR });
        outline.setPosition(bhScreen);
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineThickness(1.0f);
        outline.setOutlineColor(sf::Color(100, 100, 120));
        window.draw(outline);
    }

    // --- draw rays and trails
    // choose head radius relative to screenScale so it scales with zoom
    float headRadius = std::max(1.0f, 0.02f * screenScale); // tweak multiplier as you like
    sf::CircleShape headShape(headRadius);
    headShape.setOrigin({ headRadius, headRadius });

    for (const auto& r : rays) {
        // If your trail stores screen coords already, draw them directly.
        // If your trail stores polar (r,phi), replace below with polarToScreen conversion.
        for (const auto& tp : r.trail) {
            headShape.setPosition(tp);
            headShape.setFillColor(sf::Color::White);
            window.draw(headShape);
        }

        // draw the head (convert current polar -> screen)
        sf::Vector2f headPos = polarToScreen(r.r, r.phi);
        headShape.setPosition(headPos);
        headShape.setFillColor(r.captured ? sf::Color::Red : sf::Color::White);
        window.draw(headShape);
    }

    window.display();
}


void Engine::spawnTestScene() {
    rays.clear();
    blackHoles.clear();

    // central BH
    BlackHole bh;
    bh.pos = sf::Vector2f(0.f, 0.f ); // we treat BH as origin in physics; pos field may be unused now
    bh.mass = 1.0;                   // geometric mass M = 1.0 (tune visually via screenScale)
    blackHoles.push_back(bh);

    // spawn horizontal rays from left side (screen coords)
    const float startX = 80.f;
    const float spacing = 25.f;
    const int halfSpan = 300;
    for (int y = -halfSpan; y <= halfSpan; y += static_cast<int>(spacing)) {
        float sy = screenCenter.y + static_cast<float>(y);
        spawnRayFromScreen(startX, sy,1.0); // horizontal right
    }
}
void Engine::spawnRayFromScreen(float sx, float sy, double speedOverride /*optional*/) {
    double speed = (speedOverride > 0.0) ? speedOverride : lightspeed;

    Ray r;
    double geom_r, geom_phi;
    screenToPolar(sf::Vector2f(sx, sy), geom_r, geom_phi);

    // hardcoded rightwards screen-space direction
    double vx = 1.0 / std::sqrt(2.0);
    double vy = -1.0 / std::sqrt(2.0);


    // project into polar basis (radial and tangential components)
    double vr = std::cos(geom_phi) * vx + std::sin(geom_phi) * vy;            // radial component
    double v_tan = (-std::sin(geom_phi) * vx + std::cos(geom_phi) * vy);     // tangential linear speed

    // convert tangential linear speed -> angular rate
    double eps = 1e-9;
    double angRate = v_tan / std::max(geom_r, eps);

    // enforce constant light speed:
    // linear_speed^2 = vr^2 + (r * angRate)^2  (because tangential linear = r * angRate)
    double linear_speed = std::sqrt(vr * vr + (geom_r * angRate) * (geom_r * angRate));
    if (linear_speed > 1e-12) {
        double s = speed / linear_speed;   // scale factor to make linear_speed == speed
        vr *= s;
        angRate *= s;
    }
    else {
        // degenerate case (spawn at origin or numerically tiny r) — nudge outward
        vr = speed;        // push outward at c
        angRate = 0.0;
    }

    // store in ray (velocities in geometric units per second)
    r.r = geom_r;
    r.phi = geom_phi;
    r.pr = vr;             // dr/dt-like
    r.pphi = angRate;// dphi/dt-like
    r.L = r.r * r.r * r.pphi;

    r.captured = false;
    r.trail.reserve(40);
    r.trail.push_back(polarToScreen( r.r, r.phi ));

    rays.push_back(std::move(r));
}


//Helper functions///



// convert Cartesian screen point (pixels) to polar (geom units)
inline void Engine::screenToPolar(const sf::Vector2f& screenPos, double& out_r, double& out_phi) {
    double dx = (screenPos.x - screenCenter.x) / screenScale;
    double dy = (screenCenter.y - screenPos.y) / screenScale; // flipped Y
    out_r = std::hypot(dx, dy);
    out_phi = std::atan2(dy, dx); // returns -pi..pi
}

// convert polar (geom units) -> screen coords (pixels)
inline sf::Vector2f Engine::polarToScreen(double r, double phi) {
    double x = r * std::cos(phi);
    double y = r * std::sin(phi);
    sf::Vector2f out_screenPos(
        static_cast<float>(screenCenter.x + x * screenScale),
        static_cast<float>(screenCenter.y - y * screenScale)
    );
    return out_screenPos;

}
