#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include "PlanetProperties.hpp"
#include "PlanetJSONReader.hpp"
#include <set>

using std::string, std::vector, std::map, std::set;
namespace fs = std::filesystem;

#define SIMULATION_FOLDER "simulations/"
static const string SIMULATION_PATH = string("../" + string(SIMULATION_FOLDER)); 


namespace fs = std::filesystem;

// Definimos o caminho. Dica: "../" pode ser perigoso dependendo de onde você roda o executável.
// O ideal é ser relativo ao binário ou usar caminhos absolutos em debug.

class SimulationFiles
{
    private:
    std::string basePath;
    
    public:
    // Construtor define o caminho base, se não for passado usa o padrão
    SimulationFiles(std::string path = SIMULATION_FOLDER) {
        // Ajusta o caminho para ser relativo ao diretório anterior conforme seu código original
        basePath = "../" + path; 
    }
    
    ~SimulationFiles() = default;
    
    // Retorna apenas os nomes dos arquivos (ex: "solar_system.json")
    std::vector<std::string> availableSimulations() const;
    
    // Carrega uma simulação específica baseada no nome escolhido
    std::vector<PlanetProperties<float>> loadSimulation(const std::string& filename) const;
    
    
    
    friend std::ostream& operator<<(std::ostream& os, const SimulationFiles& sim);
};



std::vector<std::string> SimulationFiles::availableSimulations() const {
    std::vector<std::string> simulations;
    
    if (!fs::exists(SIMULATION_PATH)) {
        sys_logger.error("File directory not found!");
        return simulations;
    }
    
    for (const auto& entry : fs::directory_iterator(SIMULATION_PATH)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            simulations.push_back(entry.path().filename().string());
        }
    }
    
    return simulations;
}


std::vector<PlanetProperties<float>> SimulationFiles::loadSimulation(const std::string& filename) const {
    
    fs::path fullPath = fs::path(SIMULATION_PATH) / filename;
    
    if (!fs::exists(fullPath)) {
        sys_logger.error("File directory not found!");
        return {};
    }
    
    PlanetJSONReader<float> reader(fullPath);
    
    return reader.get_planets();
}



std::ostream& operator<<(std::ostream& os, const SimulationFiles& sim)
{
    auto simulations = sim.availableSimulations();
    os << "Available Simulations: " << simulations.size() << "\n";
    for (const auto& sim_name : simulations) {
        os << "  - " << sim_name << "\n";

    }
    return os;
}
