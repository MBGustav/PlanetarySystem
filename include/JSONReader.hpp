#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <filesystem>
// This class is responsible for reading JSON files and returning a map of key-value pairs in string format.
// We consider a very simple JSON structure with only one nested objects with arrays for this implementation.


class JSONReader
{
protected:
    std::string filepath;    
    std::map<std::string, std::string> data_map;
    
    void open_file();
    virtual void parse_content();
    
    public:
    JSONReader(std::string filepath); 
    ~JSONReader();
    
    bool file_exists() const;
    void set_new_filepath(std::string new_filepath);
    std::map<std::string, std::string> get_data_as_map();
    void get_data_as_vector(std::vector<std::string>& data_vector);
    
};

void JSONReader::open_file()
{
    try
    {
        if (file_exists()) 
        { 
            std::cout << "File found: " << filepath << std::endl;
        }
        
        else
        { 
            
            std::filesystem::path full_path = std::filesystem::absolute(this->filepath);

            throw std::runtime_error("File not found: " + full_path.string());
        } 

        data_map.clear();
        parse_content();    
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

JSONReader::JSONReader(std::string path)
{
    auto fs = std::filesystem::absolute(path);
    fs = fs.lexically_normal();
    filepath = fs.string();
    open_file();
}

std::map<std::string, std::string> JSONReader::get_data_as_map()
{
    return data_map;
}

void JSONReader::set_new_filepath(std::string new_filepath) {
    
    this->filepath = new_filepath;
    open_file();

}

void JSONReader::get_data_as_vector(std::vector<std::string>& data_vector)
{
    data_vector.clear();
    for (const auto& pair : data_map)
    data_vector.push_back(pair.second);
}

void JSONReader::parse_content()
{
    std::ifstream file(this->filepath);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << filepath << std::endl;
        return;
    }

    data_map.clear();

    char c;
    std::string key, value;
    std::string parent_key, tmp_key_concat;

    // Função lambda para transformar string em lowercase
    auto to_lower = [](std::string &str) {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c){ return std::tolower(c); });
    };

    while (file >> c)
    {
        if (c == '"')
        {
            std::getline(file, key, '"');
            file >> c; // deve ser ':'
            file >> c; // próximo caractere do valor

            //  caso objeto 
            if (c == '{')
            {
                parent_key = key;

                while (file >> c && c != '}')
                {
                    if (c == '"')
                    {
                        std::getline(file, key, '"');
                        file >> c; // :
                        file >> c; // próximo caractere do valor

                        //  valor string 
                        if (c == '"')
                        {
                            std::getline(file, value, '"');
                        }
                        //  valor array 
                        else if (c == '[')
                        {
                            value.clear();
                            int bracket_count = 1;
                            char arr_char;
                            while (file.get(arr_char) && bracket_count > 0)
                            {
                                if (arr_char == '[') bracket_count++;
                                else if (arr_char == ']') bracket_count--;
                                if (bracket_count > 0) value += arr_char;
                            }
                        }
                        //  valor simples (número, bool, null) 
                        else
                        {
                            value.clear();
                            while (!std::isspace(c) && c != ',' && c != '}')
                            {
                                value += c;
                                file.get(c);
                            }
                        }

                        tmp_key_concat = parent_key + "." + key;
                        to_lower(tmp_key_concat);
                        data_map[tmp_key_concat] = value;
                    }
                }

                parent_key.clear();
            }
            //  Level 1 
            else
            {
                //  string 
                if (c == '"') std::getline(file, value, '"');
                //  array 
                else if (c == '[')
                {
                    value.clear();
                    int bracket_count = 1;
                    char arr_char;
                    while (file.get(arr_char) && bracket_count > 0)
                    {
                        if (arr_char == '[') bracket_count++;
                        else if (arr_char == ']') bracket_count--;
                        if (bracket_count > 0) value += arr_char;
                    }
                }
                //  simple Values 
                else
                {
                    value.clear();
                    while (!std::isspace(c) && c != ',' && c != '}' && c != ']')
                    {
                        value += c;
                        file.get(c);
                    }
                }

                data_map[key] = value;
            }
        }
    }
}



JSONReader::~JSONReader()
{
    data_map.clear();
    filepath.clear();
}

bool JSONReader::file_exists() const
{
    try
    {
        std::filesystem::path full_path = std::filesystem::absolute(this->filepath);
        full_path = full_path.lexically_normal();
        std::ifstream file(full_path);
        if (!file.is_open()) { 
            throw std::runtime_error("File not found: " + full_path.string());
        }
        return file.good();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}


