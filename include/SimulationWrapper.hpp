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
    
    std::vector<PlanetProperties<float>> planets;
    
    
    
    public:
    SimulationWrapper();
    ~SimulationWrapper();
    
    void calculateIteration(float deltatime);
    bool addPlanet(const PlanetProperties<float>& planet);
    size_t getPlanetCount() const;
    const std::vector<PlanetProperties<float>>& getPlanets() const;
    const std::vector<PlanetProperties<float>>& getcopyPlanets() const;
    const vector<Planet<float>>& getRawPlanets() const;
    const std::vector<PlanetProperties<float>>& getPlanetProperties() const { return planets; }
    
};

SimulationWrapper::SimulationWrapper()
{
    planets.clear();
}

SimulationWrapper::~SimulationWrapper()
{
    planets.clear();
}


bool SimulationWrapper::addPlanet(const PlanetProperties<float>& planet) 
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


const vector<PlanetProperties<float>>& SimulationWrapper::getPlanets() const {
    return planets;
}

const vector<PlanetProperties<float>>& SimulationWrapper::getcopyPlanets() const {
    std::vector<PlanetProperties<float>> copy_planets;
    for(auto p : planets) {
        copy_planets.push_back(p);
    }
    return copy_planets;
}


/*void SimulationWrapper::calculateIteration(float deltatime) {
    // TO-DO: implement the simulation step calculations
    size_t i, j;
    const size_t n_planets = planets.size();
    // iterate over each planet:
    // (TODO: optimize with parallel processing)
    for(i =0; i < n_planets; i++) {
        PlanetProperties<float>& p1 = planets[i];
        for(j =i + 1; j < n_planets; j++) 
        {
            
            PlanetProperties<float>& p2 = planets[j];
            glm::vec3 force = p2.apply_newton_law(p1);
            p1.accumulateForce(+force);   
            p2.accumulateForce(-force); 
        }
    }

    // After all forces are calculated, update each planet
    for(auto p : planets) p.update(static_cast<float>(deltatime));
}*/



