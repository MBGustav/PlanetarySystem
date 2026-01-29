#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "CelestialObject.hpp"
#include "Camera.hpp"
#include <glad/glad.h>

// CelestialObjectProperties adds extra features (name, rendering) to CelestialObject


template<typename T>
class CelestialObjectProperties : public CelestialObject<T>
{
private:
    std::string name;

public:
    // Default constructor
    CelestialObjectProperties()
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

    // Constructor copy of CelestialObject
    CelestialObjectProperties(const CelestialObject<T>& planet, const std::string& name = "Unnamed")
        : CelestialObjectProperties(planet.get_position(),
             planet.get_velocity(),
             planet.get_acceleration(),
             planet.get_radius(),
             planet.get_mass(),
             planet.get_color(),
             name){}

    // Full constructor
    CelestialObjectProperties(
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

    void set_name(const std::string& name) { this->name = name; }

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


    friend std::ostream& operator<<(std::ostream& os, const CelestialObjectProperties<T>& CelestialObject);


    ~CelestialObjectProperties() = default;
};


template<typename T>
std::ostream& operator<<(std::ostream& os, const CelestialObjectProperties<T>& CelestialObject)
{
    auto mass  = CelestialObject.get_mass();
    auto pos   = CelestialObject.get_position();
    auto vel   = CelestialObject.get_velocity();
    auto acc   = CelestialObject.get_acceleration();
    auto color = CelestialObject.get_color(); 
    auto force = CelestialObject.get_force(); 


    os << "CelestialObject Informations: \n";
    os <<"name:                " << CelestialObject.get_name()         << std::endl;
    os <<"mass:                " << CelestialObject.get_mass()         << std::endl;
    os <<"radius:              " << CelestialObject.get_radius()       << std::endl;
    os <<"Is fixed:            " << CelestialObject.is_fixed()         << std::endl;
    os <<"Color:               " << color.x << ", " << color.y << ", " << color.z << std::endl;
    os <<"force:               " << force.x << ", " << force.y << ", " << force.z << std::endl;
    os <<"position(x,y,z):     " << pos.x   << ", " << pos.y   << ", " << pos.z << std::endl;
    os <<"velocity(x,y,z):     " << vel.x   << ", " << vel.y   << ", " << vel.z << std::endl;
    os <<"acceleration(x,y,z): " << acc.x   << ", " << acc.y   << ", " << acc.z << std::endl;

    return os;
}