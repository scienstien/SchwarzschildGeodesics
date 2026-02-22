#include <SFML/Graphics.hpp>
#include <iostream>
#include "Engine.hpp"
using namespace std;
using namespace sf;

int main()
{
    try {
        Engine engine(1280, 720);
        engine.init();
        engine.run();
    }
    catch (const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Unknown non-std exception caught\n";
    }
    return 0;
}