#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <PlanetProperties.hpp>
#include <vector>
#include <Physics.hpp>

using std::vector;

// === Camera variables ===
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 60.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 1; // our delta time is fixed to one day
float lastFrame = 0.0f;
//static float G = 2.96e-4;


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


int main() {
    // === Init GLFW ===
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, " Solar System", NULL, NULL);
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

    //creating planets
    
    CelestialObjectProperties<float> sun(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                     0.00465f, 1.0f, glm::vec3(1,0.5,0), "Sun"); // radius, mass

    CelestialObjectProperties<float> mercury(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                            1.63e-5f, 1.66e-7f, glm::vec3(0.5,0.5,0.5), "Mercury");

    CelestialObjectProperties<float> venus(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                        4.05e-5f, 2.45e-6f, glm::vec3(1,0.8,0.2), "Venus");

    CelestialObjectProperties<float> earth(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                            4.26e-5f, 3.0e-6f, glm::vec3(0,0.5,1), "Earth");

    CelestialObjectProperties<float> mars(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                        2.27e-5f, 3.21e-7f, glm::vec3(1,0,0), "Mars");

    CelestialObjectProperties<float> jupiter(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                            0.000285f, 9.55e-4f, glm::vec3(1,0.5,0), "Jupiter");

    CelestialObjectProperties<float> saturn(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                            0.000241f, 2.86e-4f, glm::vec3(1,1,0), "Saturn");

    CelestialObjectProperties<float> uranus(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                            0.000103f, 4.37e-5f, glm::vec3(0,1,1), "Uranus");

    CelestialObjectProperties<float> neptune(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                            0.000100f, 5.15e-5f, glm::vec3(0,0,1), "Neptune");


    //initializing the orbit of each planet
    init_elliptical_orbit_visviva(sun, mercury, 0.387f, 0.205f);
    init_elliptical_orbit_visviva(sun, venus,   0.723f, 0.007f);
    init_elliptical_orbit_visviva(sun, earth,   1.0f,   0.017f);
    init_elliptical_orbit_visviva(sun, mars,    1.524f, 0.093f);
    init_elliptical_orbit_visviva(sun, jupiter, 5.203f, 0.048f);
    init_elliptical_orbit_visviva(sun, saturn,  9.537f, 0.056f);
    init_elliptical_orbit_visviva(sun, uranus, 19.191f, 0.046f);
    init_elliptical_orbit_visviva(sun, neptune, 30.07f, 0.010f);

    std::vector<CelestialObjectProperties<float>> planets = {
        sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune
    };


    std::vector<CelestialObjectProperties<float>> planet1 = {sun, mercury};
    std::vector<CelestialObjectProperties<float>> planet2 = {sun, venus};
    std::vector<CelestialObjectProperties<float>> planet3 = {sun, earth};
    std::vector<CelestialObjectProperties<float>> planet4 = {sun, mars};
    std::vector<CelestialObjectProperties<float>> planet5 = {sun, jupiter};
    std::vector<CelestialObjectProperties<float>> planet6 = {sun, saturn};
    std::vector<CelestialObjectProperties<float>> planet7 = {sun, uranus};
    std::vector<CelestialObjectProperties<float>> planet8 = {sun, neptune};

    //initializing the acceleration of each planet
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

        //Update position of each planet (we neglet the influenece of planets on each other 
        //since the sun has the biggest influence)

        velocity_verlet(planet1);
        velocity_verlet(planet2);
        velocity_verlet(planet3);
        velocity_verlet(planet4);
        velocity_verlet(planet5);
        velocity_verlet(planet6);
        velocity_verlet(planet7);
        velocity_verlet(planet8);


        //we draw the planets
        for (int i = 0; i < 2; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), planet1[i].get_position());
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glm::vec3 color = (i == 0) ? glm::vec3(0.2f, 0.6f, 1.0f) : glm::vec3(1.0f, 0.8f, 0.2f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::vec3 pos = planet1[i].get_position();
            std::cout << "Planet Pos: (" << pos.x << ", "<< pos.y<< ", " << pos.z <<")\n";
        }
        
        for (int i = 0; i < 2; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), planet2[i].get_position());
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glm::vec3 color = (i == 0) ? glm::vec3(0.2f, 0.6f, 1.0f) : glm::vec3(1.0f, 0.8f, 0.2f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::vec3 pos = planet2[i].get_position();
            std::cout << "Planet Pos: (" << pos.x << ", "<< pos.y<< ", " << pos.z <<")\n";
        }

        for (int i = 0; i < 2; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), planet3[i].get_position());
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glm::vec3 color = (i == 0) ? glm::vec3(0.2f, 0.6f, 1.0f) : glm::vec3(1.0f, 0.8f, 0.2f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::vec3 pos = planet3[i].get_position();
            std::cout << "Planet Pos: (" << pos.x << ", "<< pos.y<< ", " << pos.z <<")\n";
        }
        
        for (int i = 0; i < 2; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), planet4[i].get_position());
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glm::vec3 color = (i == 0) ? glm::vec3(0.2f, 0.6f, 1.0f) : glm::vec3(1.0f, 0.8f, 0.2f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::vec3 pos = planet4[i].get_position();
            std::cout << "Planet Pos: (" << pos.x << ", "<< pos.y<< ", " << pos.z <<")\n";
        }
        
        for (int i = 0; i < 2; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), planet5[i].get_position());
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glm::vec3 color = (i == 0) ? glm::vec3(0.2f, 0.6f, 1.0f) : glm::vec3(1.0f, 0.8f, 0.2f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::vec3 pos = planet5[i].get_position();
            std::cout << "Planet Pos: (" << pos.x << ", "<< pos.y<< ", " << pos.z <<")\n";
        }

        for (int i = 0; i < 2; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), planet6[i].get_position());
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glm::vec3 color = (i == 0) ? glm::vec3(0.2f, 0.6f, 1.0f) : glm::vec3(1.0f, 0.8f, 0.2f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::vec3 pos = planet6[i].get_position();
            std::cout << "Planet Pos: (" << pos.x << ", "<< pos.y<< ", " << pos.z <<")\n";
        }
        
        for (int i = 0; i < 2; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), planet7[i].get_position());
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glm::vec3 color = (i == 0) ? glm::vec3(0.2f, 0.6f, 1.0f) : glm::vec3(1.0f, 0.8f, 0.2f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::vec3 pos = planet7[i].get_position();
            std::cout << "Planet Pos: (" << pos.x << ", "<< pos.y<< ", " << pos.z <<")\n";
        }

        for (int i = 0; i < 2; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), planet8[i].get_position());
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glm::vec3 color = (i == 0) ? glm::vec3(0.2f, 0.6f, 1.0f) : glm::vec3(1.0f, 0.8f, 0.2f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glm::vec3 pos = planet8[i].get_position();
            std::cout << "Planet Pos: (" << pos.x << ", "<< pos.y<< ", " << pos.z <<")\n";
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}



    