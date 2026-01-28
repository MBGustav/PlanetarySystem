#pragma once

#include <iostream>
#include <vector>
#include <PlanetProperties.hpp>
#include <glm/glm.hpp>
#include <cmath>

template<typename T>
T G = static_cast<T>(2.96e-4); // gravitational constant in your units

// Compute accelerations due to gravity
template<typename T>
void compute_accelerations(std::vector<PlanetProperties<T>>& planets)
{
    // Reset each planet's acceleration
    for (auto& p : planets)
        p.set_acceleration(glm::tvec3<T>(0));

    // Pairwise gravity
    for (size_t i = 0; i < planets.size(); ++i) {
        for (size_t j = i + 1; j < planets.size(); ++j) {
            glm::tvec3<T> r = planets[j].get_position() - planets[i].get_position();
            T dist = glm::length(r);
            T dist3 = dist * dist * dist + static_cast<T>(1e-6); // avoid division by zero

            glm::tvec3<T> acc = G<T> * r / dist3;

            planets[i].set_acceleration(planets[i].get_acceleration() + planets[j].get_mass() * acc);
            planets[j].set_acceleration(planets[j].get_acceleration() - planets[i].get_mass() * acc);
        }
    }
}

// Velocity Verlet integration
template<typename T>
void velocity_verlet(std::vector<PlanetProperties<T>>& planets)
{
    std::vector<glm::tvec3<T>> a_old(planets.size());
    for (size_t i = 0; i < planets.size(); ++i)
        a_old[i] = planets[i].get_acceleration();

    // 1. Update positions
    for (size_t i = 0; i < planets.size(); ++i) {
        planets[i].set_position(planets[i].get_position() 
                                + planets[i].get_velocity() 
                                + static_cast<T>(0.5) * a_old[i]);
    }

    // 2. Compute new accelerations
    compute_accelerations(planets);

    // 3. Update velocities
    for (size_t i = 0; i < planets.size(); ++i) {
        planets[i].set_velocity(planets[i].get_velocity() 
                                + static_cast<T>(0.5) * (a_old[i] + planets[i].get_acceleration()));
    }
}

// Initialize two planets in an elliptical orbit using vis-viva equation
template<typename T>
void init_elliptical_orbit_visviva(PlanetProperties<T>& p1,
                                   PlanetProperties<T>& p2,
                                   T a, T e,
                                   T true_anomaly = 0)
{
    T m1 = p1.get_mass();
    T m2 = p2.get_mass();

    // Initial distance on ellipse
    T r_mag = a * (1 - e*e) / (1 + e * cos(true_anomaly));

    // Place planets along x-axis
    p1.set_position(glm::tvec3<T>(-m2/(m1+m2)*r_mag, 0, 0));
    p2.set_position(glm::tvec3<T>( m1/(m1+m2)*r_mag, 0, 0));

    // Velocity magnitude (vis-viva)
    T v_mag = std::sqrt(G<T> * (m1 + m2) * (static_cast<T>(2)/r_mag - static_cast<T>(1)/a));

    glm::tvec3<T> r_vec = p2.get_position() - p1.get_position();
    glm::tvec3<T> r_hat = glm::normalize(r_vec);
    glm::tvec3<T> theta_hat(-r_hat.y, r_hat.x, static_cast<T>(0)); // perpendicular in xy-plane

    glm::tvec3<T> v_vec = v_mag * theta_hat;

    // Set velocities relative to center of mass
    p1.set_velocity(-m2/(m1+m2) * v_vec);
    p2.set_velocity( m1/(m1+m2) * v_vec);

    // Compute initial accelerations
    glm::tvec3<T> r12 = p2.get_position() - p1.get_position();
    T dist3 = glm::length(r12) * glm::length(r12) * glm::length(r12) + static_cast<T>(1e-6);

    glm::tvec3<T> a1 = G<T> * m2 * r12 / dist3;
    glm::tvec3<T> a2 = -G<T> * m1 * r12 / dist3;

    p1.set_acceleration(a1);
    p2.set_acceleration(a2);
}
