#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "PlanetProperties.hpp"

#include "SimulationFiles.hpp"

using std::vector;
using std::string;

class SimulationWrapper
{
    private:
    
    std::vector<PlanetProperties> planets;
    float delta_time;

    // Initial State from our simulation
    std::vector<PlanetProperties> InitialState;
    

    float accumulator = 0.0f;
    const float FIXED_DT = 0.005f; // 5ms (precisão alta para órbitas estáveis)
    
    
    bool bound_check(size_t index){return index < planets.size() && index >= 0;}
    
    public:
    SimulationWrapper(float delta_time = 0.01f);
    ~SimulationWrapper();
    
    void StepPhysics(float deltatime);
    bool addPlanet(const PlanetProperties& planet);
    size_t getPlanetCount() const;
    const std::vector<PlanetProperties>& getPlanets() const;
    const std::vector<PlanetProperties>& getcopyPlanets() const;
    const std::vector<PlanetProperties>& getPlanetProperties() const { return planets; }
    void UpdateSimulation(float frameTime, float speedMultiplier = 1.0f);
    
    void save_state();
    void reset_simulation();




    void setDeltaTime(float dt) { 
        if (dt <= 0.000001f) { 
            this->delta_time = 0.01f;
            return;
        }
        this->delta_time = dt; 
    }
    
    float getDeltaTime() const { return this->delta_time; }
    
    const std::vector<string>& getPlanetNames() const {
        static std::vector<string> names;
        names.clear();
        for (const auto& p : planets) {
            names.push_back(p.get_name());
        }
        return names;
    }
    
    void setPlanet_mass(size_t index, float mass) {
        if (!bound_check(index)) return;
        planets[index].set_mass(mass);
        
    }
    
    void setPlanet_radius(size_t index, float radius) {
        if (!bound_check(index)) return;
        planets[index].set_radius(radius);
        
    }
    
    
    void setPlanet_velocity(size_t index, const glm::vec3& velocity) {
        if (!bound_check(index)) return;
        planets[index].set_velocity(velocity);
    }
    
};

SimulationWrapper::SimulationWrapper(float delta_time) : delta_time(delta_time)
{
    planets.clear();
}

SimulationWrapper::~SimulationWrapper()
{
    planets.clear();
}


void SimulationWrapper::save_state(){
    InitialState = planets;
}

void SimulationWrapper::reset_simulation(){
    planets = InitialState;
}

bool SimulationWrapper::addPlanet(const PlanetProperties& planet) 
{
    try
    {
        planets.push_back(planet);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        // TO-DO: insert logging por windowViewer
        return false;
    }
    // TO-DO: insert logging por windowViewer
    return true;
}


size_t SimulationWrapper::getPlanetCount() const
{
    return planets.size();
}


const vector<PlanetProperties>& SimulationWrapper::getPlanets() const {
    return planets;
}

const vector<PlanetProperties>& SimulationWrapper::getcopyPlanets() const {
    std::vector<PlanetProperties> copy_planets;
    for(auto p : planets) {
        copy_planets.push_back(p);
    }
    return copy_planets;
}


void SimulationWrapper::UpdateSimulation(float frameTime, float speedMultiplier) {
    
    
    // Set constraint  
    if (frameTime > 0.25f) frameTime = 0.25f;
    
    // 2. Acumulate time
    accumulator += frameTime * speedMultiplier;
    
    // Integration
    while (accumulator >= FIXED_DT) {
        
        StepPhysics(FIXED_DT);
        for(auto& p : planets) p.update(FIXED_DT);
        accumulator -= FIXED_DT;
    }
}


void SimulationWrapper::StepPhysics(float deltatime) {
    const size_t n_planets = planets.size();
    
    // RESET FORCES, before start
    for(auto& p : planets) p.set_force(glm::vec3(0,0,0)); 
    
    // iterate over each planet:
    // (TODO: optimize with parallel processing)
    size_t n = planets.size();
    for(size_t i = 0; i < n; ++i) {
        for(size_t j = i + 1; j < n; ++j) {
            // Calcula força vetorial de atração
            glm::vec3 force = planets[i].apply_newton_law(planets[j]);
            
            // Aplica Lei de Newton (Ação e Reação)
            planets[i].accumulateForce(force);
            planets[j].accumulateForce(-force);
        }
    }
    
    // After all forces are calculated, update each planet
    for(auto &p : planets) p.update(static_cast<float>(deltatime));
}



