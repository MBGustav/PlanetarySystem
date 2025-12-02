

#include <glm/glm.hpp>
#include <string>


class Planet
{
private:
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    float radius;
    
public:
    Planet(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 acceleration, float radius, float mass);

    
    glm::vec3 get_position()     const { return position; }
    glm::vec3 get_velocity()     const { return velocity; }
    glm::vec3 get_acceleration() const { return acceleration; }
    float get_mass()             const { return mass; }
    float get_radius()           const { return radius; }


    void update(float dt);
    void applyForce(const glm::vec3& force);

    float distance(const Planet &OtherPlanet) const;
    bool is_colision(const Planet &OtherPlanet);
    
    ~Planet();
};

Planet::Planet(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 acceleration, float radius, float mass):
    position(position), velocity(velocity), acceleration(acceleration), 
    mass(mass), radius(radius) {}

void Planet::update(float dt) {
    velocity += acceleration * dt;  // v = v + a*dt
    position += velocity * dt;      // p = p + v*dt
    acceleration = {0,0,0};          // zero acceleration after update ?
}


void Planet::applyForce(const glm::vec3& force) {
    acceleration += force / mass;
}

bool Planet::is_colision(const Planet &OtherPlanet)
{
        
}


float Planet::distance(const Planet &OtherPlanet) const{
    float val  = glm::length(OtherPlanet.get_position() - this->get_position());
    return val;
}

Planet::~Planet()
{
}
