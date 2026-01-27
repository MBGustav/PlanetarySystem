#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <PlanetProperties.hpp>
#include <vector>

using std::vector;

// === Camera variables ===
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 1; // our delta time is fixed to one day
float lastFrame = 0.0f;
static float G = 2.96e-4;


// === Input handling ===
void processInput(GLFWwindow *window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos.y += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos.y -= cameraSpeed;
}

// === Simple sphere vertex data ===
// (for simplicity we just draw small cubes to represent planets)
float vertices[] = {
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,0.5f,-0.5f,  0.5f,0.5f,-0.5f, -0.5f,0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f,0.5f, 0.5f,  0.5f,0.5f, 0.5f, -0.5f,0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
    -0.5f,0.5f, 0.5f,  -0.5f,0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,-0.5f, -0.5f,-0.5f,0.5f, -0.5f,0.5f,0.5f,
     0.5f,0.5f, 0.5f,   0.5f,0.5f,-0.5f,   0.5f,-0.5f,-0.5f,   0.5f,-0.5f,-0.5f,  0.5f,-0.5f,0.5f,  0.5f,0.5f,0.5f,
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f,0.5f,  0.5f,-0.5f,0.5f, -0.5f,-0.5f,0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f,0.5f,-0.5f,   0.5f,0.5f,-0.5f,   0.5f,0.5f,0.5f,   0.5f,0.5f,0.5f,  -0.5f,0.5f,0.5f,  -0.5f,0.5f,-0.5f
};

// === Simple shader sources ===
const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
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

// === Helper: compile shader ===
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compile error: " << infoLog << std::endl;
    }
    return shader;
}

/*void apply_newton_law(std::vector<PlanetProperties>& planets)
{
    if((planets[0].get_position()-planets[1].get_position()).length() > 1e-2f){
        // Use references (NO copies)
        PlanetProperties& p1 = planets[0];
        PlanetProperties& p2 = planets[1];

        // === 1. Update positions (Velocity Verlet) ===
        p1.set_position(
            p1.get_position() +
            p1.get_velocity() +
            0.5f * p1.get_acceleration()
        );

        p2.set_position(
            p2.get_position() +
            p2.get_velocity() +
            0.5f * p2.get_acceleration()
        );

        // === 2. Compute new accelerations (Newtonian gravity) ===
        glm::vec3 r12 = p2.get_position() - p1.get_position();
        float dist = glm::length(r12);

        // (Optional but recommended: avoid singularity)
        float dist3 = dist * dist * dist + 1e-6f;

        glm::vec3 a1_new = G * p2.get_mass() * r12 / dist3;
        glm::vec3 a2_new = -G * p1.get_mass() * r12 / dist3;

        // === 3. Update velocities ===
        p1.set_velocity(
            p1.get_velocity() +
            0.5f * (p1.get_acceleration() + a1_new)
        );

        p2.set_velocity(
            p2.get_velocity() +
            0.5f * (p2.get_acceleration() + a2_new)
        );

        // === 4. Store new accelerations ===
        p1.set_acceleration(a1_new);
        p2.set_acceleration(a2_new);
    }
} */


void compute_accelerations(std::vector<PlanetProperties>& planets) {
    size_t N = planets.size();

    // Reset accelerations
    for (auto& p : planets) p.set_acceleration(glm::vec3(0.0f));

    // Pairwise gravitational forces
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            if (i == j) continue;
            glm::vec3 r_ij = planets[j].get_position() - planets[i].get_position();
            float dist3 = glm::length(r_ij);
            dist3 = dist3 * dist3 * dist3 + 1e-6f;
            planets[i].set_acceleration(planets[i].get_acceleration() + 
                                        G * planets[j].get_mass() * r_ij / dist3);
        }
    }
}

void velocity_verlet(std::vector<PlanetProperties>& planets, float dt) {
    size_t N = planets.size();

    // 1. Update positions
    for (auto& p : planets) {
        p.set_position(p.get_position() + p.get_velocity() * dt + 0.5f * p.get_acceleration() * dt*dt);
    }

    // 2. Store old accelerations
    std::vector<glm::vec3> old_acc(N);
    for (size_t i = 0; i < N; ++i) old_acc[i] = planets[i].get_acceleration();

    // 3. Compute new accelerations
    compute_accelerations(planets);

    // 4. Update velocities
    for (size_t i = 0; i < N; ++i) {
        planets[i].set_velocity(planets[i].get_velocity() + 0.5f * (old_acc[i] + planets[i].get_acceleration()) * dt);
    }
}

glm::vec3 compute_center_of_mass(const std::vector<PlanetProperties>& planets) {
    glm::vec3 COM(0.0f);
    float total_mass = 0.0f;
    for (const auto& p : planets) {
        COM += p.get_mass() * p.get_position();
        total_mass += p.get_mass();
    }
    return COM / total_mass;
}

void init_orbital_velocities(std::vector<PlanetProperties>& planets) {
    glm::vec3 COM = compute_center_of_mass(planets);

    // Total mass excluding each planet (for simple approximation)
    float total_mass = 0.0f;
    for (auto& p : planets) total_mass += p.get_mass();

    for (auto& p : planets) {
        glm::vec3 r = p.get_position() - COM;       // radius vector from COM
        float r_mag = glm::length(r);
        if (r_mag < 1e-6f) continue;               // skip COM itself

        // Tangent vector perpendicular to r (simple 2D approach in XY plane)
        glm::vec3 tangent(-r.y, r.x, 0.0f);        // perpendicular in XY
        tangent = glm::normalize(tangent);

        // Velocity magnitude using vis-viva (circular orbit)
        float v_mag = sqrt(G * (total_mass - p.get_mass()) / r_mag);

        // Set initial velocity
        p.set_velocity(tangent * v_mag);
    }
}




int main() {
    // === Init GLFW ===
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Two Planets", NULL, NULL);
    if (!window) { std::cerr << "Failed to create window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    // === Build shader ===
    GLuint vShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    // === VAO / VBO setup ===
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    vector<PlanetProperties> planets;
    planets.emplace_back(glm::vec3(-1,0,0), glm::vec3(0),glm::vec3(0), 0.1f, 3.0f, glm::vec3(1,0,0), "P1");
    planets.emplace_back(glm::vec3( 1,0,0), glm::vec3(0),glm::vec3(0), 0.05f, 1.0f, glm::vec3(0,1,0), "P2");
    planets.emplace_back(glm::vec3( 0,1,0), glm::vec3(0),glm::vec3(0), 0.07f, 2.0f, glm::vec3(0,0,1), "P3");

    // Compute initial velocities
    init_orbital_velocities(planets);
    // Compute initial accelerations
    compute_accelerations(planets);


    float deltaTime = 1; // our delta time is fixed to one day

    // === Main loop ===
    while (!glfwWindowShouldClose(window)) {
       

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

        //Update position of each planet
        //apply_newton_law(planets);
        velocity_verlet(planets, deltaTime);



        // Draw both "planets"
        for (int i = 0; i < 3; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), planets[i].get_position());
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glm::vec3 color = (i == 0) ? glm::vec3(0.2f, 0.6f, 1.0f) : glm::vec3(1.0f, 0.8f, 0.2f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::vec3 pos = planets[i].get_position();
            std::cout << "Planet Pos: (" << pos.x << ", "<< pos.y<< ", " << pos.z <<")\n";
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
