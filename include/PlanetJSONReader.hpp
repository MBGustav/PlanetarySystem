#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include "PlanetProperties.hpp"
#include "JSONReader.hpp"

class PlanetJSONReader : public JSONReader {
public:
    std::vector<PlanetProperties> planets;

    PlanetJSONReader(const std::string &path) : JSONReader(path) {
        parse_planets(); 
    }

    std::vector<PlanetProperties> get_planets() const {
        return planets;
    }

    // Lê o JSON e popula o vetor planets
    void parse_planets() {
        // NÃO chame parse_content() aqui novamente. O construtor base já fez isso.
        auto it = data_map.find("planets");
        if (it != data_map.end()) {
            // A string no mapa contém o array bruto JSON. Convertê-lo agora:
            planets = parse_planets_array(it->second);
        }
    }

private:
    // Converte string de array para vector<double>
    static std::vector<double> parse_array(const std::string &str) {
        std::vector<double> result;
        std::stringstream ss(str);
        std::string item;

        while (std::getline(ss, item, ',')) {
            size_t start = item.find_first_not_of(" \t\n\r");
            size_t end = item.find_last_not_of(" \t\n\r");
            if (start != std::string::npos && end != std::string::npos)
                item = item.substr(start, end - start + 1);
            if (!item.empty())
                result.push_back(std::stod(item));
        }

        return result;
    }

    // Converte string de objeto planeta em struct Planet
    static PlanetProperties parse_planet_object(const std::string &obj_str) {
        PlanetProperties planet;
        std::map<std::string, std::string> temp_map; // Mapa local para este objeto

        char c;
        std::string key, value;

        std::istringstream file(obj_str);
        while (file >> c) {
            if (c == '"') {
                std::getline(file, key, '"');
                file >> c; // espera :
                
                // Lê o valor (pode ser string, numero ou array)
                file >> c; 
                if (c == '"') {
                    std::getline(file, value, '"');
                } else if (c == '[') {
                    value.clear();
                    // value += '['; // Opcional: manter os colchetes
                    int bracket_count = 1;
                    char arr_char;
                    while (file.get(arr_char) && bracket_count > 0) {
                        if (arr_char == '[') bracket_count++;
                        else if (arr_char == ']') bracket_count--;
                        if (bracket_count > 0) value += arr_char; // Pega conteúdo interno
                    }
                } else {
                    value.clear();
                    value += c; // Já leu o primeiro char do número
                    while (file.get(c) && !std::isspace(c) && c != ',' && c != '}') {
                        value += c;
                    }
                    // Se leu ',' ou '}', o loop para, mas o stream avança. Ok para parsing simples.
                }

                // Normaliza chave
                std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                temp_map[key] = value;
            }
        }

        // Popula o objeto PlanetProperties
        if(temp_map.count("name")) planet.set_name(temp_map["name"]);
        if(temp_map.count("mass")) planet.set_mass(std::stod(temp_map["mass"]));
        if(temp_map.count("radius")) planet.set_radius(std::stod(temp_map["radius"]));
        if(temp_map.count("position")) planet.set_position(parse_array(temp_map["position"]));
        if(temp_map.count("velocity")) planet.set_velocity(parse_array(temp_map["velocity"]));

        return planet;
    }

    // Converte array de objetos em vector<Planet>
    static std::vector<PlanetProperties> parse_planets_array(const std::string &array_str) {
        std::vector<PlanetProperties> result_planets;
        int brace_count = 0;
        std::string current_obj;

        for (char c : array_str) {
            if (c == '{') brace_count++;
            
            if (brace_count > 0) current_obj += c;
            
            if (c == '}') {
                brace_count--;
                if (brace_count == 0) {
                    // Fim de um objeto, parsear ele
                    result_planets.push_back(parse_planet_object(current_obj));
                    current_obj.clear();
                }
            }
        }
        return result_planets;
    }
};