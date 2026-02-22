#pragma once
#include <vector>

struct Ray;
struct BlackHole;

// Advance null geodesics in Schwarzschild spacetime
void stepSimulation(
    std::vector<Ray>& rays,
    const std::vector<BlackHole>& blackHoles,
    double dtFrame,
    int substeps = 6
);
