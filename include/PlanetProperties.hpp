
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>


class PlanetProperties
{
private:
    // Planet attributes
    std::string name;
    glm::vec3 position;
    glm::vec3 color;
    float radius;

    
public:
    PlanetProperties(const glm::vec3 position, const glm::vec3 color, float radius, std::string name);
    glm::vec3 get_color() const { return color; }
    glm::vec3 get_position() const { return position; }
    float get_radius() const { return radius; }
    std::string get_name() const { return name; }
    
    void draw(Camera& camera, GLuint shaderProgram, GLuint VAO);
    ~PlanetProperties();
};

PlanetProperties::PlanetProperties(const glm::vec3 position, const glm::vec3 color, float radius, std::string name):
    position(position), radius(radius), name(name), color(color) {}

PlanetProperties::~PlanetProperties()
{
}

void PlanetProperties::draw(Camera& camera, GLuint shaderProgram, GLuint VAO)
{
    // Placeholder for drawing logic
    // In a real application, you would bind the appropriate VAO,
    // set up model matrices, and issue draw calls here.
    glm::mat4 projection = camera.perspective();//glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
    glm::mat4 view = camera.lookAt();//glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(0.5f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glm::vec3 color = get_color();
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}