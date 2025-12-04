#pragma once

#include "ImguiLayer.hpp"
#include "imgui.h"
#pragma once

#include "ImguiLayer.hpp"
#include "SimulationWrapper.hpp"

// imgui.h is included inside ImguiLayer.hpp, but can be included here if needed
// #include "imgui.h" 

class Application
{
    private:
    // Core application
    // UILayer - imgui
    gui::ImguiLayer uiLayer;
    
    // Graphics -- for the simulation
    

    // Simulation -- just calc and maths
    SimulationWrapper simWrapper;
    
    public:
    
    Application();
    ~Application();
    void setup();
    void run();
};

// Initialization of many components used
inline void Application::setup()
{
    // Setup ImGui Layer
    uiLayer.setup();
    
    // Setup Graphics Layer
    
    // Setup Simulation Layer
    

}   

inline Application::Application()
{
    this->setup();
}

inline Application::~Application()
{
}

inline void Application::run()
{
    // Main application loop controlled by Application
    while (!uiLayer.ShouldClose()) {
        
        // 1. Poll Inputs
        uiLayer.processEvents();
        
        // 2. Start Frame
        uiLayer.beginFrame();
        
        // 3. Update Simulation Logic (Maths/Physics)
        // ... simulation code ...
        
        // 4. Draw UI
        uiLayer.drawUI();
        
        // 5. Render & Swap Buffers
        uiLayer.endFrame();
    }
    uiLayer.clean_up();
}
