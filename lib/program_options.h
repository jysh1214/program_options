#ifndef PROGRAM_OPTIONS_H
#define PROGRAM_OPTIONS_H

#include <vector>
#include <string>

class ProgramOptions
{
public:
	struct Option
	{
		std::string name = "";
		std::string desc = "";
		std::string quiet = "";
		std::vector<std::string> valid{};
	};

	ProgramOptions();
	~ProgramOptions();

	ProgramOptions(const ProgramOptions&) = delete;
	ProgramOptions& operator=(const ProgramOptions&) = delete;

    void read(int argc, char** argv);
	void registerOption(std::string name, std::string desc, std::string quiet, std::initializer_list<std::string> valid);
	void registerOption(std::string name, std::string desc, std::string quiet);
	void registerOption(std::string name, std::string desc);
	std::string parse(std::string target);
	void showInfo();

private:
    bool optionsExist(std::string target);
	Option findOption(std::string target);
    void detectInvalidValue(std::string option, std::string value);
    
    std::string m_programName = "";
    std::vector<std::string> m_inputOptions{};
	std::vector<Option> m_registeredOptions{};
};

#endif