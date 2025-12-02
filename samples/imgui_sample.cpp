#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Shader sources
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"}\n";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
"}\n";

// Shared triangle vertices
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

// global for both windows
GLuint shaderProgram;

// Per-window VAOs (because VAOs are NOT shared)
GLuint mainVAO, mainVBO;
GLuint figureVAO, figureVBO;

// ---------- HELPER: Compile shader ----------
GLuint compileShader(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);

    GLint success;
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(s, 512, NULL, log);
        std::cout << "Shader compile error:\n" << log << std::endl;
    }

    return s;
}

// ---------- HELPER: Create VAO for current context ----------
void createVAO(GLuint& VAO, GLuint& VBO)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

int main()
{
    glfwInit();

    // OpenGL 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Main Window", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Load GL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD\n";
        return 0;
    }

    // ---- SHADERS ----
    GLuint vert = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);

    glDeleteShader(vert);
    glDeleteShader(frag);

    // Create VAO for MAIN CONTEXT
    createVAO(mainVAO, mainVBO);

    // ----- IMGUI INIT -----
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Figure window pointer
    GLFWwindow* figureWindow = nullptr;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // ---------- IMGUI FRAME ----------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Test GUI");
        if (ImGui::Button("Show Figure"))
        {
            if (!figureWindow)
            {
                figureWindow = glfwCreateWindow(500, 500, "Figure", NULL, window);

                if (figureWindow)
                {
                    // Must create VAO inside its OWN context
                    glfwMakeContextCurrent(figureWindow);
                    createVAO(figureVAO, figureVBO);
                    glfwMakeContextCurrent(window);
                }
            }
        }
        ImGui::End();


        // --------- RENDER IMGUI ON MAIN WINDOW ----------
        ImGui::Render();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.45, 0.55, 0.60, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);


        // ---------- RENDER FIGURE WINDOW ----------
        if (figureWindow)
        {
            if (glfwWindowShouldClose(figureWindow))
            {
                glfwDestroyWindow(figureWindow);
                figureWindow = nullptr;
            }
            else
            {
                glfwMakeContextCurrent(figureWindow);

                int fw, fh;
                glfwGetFramebufferSize(figureWindow, &fw, &fh);
                glViewport(0, 0, fw, fh);

                glClearColor(0.2, 0.3, 0.3, 1.0);
                glClear(GL_COLOR_BUFFER_BIT);

                glUseProgram(shaderProgram);
                glBindVertexArray(figureVAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);

                glfwSwapBuffers(figureWindow);

                // Return to main context
                glfwMakeContextCurrent(window);
            }
        }
    }

    // Cleanup
    glDeleteVertexArrays(1, &mainVAO);
    glDeleteBuffers(1, &mainVBO);

    if (figureWindow)
    {
        glDeleteVertexArrays(1, &figureVAO);
        glDeleteBuffers(1, &figureVBO);
        glfwDestroyWindow(figureWindow);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
