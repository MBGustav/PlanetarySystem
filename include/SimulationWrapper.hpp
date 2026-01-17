#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "PlanetProperties.hpp"



using std::vector;
using std::string;

class SimulationWrapper
{
private:
    
    std::vector<PlanetProperties> planets;
    float delta_time;

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
        
        if (frameTime > 0.25f) frameTime = 0.25f;

        // 2. Acumula o tempo real
        accumulator += frameTime * speedMultiplier;

        // 3. Consome o tempo em fatias fixas (Física estável)
        while (accumulator >= FIXED_DT) {
            StepPhysics(FIXED_DT);
            accumulator -= FIXED_DT;
        }
    }


void SimulationWrapper::StepPhysics(float deltatime) {
    // TO-DO: implement the simulation step calculations
    size_t i, j;
    const size_t n_planets = planets.size();
    // iterate over each planet:
    // (TODO: optimize with parallel processing)
    for(i =0; i < n_planets; i++) {
        PlanetProperties& p1 = planets[i];
        for(j =i + 1; j < n_planets; j++) 
        {
            
            PlanetProperties& p2 = planets[j];
            glm::vec3 force = p2.apply_newton_law(p1);
            p1.accumulateForce(-force);   
            p2.accumulateForce(+force); 
        }
    }

    // After all forces are calculated, update each planet
    for(auto &p : planets) p.update(static_cast<float>(deltatime));
}



