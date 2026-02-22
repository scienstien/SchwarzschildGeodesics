// Physics.cpp
#include "Physics.hpp"
#include "Entities.hpp"   // Ray, BlackHole
#include <array>
#include <cmath>
#include <algorithm>
#include <iostream>

// derivative function for the reduced null-geodesic system in Schwarzschild (equatorial plane).
// State vector y = { r, pr, phi } where pr = dr/dλ and dφ/dλ = L / r^2 (L is constant).
// Returns dy/dλ = { dr/dλ, dpr/dλ, dphi/dλ }.
static inline std::array<double, 3> geodesic_deriv(const std::array<double, 3>& y, double L, double M) {
    double r = y[0];
    double pr = y[1];
    // dphi/dλ = L / r^2
    double dphi = L / (r * r);

    // compute dpr/dλ = L^2 * (1/r^3 - 3M/r^4)
    // derived from the effective potential for null geodesics in Schwarzschild.
    double r2 = r * r;
    double r3 = r2 * r;
    double r4 = r3 * r;

    double dpr = 0.0;
    if (r > 0.0) {
        dpr = L * L * (1.0 / r3 - 3.0 * M / r4);
    }

    return { pr, dpr, dphi };
}

void stepSimulation(std::vector<Ray>& rays,
    const std::vector<BlackHole>& blackHoles,
    double dtFrame,
    int substeps)
{
    if (blackHoles.empty() || rays.empty()) return;

    // Use the first black hole as central mass (we assume BH at origin).
    const double M = static_cast<double>(blackHoles[0].mass);

    // Safety thresholds
    const double min_r = 1e-9;

    // choose the affine step size h = dtFrame / substeps
    double h = dtFrame / static_cast<double>(std::max(1, substeps));

    // iterate rays
    for (auto& ray : rays) {
        if (ray.captured) continue;

        // Ensure L is set. If not, compute from pphi (backwards-compatible).
        if (ray.L == 0.0 && std::abs(ray.pphi) > 0.0) {
            ray.L = ray.r * ray.r * ray.pphi;
        }

        // RK4 sub-stepping in affine parameter lambda
        for (int s = 0; s < substeps; ++s) {
            // pack state
            std::array<double, 3> y = { ray.r, ray.pr, ray.phi };

            // k1 = f(y)
            auto k1 = geodesic_deriv(y, ray.L, M);

            // k2 = f(y + 0.5*h*k1)
            std::array<double, 3> ytmp;
            for (int i = 0; i < 3; ++i) ytmp[i] = y[i] + 0.5 * h * k1[i];
            auto k2 = geodesic_deriv(ytmp, ray.L, M);

            // k3 = f(y + 0.5*h*k2)
            for (int i = 0; i < 3; ++i) ytmp[i] = y[i] + 0.5 * h * k2[i];
            auto k3 = geodesic_deriv(ytmp, ray.L, M);

            // k4 = f(y + h*k3)
            for (int i = 0; i < 3; ++i) ytmp[i] = y[i] + h * k3[i];
            auto k4 = geodesic_deriv(ytmp, ray.L, M);

            // combine: y += (h/6) * (k1 + 2*k2 + 2*k3 + k4)
            for (int i = 0; i < 3; ++i) {
                y[i] = y[i] + (h / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
            }

            // write back to ray
            ray.r = y[0];
            ray.pr = y[1];
            ray.phi = y[2];

            // capture check: event horizon at r <= 2M
            if (ray.r <= 2.0 * M) {
                ray.captured = true;
                break;
            }

            // guard: clamp tiny r and NaNs
            if (!(ray.r > 0.0) || std::isnan(ray.r)) {
                ray.r = min_r;
                ray.pr = 0.0;
                ray.captured = true;
                break;
            }
        } // end substeps
    } // end rays
}
