#ifndef PROGRAM_OPTIONS_H
#define PROGRAM_OPTIONS_H

#include <initializer_list>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct RegisterOptionImpl;

class ProgramOptions {
public:
  ProgramOptions();
  ~ProgramOptions();

  enum class DataType {
    INT,
    FLOAT,
  };

  struct OptionBaseDataTypeInfo {
    virtual ~OptionBaseDataTypeInfo(){};
    void *quiet = nullptr;
  };

  template <typename T> struct OptionDataTypeInfo : OptionBaseDataTypeInfo {
    ~OptionDataTypeInfo() { free(quiet); }
    using CheckFunc = bool (*)(const T);
    CheckFunc check_func = nullptr;
  };

  struct RegisterOption {
    RegisterOption(const std::string &_name, const std::string &_desc,
                   const DataType _type)
        : name(_name), desc(_desc), type(_type) {}
    ~RegisterOption() {}

    OptionBaseDataTypeInfo *info = nullptr;
    std::string name;
    std::string desc;
    DataType type;
    std::string value;
  };

  template <typename T>
  void register_option(std::string name, std::string desc);

  template <typename T>
  void register_option(std::string name, std::string desc, const T quiet);

  template <typename T>
  void register_option(std::string name, std::string desc,
                       bool (*check_func)(T));

  template <typename T>
  void register_option(std::string name, std::string desc, const T quiet,
                       bool (*check_func)(T));

  void set_help(std::string _help_option);

  void read(const int argc, const char **argv);

  template <typename T> T get(std::string name);

  void show_help(std::string app_name);

private:
  bool find_help;
  std::string help_option;
  std::unordered_map<std::string, RegisterOption *> options;

  friend struct RegisterOptionImpl;
};

#endif // PROGRAM_OPTIONS_H
