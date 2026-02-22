cmake_minimum_required(VERSION 3.20)
project(SchwarzschildGeodesics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(SchwarzschildGeodesics
    src/main.cpp
    src/Engine.cpp
    src/Physics.cpp
)

target_link_libraries(SchwarzschildGeodesics
    SFML::Graphics
    SFML::Window
    SFML::System
)