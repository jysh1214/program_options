#include "program_options.h"

#include <iostream>
#include <vector>

#define REGISTER_OPTION_CODE_GEN(T)                                            \
  template <>                                                                  \
  void ProgramOptions::register_option<T>(std::string name,                    \
                                          std::string desc) {                  \
    RegisterOptionImpl::parse<T>(this, name, desc);                            \
  }                                                                            \
  template <>                                                                  \
  void ProgramOptions::register_option<T>(std::string name, std::string desc,  \
                                          const T quiet) {                     \
    RegisterOptionImpl::parse<T>(this, name, desc, quiet);                     \
  }                                                                            \
  template <>                                                                  \
  void ProgramOptions::register_option<T>(std::string name, std::string desc,  \
                                          bool (*check_func)(T)) {             \
    RegisterOptionImpl::parse<T>(this, name, desc, check_func);                \
  }                                                                            \
  template <>                                                                  \
  void ProgramOptions::register_option<T>(std::string name, std::string desc,  \
                                          const T quiet,                       \
                                          bool (*check_func)(T)) {             \
    RegisterOptionImpl::parse<T>(this, name, desc, quiet, check_func);         \
  }

namespace {

template <typename T> struct TypeMap {};

template <> struct TypeMap<int> {
  static ProgramOptions::DataType getType() {
    return ProgramOptions::DataType::INT;
  }
};

inline bool is_option(
    std::unordered_map<std::string, ProgramOptions::RegistOption *> &options,
    const std::string &target) {
  return (options[target] != nullptr);
}

inline bool handle_option(std::vector<std::string> &list, const uint32_t index,
                   std::unordered_map<std::string, std::string> &option_value) {
  if (index + 1 >= list.size()) {
    return false;
  }
  option_value[list[index]] = list[index + 1];
  return true;
}

} // namespace

struct RegisterOptionImpl {
public:
  template <typename T> static void parse() {}

  template <typename T>
  static void parse(ProgramOptions::OptionInfo<T> *info) {}

  template <typename T>
  static void parse(ProgramOptions::OptionInfo<T> *info, const T quiet) {
    info->quiet = new T[1]{quiet};
  }

  template <typename T>
  static void parse(ProgramOptions::OptionInfo<T> *info,
                    bool (*check_func)(T)) {
    if (info->quiet) {
      if (!check_func(*info->quiet))
        throw std::runtime_error("ERROR: Default value is not valid.");
    }
    info->check_func = check_func;
  }

  template <typename T, typename... Args>
  static void parse(ProgramOptions::OptionInfo<T> *info, const T quiet,
                    Args... rest) {
    info->quiet = new T[1]{quiet};
    parse(info, rest...);
  }

  template <typename T, typename... Args>
  static void parse(ProgramOptions *program_options, const std::string &name,
                    const std::string &desc, Args... rest) {
    if (program_options->option_register.find(name) !=
        program_options->option_register.end()) {
      throw std::runtime_error("ERROR: Detect deplicated option name.");
    }
    ProgramOptions::DataType type = TypeMap<T>::getType();
    ProgramOptions::RegistOption *register_option =
        new ProgramOptions::RegistOption(name, desc, type);
    ProgramOptions::OptionInfo<T> *info = new ProgramOptions::OptionInfo<T>();
    parse(info, rest...);
    register_option->info = dynamic_cast<ProgramOptions::BaseInfo *>(info);
    program_options->option_register[name] = register_option;
  }
};

ProgramOptions::ProgramOptions() : help_option("--help") {}

ProgramOptions::~ProgramOptions() {}

REGISTER_OPTION_CODE_GEN(int)

void ProgramOptions::set_help(std::string _help_option) {
  help_option = std::move(_help_option);
}

void ProgramOptions::read(const int argc, const char **argv) {
  std::vector<std::string> list;
  for (int index = 1; index < argc; ++index) {
    std::string item = argv[index];

    if (item == help_option) {
      show_help(argv[0]);
      return;
    }

    list.push_back(item);
  }

  for (uint32_t index = 0; index < list.size(); ++index) {
    bool success;
    if (is_option(option_register, list[index])) {
      success = handle_option(list, index, option_value);
      if (!success) {
        printf("Missing option value for '%s'; ", list[index].c_str());
        printf("Please use '%s %s'. \n", argv[0], help_option.c_str());
        throw std::runtime_error("Missing option value");
      }
    }

    printf("Unknow option. Please use '%s %s'. \n", argv[0],
           help_option.c_str());
    throw std::runtime_error("Unknow option");
  }
}

void ProgramOptions::show_help(std::string app_name) {
  printf("Usage: %s [option] ...", app_name.c_str());
  printf("Options and Arguments: \n");

  auto it = option_register.begin();
  for (; it != option_register.end(); ++it) {
    std::string name = it->first;
    std::string desc = (it->second)->desc;
    printf("%-15s: %s \n", name.c_str(), desc.c_str());
  }
}

#undef REGISTER_OPTION_CODE_GEN
