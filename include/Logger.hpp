#pragma once 

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

class Logger {
private:
    std::string filename;
    bool with_time, with_date;

    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        if(with_date) ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d") << "] ";
        if(with_time) ss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "]";

        return ss.str();
    }
    
    void log(const std::string& level, const std::string& message) {
        std::cout << getCurrentTime() 
                  << "[" << level << "] " << RESET << message <<  std::endl;
    }
    
public:
    Logger(const std::string& filename, bool with_time = true, bool with_date = true) : 
        filename(filename), with_time(with_time), with_date(with_date) {}
    
    void info(const std::string& message) {
        log(std::string(WHITE) + std::string(BOLDBLUE) + "INFO" , message);
    }
    
    void error(const std::string& message) {
        log(std::string(WHITE) + std::string(BOLDRED) + "ERROR", message);
    }
    
    void warning(const std::string& message) {
        log(std::string(BLACK) + std::string(BOLDYELLOW) + "WARNING", message);
    }
    
    void debug(const std::string& message) {
        log(std::string(BLACK) + std::string(BOLDWHITE) + "DEBUG", message);
    }
};

// Glbal Logger Instance
static Logger sys_logger("Application");
