#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "CelestialObjectProperties.hpp"

#include "SimulationFiles.hpp"
#include "Physics.hpp"



using std::vector;
using std::string;

template<typename T>
static constexpr T G_CONSTANT = static_cast<T>(2.96e-4);

// template<typename T>
class SimulationWrapper
{
    private:
    
    std::vector<CelestialObjectProperties<float>> planets;
    float delta_time;
    std::vector<CelestialObjectProperties<float>> InitialState;    
    float accumulator = 0.0f;
    const float FIXED_DT = 0.005f; // 5ms
    
    // Normalization factors
    // float mass_scale;
    // float distance_scale;
    // float g_normalized;
    
    void data_normalization();
    
    // Physics helpers
    void compute_accelerations();
    
    void velocity_verlet(float dt);
    
    void init_elliptical_orbit_visviva(CelestialObjectProperties<float>& p1, CelestialObjectProperties<float>& p2, float a, float e, float true_anomaly = 0.0f);
    
    void loadWithEccentricity(std::vector<CelestialObjectProperties<float>>& planets, const std::vector<float>& axis, const std::vector<float>& eccentricity, int planet_refId);
    
    bool bound_check(size_t idx) const {return idx < planets.size() && planets.size() > 0;}
    
    public:
    // Constructor & Destructor
    SimulationWrapper(vector<CelestialObjectProperties<float>> initial_planets,  vector<float> axis, vector<float> eccentricity, int planet_refId=0,float dt=0.01f);
    SimulationWrapper();
    
    ~SimulationWrapper();
    
    void reset_simulation();
    
    
    void setInitialState(std::vector<CelestialObjectProperties<float>> v_planets, bool reset_simulation);
    void setInitialState(PlanetJSONReader<float>& reader, bool reset_simulation);
    void UpdateSimulation(float frameTime, float speedMultiplier = 1.0f);
    const std::vector<string>& getPlanetNames() const;
    const std::vector<CelestialObjectProperties<float>>& getPlanets() const {return planets;}
    CelestialObjectProperties<float>& PlanetByIdx(size_t idx);
    
    
    float getDeltaTime() const  { return this->delta_time; }
    void setDeltaTime(float dt) { this->delta_time = dt; }
    void StepPhysics(float dt)  {velocity_verlet(dt);}
    
    size_t getPlanetCount() const { return planets.size();}
    
    // void initOrbitExample() {
    //     if (planets.size() >= 2) {
    //         init_elliptical_orbit_visviva(planets[0], planets[1], 100.0f, 0.1f);
    //     }
    // }
};

const std::vector<string>& SimulationWrapper::getPlanetNames() const 
{
    static std::vector<string> names;
    names.clear();
    for (const auto& p : planets) {
        names.push_back(p.get_name());
    }
    return names;
}


void SimulationWrapper::data_normalization()
{
    // obtain the maximum mass and distance
    float mass_scale = 0.0f;
    float distance_scale = 0.0f;
    
    for (const auto& p : InitialState) {
        mass_scale = std::max(mass_scale, p.get_mass());
        distance_scale = std::max(distance_scale, glm::length(p.get_position()));
    }
    
    // Avoid division by zero
    if (mass_scale == 0.0f) mass_scale = 1.0;
    if (distance_scale == 0.0f) distance_scale = 1.0;
    
    // Normalize masses and positions
    for (auto& p : InitialState) {
        p.set_mass(p.get_mass() / mass_scale);
        p.set_position(p.get_position() / distance_scale);
        p.set_velocity(p.get_velocity() / distance_scale);
    }
    
    // g_normalized = G_CONSTANT<float>;
}

void SimulationWrapper::setInitialState(std::vector<CelestialObjectProperties<float>> v_planets, bool reset_simulation)
{
    InitialState = v_planets;
    // data_normalization();
    if(reset_simulation){
        this->reset_simulation();
    }
    
}

void SimulationWrapper::setInitialState(PlanetJSONReader<float>& reader, bool reset_simulation)
{
    // initialize planets from reader
    InitialState = reader.get_planets();
    auto axis = reader.get_axis();
    auto eccentricity = reader.get_eccentricity();
    

    
    // check if eccentricity and axis sizes exists and match planet count
    if(axis.size() != InitialState.size() || eccentricity.size() != InitialState.size()){
        sys_logger.warn("Eccentricity or Axis size does not match planet count. Skipping normalization of orbits.");
    }
    else {
        loadWithEccentricity(InitialState, axis, eccentricity, 0);
    }
    
    if(reset_simulation){
        this->reset_simulation();
    }
    
    // show loaded planets
    for(const auto& p : InitialState){
        sys_logger.info("Loaded planet: " + p.get_name());
        sys_logger.info("  Position: (" + std::to_string(p.get_position().x) + ", " + std::to_string(p.get_position().y) + ", " + std::to_string(p.get_position().z) + ")");
    }
}


void SimulationWrapper::UpdateSimulation(float frameTime, float speedMultiplier) 
{
    
    if (frameTime > 0.25f) frameTime = 0.25f;
    accumulator += frameTime * speedMultiplier;
    
    while (accumulator >= FIXED_DT) {
        StepPhysics(FIXED_DT);
        accumulator -= FIXED_DT;
    }
}


void SimulationWrapper::compute_accelerations() 
{
    // Reset accelerations
    for (auto& p : planets)
    p.set_acceleration(glm::vec3(0));
    
    // Pairwise gravity
    size_t n = planets.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            glm::vec3 r = planets[j].get_position() - planets[i].get_position();
            float dist = glm::length(r);
            float dist3 = dist * dist * dist + 1e-6f;
            
            glm::vec3 acc = G_CONSTANT<float> * r / dist3;
            
            planets[i].set_acceleration(planets[i].get_acceleration() + planets[j].get_mass() * acc);
            planets[j].set_acceleration(planets[j].get_acceleration() - planets[i].get_mass() * acc);
        }
    }
}


void  SimulationWrapper::velocity_verlet(float dt)
{
    std::vector<glm::vec3> a_old(planets.size());
    for (size_t i = 0; i < planets.size(); ++i)
    a_old[i] = planets[i].get_acceleration();
    
    // Update positions
    for (size_t i = 0; i < planets.size(); ++i) {
        planets[i].set_position(planets[i].get_position()
        + planets[i].get_velocity() * dt
        + 0.5f * a_old[i] * dt * dt);
    }
    
    // Update accelerations
    compute_accelerations();
    
    // Update velocities
    for (size_t i = 0; i < planets.size(); ++i) {
        planets[i].set_velocity(planets[i].get_velocity()
        + 0.5f * (a_old[i] + planets[i].get_acceleration()) * dt);
    }
}


void SimulationWrapper::init_elliptical_orbit_visviva(CelestialObjectProperties<float>& p1, CelestialObjectProperties<float>& p2, float a, float e, float true_anomaly)
{
    float m1 = p1.get_mass();
    float m2 = p2.get_mass();
    
    float r_mag = a * (1 - e*e) / (1 + e * cos(true_anomaly));
    
    p1.set_position(glm::vec3(-m2/(m1+m2)*r_mag, 0, 0));
    p2.set_position(glm::vec3( m1/(m1+m2)*r_mag, 0, 0));
    
    float v_mag = sqrt(G_CONSTANT<float> * (m1 + m2) * (2/r_mag - 1/a));
    
    glm::vec3 r_vec = p2.get_position() - p1.get_position();
    glm::vec3 r_hat = glm::normalize(r_vec);
    glm::vec3 theta_hat(-r_hat.y, r_hat.x, 0);
    
    glm::vec3 v_vec = v_mag * theta_hat;
    
    p1.set_velocity(-m2/(m1+m2) * v_vec);
    p2.set_velocity( m1/(m1+m2) * v_vec);
    
    // initial accelerations
    glm::vec3 r12 = p2.get_position() - p1.get_position();
    float dist3 = pow(glm::length(r12), 3) + 1e-6f;
    
    p1.set_acceleration(G_CONSTANT<float> * m2 * r12 / dist3);
    p2.set_acceleration(-G_CONSTANT<float> * m1 * r12 / dist3);
}

CelestialObjectProperties<float>& SimulationWrapper::PlanetByIdx(size_t idx) 
{
    if(!bound_check(idx)) {
        throw std::out_of_range("PlanetByIdx: Index out of range");
    }
    return planets[idx];
}

void SimulationWrapper::reset_simulation()
{    
    // Set planets to initial state
    planets = InitialState;
    
    // Reset accumulator
    accumulator = 0.0f;
    
    sys_logger.simulation("Simulation restarted.");
    // compute_accelerations();
}



void SimulationWrapper::loadWithEccentricity(std::vector<CelestialObjectProperties<float>>& planets, const std::vector<float>& axis, const std::vector<float>& eccentricity, int planet_refId)
{
    for(int i=1;i<planets.size();i++){
        init_elliptical_orbit_visviva(planets[planet_refId], planets[i], axis[i], eccentricity[i]);
    }
}

SimulationWrapper::SimulationWrapper(vector<CelestialObjectProperties<float>> initial_planets,  vector<float> axis, vector<float> eccentricity, int planet_refId,float dt) :
delta_time(dt),
planets(initial_planets),
InitialState(initial_planets)
{
    try {
        if (!bound_check(planet_refId)) {
            sys_logger.warn("planet_refId is out of range, defaulting to 0");
            throw std::out_of_range("planet_refId is out of range, defaulting to 0");
        }
    } catch (const std::out_of_range& e) {
        std::cerr << e.what() << std::endl;
        planet_refId = 0;
    }
    
    
    loadWithEccentricity(InitialState, axis, eccentricity, planet_refId);
}
SimulationWrapper::~SimulationWrapper() {
    planets.clear();
    InitialState.clear();
}

SimulationWrapper::SimulationWrapper() : delta_time(0.01f)
{
    planets.clear();
    InitialState.clear();
    
}