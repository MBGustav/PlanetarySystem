#pragma once

#include "ImguiLayer.hpp"
#include "imgui.h"

#include "ImguiLayer.hpp"
#include "SimulationWrapper.hpp"
#include "GraphicsWrapper.hpp"
#include "Logger.hpp"
#include "PlanetProperties.hpp"
// imgui.h is included inside ImguiLayer.hpp, but can be included here if needed
// #include "imgui.h" 

// using namespace graphics;



class Application
{
    private:
    // Core application
    // UILayer - imgui
    gui::ImguiLayer uiLayer;

    
    // Graphics -- for the simulation
    graphics::GraphicsWrapper gfx;

    // Simulation -- just calc and maths
    SimulationWrapper simWrapper;

    //Debug / Logger
    
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
    
    // Setup Graph Layer
    gfx.SetupShader();
    gfx.SetupBuffers();
    // gfx.SetupFramebuffer(400, 300);
    
    // Simulation Windows must comunicate with layer
    
    // Create some default planets for testing
}   

inline Application::Application()
{
    uiLayer = gui::ImguiLayer(gfx, simWrapper);
    this->setup();
    sys_logger.info("Application constructor");
}

inline Application::~Application()
{
}

void Application::run()
{
    sys_logger.debug("Starting main application loop");
    while (!uiLayer.ShouldClose()) 
    {
        
        // 1. Poll Inputs
        sys_logger.debug("Polling events");
        uiLayer.processEvents();
        
        glEnable(GL_DEPTH_TEST);       // enable depth testing
        
        sys_logger.debug("Rendering Framebuffer");
        gfx.RenderToFramebuffer(simWrapper.getPlanets());

        //changing planets positions
        std::vector<PlanetProperties<float>> planets= simWrapper.getPlanets();
        for (std::vector<PlanetProperties<float>>::iterator planet=planets.begin();planet!=planets.end();planet++){
            (*planet).set_position((*planet).get_position()+0.5f);
        }

        
        // 2. Start Frame
        sys_logger.debug("Starting frame");
        uiLayer.beginFrame();
        
        // 3. Update Simulation Logic (Maths/Physics)
        
        // 4. Render Graphics
        // 4.1 set constants for rendering
        // ViewportRequest vpRequest;
        // vpRequest = uiLayer.getRenderProp();
        
        // 4.2 render scene
        // gfx.render();
        // gfx.RenderScene(800.0f, 600.0f);

        
        // 4. Draw UI
        uiLayer.drawUI();
        sys_logger.debug("UI drawn");
        // 5. Render & Swap Buffers
        uiLayer.endFrame();
        sys_logger.debug("Frame ended");
    }
    uiLayer.clean_up();
}
