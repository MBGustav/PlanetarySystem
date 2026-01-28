#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "Planet.hpp"
#include "Camera.hpp"
#include <glad/glad.h>

// PlanetProperties adds extra features (name, rendering) to Planet


template<typename T>
class PlanetProperties : public Planet<T>
{
private:
    std::string name;

public:
    // Default constructor
    PlanetProperties()
        : Planet<T>(
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
    PlanetProperties(
        const glm::tvec3<T>& position,
        const glm::tvec3<T>& velocity,
        const glm::tvec3<T>& acceleration,
        T radius,
        T mass,
        const glm::tvec3<T>& color,
        const std::string& name
    )
    : Planet<T>(position, velocity, acceleration, radius, mass, color),
      name(name)
    {}

    // Getter for name
    std::string get_name() const { return name; }

    void set_name(const std::string& name) { this->name = name; }

    // Convert to base Planet<T>
    Planet<T> toPlanet() const {
        return Planet<T>(
            this->get_position(),
            this->get_velocity(),
            this->get_acceleration(),
            this->get_radius(),
            this->get_mass(),
            glm::tvec3<T>(1)  // default color
        );
    }


    friend std::ostream& operator<<(std::ostream& os, const PlanetProperties<T>& Planet);


    ~PlanetProperties() = default;
};


template<typename T>
std::ostream& operator<<(std::ostream& os, const PlanetProperties<T>& Planet)
{
    auto mass  = Planet.get_mass();
    auto pos   = Planet.get_position();
    auto vel   = Planet.get_velocity();
    auto acc   = Planet.get_acceleration();
    auto color = Planet.get_color(); 
    auto force = Planet.get_force(); 


    os << "Planet Informations: \n";
    os <<"name:                " << Planet.get_name()         << std::endl;
    os <<"mass:                " << Planet.get_mass()         << std::endl;
    os <<"radius:              " << Planet.get_radius()       << std::endl;
    os <<"Is fixed:            " << Planet.is_fixed()         << std::endl;
    os <<"Color:               " << color.x << ", " << color.y << ", " << color.z << std::endl;
    os <<"force:               " << force.x << ", " << force.y << ", " << force.z << std::endl;
    os <<"position(x,y,z):     " << pos.x   << ", " << pos.y   << ", " << pos.z << std::endl;
    os <<"velocity(x,y,z):     " << vel.x   << ", " << vel.y   << ", " << vel.z << std::endl;
    os <<"acceleration(x,y,z): " << acc.x   << ", " << acc.y   << ", " << acc.z << std::endl;

    return os;
}