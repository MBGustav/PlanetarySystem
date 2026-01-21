#pragma once

#include <glm/glm.hpp>
#include <string>

// TODO: add templated format ie. float, double, int ...

static float G_CONSTANT = 2.96e-4;
float M0 =0;
float t0=0;
class Planet
{
private:
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    //glm::vec3 force;
    glm::vec3 color;

    float mass;
    float radius;
    
public:
    Planet(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 acceleration, float radius, float mass, glm::vec3 color);

    
    glm::vec3 get_position()     const { return position; }
    glm::vec3 get_velocity()     const { return velocity; }
    glm::vec3 get_acceleration() const { return acceleration; }
    glm::vec3 get_color()        const { return color; }
    //glm::vec3 get_force()        const { return force; }
    float     get_mass()         const { return mass; }
    float     get_radius()       const { return radius; }

    void set_position(glm::vec3 position)           { this->position = position; }
    void set_velocity(glm::vec3 velocity)           { this->velocity = velocity; }
    void set_acceleration(glm::vec3 acceleration)   { this->acceleration = acceleration; }
    //void set_force(glm::vec3 force)                 { this->force = force; }
    void set_color(glm::vec3 color)                 { this->color = color; }
    void set_mass(float mass)                       { this->mass = mass; }
    void set_radius(float radius)                   { this->radius = radius; }

    void update(float dt);
    
    void accumulateForce(const glm::vec3& force);
    void apply_acceleration(Planet another_planet);

    float distance(const Planet &OtherPlanet) const;
    bool check_colision(const Planet &OtherPlanet);
    
    glm::vec3 apply_newton_law(const Planet &OtherPlanet);
    void elastic_colision(Planet &OtherPlanet);

    ~Planet();
};

Planet::Planet(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 acceleration, float radius, float mass, glm::vec3 color):
    position(position), velocity(velocity), acceleration(acceleration), color(color),
    mass(mass), radius(radius) {}

void Planet::update(float dt) {
    
    if(dt <= 0.0f) return;  
    //apply_acceleration(force/10.0f);


    velocity += acceleration * dt;  // v = v + a*dt
    position += velocity * dt;      // p = p + v*dt
    
    // acceleration = {0,0,0};      // zero acceleration after update ?

    //set_force({0,0,0});             // zero force after update
}


void Planet::apply_acceleration(Planet another_planet) {
    glm::vec3 rvect= another_planet.get_position() - position;
    float rmag=this->distance(another_planet);
    acceleration=(G_CONSTANT*(another_planet.mass)/rmag)*rvect ;
}

void Planet::accumulateForce(const glm::vec3& ext_force) {
    //force += ext_force;
}

void Planet::elastic_colision(Planet &A) {
    // Placeholder for elastic collision logic
    // In a real application, you would calculate the new velocities
    // based on conservation of momentum and kinetic energy.

    glm::vec3 normal = glm::normalize(A.get_position() - this->get_position());

    glm::vec3 r_vel = this->get_velocity() - A.get_velocity();

    glm::vec3 v_normal = glm::dot(r_vel, normal) * normal;

    // Update velocities
    A.set_velocity(A.get_velocity() + v_normal);
    set_velocity(get_velocity() - v_normal);


}

bool Planet::check_colision(const Planet &OtherPlanet)
{
    float dist = this->distance(OtherPlanet);
    if (dist <= (this->get_radius() + OtherPlanet.get_radius())) {
        return true;
    }
    return false;
}


float Planet::distance(const Planet &OtherPlanet) const{
    float val  = glm::length(OtherPlanet.get_position() - this->get_position());
    return val;
}


glm::vec3 Planet::apply_newton_law(const Planet &OtherPlanet)
{
    glm::vec3 direction = OtherPlanet.get_position() - this->get_position();
    float dist = glm::length(direction);
    // dist = dist < 0.000001f ? 0.000001f : dist; // prevent division by zero

    direction = glm::normalize(direction);
    
    float forceMagnitude = (G_CONSTANT * this->get_mass() * OtherPlanet.get_mass()) / (dist * dist);

    // this->check_colision(OtherPlanet);

    return forceMagnitude * direction;
}


Planet::~Planet()
{
}
