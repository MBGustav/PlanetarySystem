#pragma once 

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

static const std::string  RESET       = "\033[0m"  ;
static const std::string  BLACK       = "\033[30m" ;            /* Black */
static const std::string  RED         = "\033[31m" ;            /* Red */
static const std::string  GREEN       = "\033[32m" ;            /* Green */
static const std::string  YELLOW      = "\033[33m" ;            /* Yellow */
static const std::string  BLUE        = "\033[34m" ;            /* Blue */
static const std::string  MAGENTA     = "\033[35m" ;            /* Magenta */
static const std::string  CYAN        = "\033[36m" ;            /* Cyan */
static const std::string  WHITE       = "\033[37m" ;            /* White */
static const std::string  BOLDBLACK   = "\033[1m\033[30m";      /* Bold Black */
static const std::string  BOLDRED     = "\033[1m\033[31m";      /* Bold Red */
static const std::string  BOLDGREEN   = "\033[1m\033[32m";      /* Bold Green */
static const std::string  BOLDYELLOW  = "\033[1m\033[33m";      /* Bold Yellow */
static const std::string  BOLDBLUE    = "\033[1m\033[34m";      /* Bold Blue */
static const std::string  BOLDMAGENTA = "\033[1m\033[35m";      /* Bold Magenta */
static const std::string  BOLDCYAN    = "\033[1m\033[36m";      /* Bold Cyan */
static const std::string  BOLDWHITE   = "\033[1m\033[37m";      /* Bold White */

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

    std::string beautify(std::string message, std::string color) 
    {
        return std::string(color + message + RESET);
    }
    
    void log(const std::string level, const std::string message) {
        std::cout << getCurrentTime() 
                  << "[" << level << "] " << RESET << message <<  std::endl;
    }
    
public:
    Logger(const std::string& filename, bool with_time = true, bool with_date = true) : 
        filename(filename), with_time(with_time), with_date(with_date) {}
    
    void info(const std::string& message) {
        log(beautify(BOLDBLUE, "INFO") , message);
    }
    
    void error(const std::string& message) {
        log(beautify(BOLDRED, "ERROR"), message);
    }
    
    void warning(const std::string& message) {
        log(beautify(BOLDYELLOW, "WARNING"), message);
    }
    
    void debug(const std::string& message) {
        log(beautify(BOLDWHITE, "DEBUG"), message);
    }
};

// Glbal Logger Instance
static Logger sys_logger("Application");
