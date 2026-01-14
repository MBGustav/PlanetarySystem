#pragma once

#include "ImguiLayer.hpp"
#include "imgui.h"

#include "ImguiLayer.hpp"
#include "SimulationWrapper.hpp"
#include "GraphicsWrapper.hpp"
#include "Logger.hpp"

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
    
    PlanetProperties sunProps;
    float sunMass = 1000.0f;
    sunProps.set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    sunProps.set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
    sunProps.set_radius(1.5f);
    sunProps.set_mass(sunMass);
    sunProps.set_color(glm::vec3(255.0f, 255.0f, 255.0f));
    sunProps.set_name("Sun");
    sunProps.set_fixed(true); // Sun is fixed in space


    
    float orbit_radius = 5.0f;
    float orbit_speed = sqrt((G_CONSTANT * sunMass) / orbit_radius ); // simplified orbital speed formula


    PlanetProperties earthProps;
    earthProps.set_position(glm::vec3(0.0f, orbit_radius, 0.0f));
    earthProps.set_velocity(glm::vec3(0.0f, 0.0f, orbit_speed));
    earthProps.set_radius(0.5f);
    earthProps.set_mass(1.0f);
    earthProps.set_name("Earth");
    
    orbit_radius = 15.0f;
    orbit_speed = sqrt((G_CONSTANT * sunMass) / orbit_radius ); // simplified orbital speed formula

    PlanetProperties marsProps;
    marsProps.set_position(glm::vec3(0.0f, orbit_radius, 0.0f));
    marsProps.set_velocity(glm::vec3(0.0f, 0.0f, orbit_speed));
    marsProps.set_radius(0.3f);
    marsProps.set_mass(1.0f);
    marsProps.set_color(glm::vec3(1.0f, 0.0f, 3.0f));
    marsProps.set_name("Mars");

    orbit_radius = 20.0f;
    orbit_speed = sqrt((G_CONSTANT * sunMass) / orbit_radius ); // simplified orbital speed formula


    // PlanetProperties JupyterProps;
    // JupyterProps.set_position({0.5f, orbit_radius, 0.0f});
    // JupyterProps.set_velocity({0.0f, 0.00f, orbit_speed});
    // JupyterProps.set_radius(0.3f);
    // JupyterProps.set_mass(5.0f);
    // JupyterProps.set_color({20.0f, 0.0f, 3.0f});
    // JupyterProps.set_name("Jupiter");


    // simWrapper.addPlanet(JupyterProps);
    simWrapper.addPlanet(sunProps);
    simWrapper.addPlanet(earthProps);
    simWrapper.addPlanet(marsProps);

    sys_logger.debug("Total planets in simulation: " + std::to_string(simWrapper.getPlanetCount()));
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
        sys_logger.info("UI drawn");
        // 5. Render & Swap Buffers
        uiLayer.endFrame();
        sys_logger.info("Frame ended");
    }
    uiLayer.clean_up();
}
