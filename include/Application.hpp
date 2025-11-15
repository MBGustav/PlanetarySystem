#pragma once

#include "ImguiLayer.hpp"
#include "imgui.h"

class Application
{
private:
    // Core application
    // UILayer - imgui
    gui::ImguiLayer uiLayer;
    void setupImGui();
    void renderImGui();

    // Graphics -- for the simulation
    // Simulation -- just calc and maths


public:
    Application();
    ~Application();
    void setup();
    void run();
};

// Initialization of many components used
void Application::setup()
{
    // Setup ImGui Layer
    uiLayer.setup();

}   

Application::Application()
{
    this->setup();
}

Application::~Application()
{
}

void Application::run()
{
    uiLayer.run();
}