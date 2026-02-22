#SchwarzschildGeodesics
Schwarzschild Geodesics Simulation (C++ / SFML)

A real-time 2D visualization of light-ray (null geodesic) trajectories in the Schwarzschild spacetime around a black hole.
Built in C++ using SFML for rendering. The simulation demonstrates gravitational lensing, photon capture, and curved spacetime effects in a simplified, interactive environment.

This project is intended as a small, self-contained benchmark for:

C++ project structure

CMake-based builds

SFML rendering

Basic general relativity–inspired physics simulations

Features

Real-time ray tracing around a Schwarzschild black hole

Visual intuition for gravitational lensing and photon capture

Configurable initial ray directions and step size

Clean CMake + vcpkg setup for reproducible builds

Works out of the box on Windows (Visual Studio)

Tech Stack

Language: C++20

Rendering: SFML 3.x

Build System: CMake

Dependency Management: vcpkg

Platform: Windows (Linux/macOS should work with minor tweaks)

Project Structure
```
SchwarzschildGeodesics/
├── CMakeLists.txt
├── CMakePresets.json
├── src/
│   ├── main.cpp
│   ├── Engine.cpp
│   ├── Physics.cpp
│   ├── Entities.hpp
│   └── ...
├── assets/ 
├── build/         (generated, ignored by git)
└── README.

```
Prerequisites

You need the following installed:

Visual Studio 2022 / 2026 (with C++ Desktop Development workload)

CMake (≥ 3.24)

vcpkg

Install SFML via vcpkg:

vcpkg install sfml:x64-windows

Make sure vcpkg integration is available:

vcpkg integrate install
Build & Run (Windows)
1. Clone the repo
```
git clone <your-repo-url>
cd SchwarzschildGeodesics
```
2. Configure with CMake Preset
```
cmake --preset windows-vs-vcpkg
```
3. Build
```
cmake --build build --config Release
```
4. Run
```
.\build\x64-Release\SchwarzschildGeodesics.exe
```

If you are using Visual Studio:

Open the folder (not the .sln)

Select the windows-vs-vcpkg preset

Click Run

Controls / Usage

Rays are initialized with configurable velocity vectors

Adjust step size (dt) and initial ray directions in code

Observe bending, capture, and escape trajectories near the event horizon

(If you want, you can document hotkeys or runtime controls here later.)

Notes

This is a toy simulation, not a numerically precise GR solver.

The Schwarzschild metric is simplified for visualization purposes.

The focus is educational and visual intuition, not scientific accuracy.

Known Limitations

2D approximation of a fundamentally 3D spacetime

No adaptive step sizing yet (large dt may cause visual artifacts)

Rays are terminated when leaving bounds or crossing the horizon

Future Improvements

Adaptive step integration (Runge–Kutta)

Better event horizon handling

Interactive controls (spawn rays, move camera, change mass)

Export frames or trajectories for analysis

Linux/macOS build presets

License

MIT License

Credits

Built by Chayan
Inspired by gravitational lensing and null geodesics in Schwarzschild spacetime.