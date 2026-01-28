#pragma once

#include <glm/glm.hpp>
#include <string>

template<typename T>
static constexpr T G_CONSTANT = static_cast<T>(2.96e-4);

template<typename T>
class CelestialObject
{
private:
    glm::tvec3<T> position;
    glm::tvec3<T> velocity;
    glm::tvec3<T> acceleration;
    glm::tvec3<T> color;

    T mass;
    T radius;

public:
    // Constructor
    CelestialObject(
        const glm::tvec3<T>& position,
        const glm::tvec3<T>& velocity,
        const glm::tvec3<T>& acceleration,
        T radius,
        T mass,
        const glm::tvec3<T>& color
    );

    // Getters
    glm::tvec3<T> get_position()     const { return position; }
    glm::tvec3<T> get_velocity()     const { return velocity; }
    glm::tvec3<T> get_acceleration() const { return acceleration; }
    glm::tvec3<T> get_color()        const { return color; }
    T get_mass()                     const { return mass; }
    T get_radius()                   const { return radius; }

    // Setters
    void set_position(const glm::tvec3<T>& position)         { this->position = position; }
    void set_velocity(const glm::tvec3<T>& velocity)         { this->velocity = velocity; }
    void set_acceleration(const glm::tvec3<T>& acceleration) { this->acceleration = acceleration; }
    void set_color(const glm::tvec3<T>& color)               { this->color = color; }
    void set_mass(T mass)                                     { this->mass = mass; }
    void set_radius(T radius)                                 { this->radius = radius; }

    ~CelestialObject() = default;
};

// Constructor implementation
template<typename T>
CelestialObject<T>::CelestialObject(
    const glm::tvec3<T>& position,
    const glm::tvec3<T>& velocity,
    const glm::tvec3<T>& acceleration,
    T radius,
    T mass,
    const glm::tvec3<T>& color
) : position(position), velocity(velocity), acceleration(acceleration),
    color(color), mass(mass), radius(radius) {}
