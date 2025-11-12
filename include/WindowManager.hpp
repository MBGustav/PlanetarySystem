
#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <glad/glad.h>
#include <iostream>


class WindowManager {
public:
    WindowManager(int width, int height, const std::string& title);
    ~WindowManager();

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    GLFWwindow* getHandle() const; // para passar ao Camera, etc.
    float getAspectRatio();

private:
    GLFWwindow* p_window;
    int width, height;
    int get_width() const { return width;  }
    int get_height() const { return height; }
};


WindowManager::WindowManager(int width, int height, const std::string& title)
    : width(width), height(height)
{
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW\n";
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    p_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!p_window) {
        std::cerr << "Erro ao criar janela GLFW\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(p_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao carregar GLAD\n";
        std::exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
    // std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";
}

WindowManager::~WindowManager() {
    glfwDestroyWindow(p_window);
    glfwTerminate();
}

bool WindowManager::shouldClose() const {
    return glfwWindowShouldClose(p_window);
}

void WindowManager::swapBuffers() {
    glfwSwapBuffers(p_window);
}

void WindowManager::pollEvents() {
    glfwPollEvents();
}

GLFWwindow* WindowManager::getHandle() const {
    return p_window;
}


float WindowManager::getAspectRatio(){
    return static_cast<float>(get_width()) / static_cast<float>(get_height());
}
