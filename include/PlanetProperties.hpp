#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "Planet.hpp"
#include "Camera.hpp"
#include <glad/glad.h>

// PLanetProperties adds extra features (name, rendering) to CelestialObject

template<typename T>
class PlanetProperties : public Planet<T>
{
private:
    std::string name;

public:
    // Default constructor
    PLanetProperties()
        : CelestialObject<T>(
              glm::tvec3<T>(0,0,0),
              glm::tvec3<T>(0,0,0),
              glm::tvec3<T>(0,0,0),
              static_cast<T>(1),
              static_cast<T>(1),
              glm::tvec3<T>(1,1,1)
          ),
          name("Unnamed")
    {}

    // Full constructor
    PLanetProperties(
        const glm::tvec3<T>& position,
        const glm::tvec3<T>& velocity,
        const glm::tvec3<T>& acceleration,
        T radius,
        T mass,
        const glm::tvec3<T>& color,
        const std::string& name
    )
    : CelestialObject<T>(position, velocity, acceleration, radius, mass, color),
      name(name)
    {}

    // Getter for name
    std::string get_name() const { return name; }

    // Convert to base CelestialObject<T>
    CelestialObject<T> toPlanet() const {
        return CelestialObject<T>(
            this->get_position(),
            this->get_velocity(),
            this->get_acceleration(),
            this->get_radius(),
            this->get_mass(),
            glm::tvec3<T>(1)  // default color
        );
    }

    ~PLanetProperties() = default;
};
