#include "program_options.h"

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdexcept>
#include <algorithm>

ProgramOptions::ProgramOptions() {}

ProgramOptions::~ProgramOptions() {}

void ProgramOptions::read(int argc, char** argv)
{
    m_programName = argv[0];
    for (int i = 0; i < argc; ++i) {
        m_inputOptions.push_back(argv[i]);
    }

    // check help option
    if (optionsExist("--help")) {
        showInfo();
        exit(0);
    }
}

void ProgramOptions::registerOption(std::string name, std::string desc, std::string quiet, std::initializer_list<std::string> valid)
{
    Option option;
    option.name = name;
    option.desc = desc;
    option.quiet = quiet;
    for (auto item : valid) {
        option.valid.push_back(item);
    }
    m_registeredOptions.push_back(option);
}

void ProgramOptions::registerOption(std::string name, std::string desc, std::string quiet)
{
    Option option;
    option.name = name;
    option.desc = desc;
    option.quiet = quiet;
    m_registeredOptions.push_back(option);
}

void ProgramOptions::registerOption(std::string name, std::string desc)
{
    Option option;
    option.name = name;
    option.desc = desc;
    m_registeredOptions.push_back(option);
}

std::string ProgramOptions::parse(std::string target)
{
    Option option = findOption(target);
    if (option.name == "") {
        std::cout << "No this Option: " << target << std::endl;
        exit(-1);
    }

    std::string result = "";
    for (size_t i = 0; i < m_inputOptions.size(); ++i) {
        if (m_inputOptions[i] == target && i + 1 < m_inputOptions.size()) {
            result = m_inputOptions[i + 1];
        }
    }

    // use default value
    if (result == "" && option.quiet != "") {
        result = option.quiet;
    }

    // missing option
    if (result == "" && option.quiet == "") {
        std::cout << "Missing Option: " << option.name << std::endl;
        printf("See '%s --help' \n", m_programName.c_str());
        exit(-1);
    }

    // check the value of option is valid
    if (result != "" && option.valid.size() > 0) {
        bool isValid = false;
        for (auto valid : option.valid) {
            if (result == valid) isValid = true;
        }
        if (!isValid) {
            detectInvalidValue(option.name, result);
        }
    }

    return result;
}

void ProgramOptions::showInfo()
{
    std::cout << "Usage: " << m_programName << " [option] ..." << std::endl;
    std::cout << "Options and Arguments: " << std::endl;

    // get max length
    size_t maxSize = 0;
    for (auto option : m_registeredOptions) {
        maxSize = (maxSize > option.name.size()) ? maxSize : option.name.size();
    }

    for (auto option : m_registeredOptions) {
        std::string name = option.name;
        std::string desc = option.desc;
        for (size_t i = 0; i < maxSize; ++i) {
            if (i < option.name.size()) std::cout << option.name[i];
            else std::cout << " ";
        }
        std::cout << ":";
        std::cout << desc << std::endl;
    }
}

// private
bool ProgramOptions::optionsExist(std::string target)
{
    if (std::find(m_inputOptions.begin(), m_inputOptions.end(), target) != m_inputOptions.end()) {
        return true;
    }

    return false;
}

// private
ProgramOptions::Option ProgramOptions::findOption(std::string target)
{
    for (auto option : m_registeredOptions) {
        if (option.name == target)
            return option;
    }

    Option empty;
    return empty;
}

// private
void ProgramOptions::detectInvalidValue(std::string option, std::string value)
{
    printf("Invalid value for option '%s': %s \n", option.c_str(), value.c_str());
    printf("See '%s --help' \n", m_programName.c_str());
    exit(-1);
}