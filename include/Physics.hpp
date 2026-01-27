#include <iostream>
#include <vector>
#include <PlanetProperties.hpp>
#include <vector>

using std::vector;

static float G = 2.96e-4;

void compute_accelerations(std::vector<PlanetProperties>& planets)
{
    // Reset
    for (auto& p : planets)
        p.set_acceleration(glm::vec3(0.0f));

    // Pairwise gravity
    for (size_t i = 0; i < planets.size(); ++i) {
        for (size_t j = i + 1; j < planets.size(); ++j) {

            glm::vec3 r = planets[j].get_position() - planets[i].get_position();
            float dist = glm::length(r);
            float dist3 = dist * dist * dist + 1e-6f;

            glm::vec3 acc = G * r / dist3;

            planets[i].set_acceleration(
                planets[i].get_acceleration() +
                planets[j].get_mass() * acc
            );

            planets[j].set_acceleration(
                planets[j].get_acceleration() -
                planets[i].get_mass() * acc
            );
        }
    }
}

void velocity_verlet(std::vector<PlanetProperties>& planets)
{
    // 0. Save old accelerations
    std::vector<glm::vec3> a_old(planets.size());
    for (size_t i = 0; i < planets.size(); ++i) {
        a_old[i] = planets[i].get_acceleration();
    }

    // 1. Update positions
    for (size_t i = 0; i < planets.size(); ++i) {
        planets[i].set_position(
            planets[i].get_position()
          + planets[i].get_velocity()
          + 0.5f * a_old[i]
        );
    }

    // 2. Compute new accelerations (a_{n+1})
    compute_accelerations(planets);

    // 3. Update velocities (uses BOTH a_n and a_{n+1})
    for (size_t i = 0; i < planets.size(); ++i) {
        planets[i].set_velocity(
            planets[i].get_velocity()
          + 0.5f * (a_old[i] + planets[i].get_acceleration())
        );
    }
}


// === Initialize two planets in an elliptical orbit ===
void init_elliptical_orbit_visviva(PlanetProperties& p1, PlanetProperties& p2, 
                                   float a, float e, float true_anomaly = 0.0f)
{
    float m1 = p1.get_mass();
    float m2 = p2.get_mass();

    // 1. Initial distance on the ellipse
    float r_mag = a * (1 - e*e) / (1 + e * cos(true_anomaly));

    // 2. Place planets along x-axis for simplicity
    p1.set_position(glm::vec3(-m2/(m1+m2)*r_mag, 0.0f, 0.0f));
    p2.set_position(glm::vec3( m1/(m1+m2)*r_mag, 0.0f, 0.0f));

    // 3. Vis-viva velocity magnitude at this position
    float v_mag = sqrt(G * (m1 + m2) * (2.0f/r_mag - 1.0f/a));

    // 4. Direction perpendicular to r (orbit in xy-plane)
    glm::vec3 r_vec = p2.get_position() - p1.get_position();
    glm::vec3 r_hat = glm::normalize(r_vec);
    glm::vec3 theta_hat = glm::vec3(-r_hat.y, r_hat.x, 0.0f); // perpendicular in xy-plane

    glm::vec3 v_vec = v_mag * theta_hat;

    // 5. Set velocities relative to center of mass
    p1.set_velocity(-m2/(m1+m2) * v_vec);
    p2.set_velocity( m1/(m1+m2) * v_vec);

    // 6. Compute initial accelerations
    glm::vec3 r12 = p2.get_position() - p1.get_position();
    float dist3 = glm::length(r12) * glm::length(r12) * glm::length(r12) + 1e-6f;

    glm::vec3 a1 = G * m2 * r12 / dist3;
    glm::vec3 a2 = -G * m1 * r12 / dist3;

    p1.set_acceleration(a1);
    p2.set_acceleration(a2);
}