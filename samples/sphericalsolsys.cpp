#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include <PlanetProperties.hpp>
#include <Physics.hpp>

using std::vector;

// ================= CAMERA =================
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 60.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 1.0f;

// ================= INPUT =================
void processInput(GLFWwindow* window) {
    float speed = 20.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
}

// ================= SHADERS =================
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 color;
void main() {
    FragColor = vec4(color, 1.0);
}
)";

// ================= SHADER UTILS =================
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cerr << "Shader compile error: " << log << std::endl;
    }
    return shader;
}

// ================= SPHERE GENERATOR =================
void generateSphere(float radius, int stacks, int sectors,
                    vector<float>& vertices,
                    vector<unsigned int>& indices)
{
    const float PI = 3.1415926f;

    for (int i = 0; i <= stacks; ++i) {
        float phi = PI * i / stacks;
        float y = cos(phi);
        float r = sin(phi);

        for (int j = 0; j <= sectors; ++j) {
            float theta = 2 * PI * j / sectors;
            vertices.push_back(radius * r * cos(theta));
            vertices.push_back(radius * y);
            vertices.push_back(radius * r * sin(theta));
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first  = i * (sectors + 1) + j;
            int second = first + sectors + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

// ================= MAIN =================
int main() {
    // ---- GLFW Init ----
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(900,700,"Spherical Solar System",NULL,NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    // ---- Shader program ----
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // ---- Sphere mesh ----
    vector<float> vertices;
    vector<unsigned int> indices;
    generateSphere(1.0f, 32, 32, vertices, indices);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    // ---- Planets ----
    vector<CelestialObjectProperties<float>> planets = {
        { {0,0,0}, {0,0,0}, {0,0,0}, 0.0004f, 1.0f, {1.0f, 0.8f, 0.2f}, "Sun" },
        { {0,0,0}, {0,0,0}, {0,0,0}, 1.63e-5f, 1.66e-7f,  {0.5f, 0.5f, 0.5f},  "Mercury" },
        { {0,0,0}, {0,0,0}, {0,0,0}, 4.05e-5f, 2.45e-6f,  {0.9f, 0.8f, 0.5f},  "Venus"   },
        { {0,0,0}, {0,0,0}, {0,0,0}, 4.26e-5f, 3.0e-6f,   {0.2f, 0.5f, 1.0f},  "Earth"   },
        { {0,0,0}, {0,0,0}, {0,0,0}, 2.27e-5f, 3.21e-7f,  {0.8f, 0.3f, 0.2f},  "Mars"    },
        { {0,0,0}, {0,0,0}, {0,0,0}, 0.000285f, 9.55e-4f,{0.9f, 0.6f, 0.3f},  "Jupiter" },
        { {0,0,0}, {0,0,0}, {0,0,0}, 0.000241f, 2.86e-4f,{0.9f, 0.8f, 0.6f},  "Saturn"  },
        { {0,0,0}, {0,0,0}, {0,0,0}, 0.000103f, 4.37e-5f,{0.4f, 0.8f, 0.8f},  "Uranus"  },
        { {0,0,0}, {0,0,0}, {0,0,0}, 0.000100f, 5.15e-5f,{0.2f, 0.3f, 0.9f},  "Neptune" }
        };


    float a[] = {0,0.387f,0.723f,1.0f,1.524f,5.203f,9.537f,19.191f,30.07f};
    float e[] = {0,0.205f,0.007f,0.017f,0.093f,0.048f,0.056f,0.046f,0.010f};

    for (size_t i = 1; i < planets.size(); ++i)
        init_elliptical_orbit_visviva(planets[0], planets[i], a[i], e[i]);

    compute_accelerations(planets);

    // ---- Scaling factors ----
    const float RADIUS_SCALE = 5000.0f;   // makes planets big enough to see
    const float DISTANCE_SCALE = 50.0f;   // increases orbital distances for visibility

    float radiusScaleSun = 50.0f;      // Sun is smaller visually
    float radiusScalePlanets = 2000.0f; // planets are larger than true, for visibility


    // ---- Main loop ----
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.05f,0.05f,0.1f,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        velocity_verlet(planets);

        glUseProgram(shaderProgram);

        glm::mat4 view = glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(45.0f),900.f/700.f,0.1f,500.f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),1,GL_FALSE,&proj[0][0]);

        const float DISTANCE_SCALE = 80.0f;   // 40–150 — tune depending how much you want to zoom out
        const float SUN_SIZE_SCALE = 25.0f;   // 15–60
        const float PLANET_SIZE_SCALE = 3500.0f;  // 2000–10000

                

        for (auto& p : planets) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), p.get_position());
            model = glm::scale(model, glm::vec3(p.get_radius()*1000.0f)); // scale for visibility
            /*float r = (p.get_name()=="Sun") ? p.get_radius()*radiusScaleSun : p.get_radius()*radiusScalePlanets;
            glm::vec3 pos = p.get_position() * DISTANCE_SCALE;
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::scale(model, glm::vec3(r));*/

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,&model[0][0]);
            glUniform3fv(glGetUniformLocation(shaderProgram,"color"),1,&p.get_color()[0]);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}