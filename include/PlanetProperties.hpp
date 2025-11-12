
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>


class PlanetProperties
{
private:
    // Planet attributes
    glm::vec3 position;
    float radius;
    std::string name;

    
public:
    PlanetProperties(const glm::vec3& position, float radius, std::string name);


    // void draw(unsigned int shaderProgram, unsigned int VAO) const;
    ~PlanetProperties();
};

PlanetProperties::PlanetProperties(const glm::vec3& position, float radius, std::string name)
    : position(position), radius(radius), name(name)
{
}

PlanetProperties::~PlanetProperties()
{
}
