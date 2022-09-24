#include "program_options.h"

#include <functional>
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

#define CHECK_NO_DUPLICATED_OPTION(options, name)                              \
  do {                                                                         \
    if (options.find(name) != options.end())                                   \
      throw std::runtime_error("Detect deplicated option name.");              \
  } while (0)

namespace {

template <typename T> struct TypeMap {};

template <> struct TypeMap<int> {
  static ProgramOptions::DataType getType() {
    return ProgramOptions::DataType::INT;
  }
};

template <typename T> struct Converter {};

template <> struct Converter<int> {
  static std::function<int(const std::string &)> get() {
    return [](const std::string &in) { return std::stoi(in); };
  }
};

inline bool is_option(
    std::unordered_map<std::string, ProgramOptions::RegistOption *> &options,
    const std::string &target) {
  return (options[target] != nullptr);
}

inline bool
handle_option(std::vector<std::string> &list, const uint32_t index,
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
    if (info->quiet != nullptr) {
      T val = *reinterpret_cast<T *>(info->quiet);
      if (!check_func(val)) 
        throw std::runtime_error("Default value is not valid.");
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
    if (program_options->find_help)
      return;

    CHECK_NO_DUPLICATED_OPTION(program_options->option_register, name);

    ProgramOptions::DataType type = TypeMap<T>::getType();
    ProgramOptions::RegistOption *register_option =
        new ProgramOptions::RegistOption(name, desc, type);
    ProgramOptions::OptionInfo<T> *info = new ProgramOptions::OptionInfo<T>();

    parse(info, rest...);
    register_option->info = dynamic_cast<ProgramOptions::BaseInfo *>(info);
    program_options->option_register[name] = register_option;
  }
};

ProgramOptions::ProgramOptions() : help_option("--help"), find_help(false) {}

ProgramOptions::~ProgramOptions() {}

REGISTER_OPTION_CODE_GEN(int)

void ProgramOptions::set_help(std::string _help_option) {
  help_option = std::move(_help_option);
}

void ProgramOptions::read(const int argc, const char **argv) {
  std::vector<std::string> list;
  for (int index = 1; index < argc; ++index) {
    std::string item = argv[index];
    if (help_option == item) {
      find_help = true;
      show_help(argv[0]);
      return;
    }
    list.push_back(item);
  }

  for (uint32_t index = 0; index < list.size(); ++index) {
    CHECK_NO_DUPLICATED_OPTION(option_value, list[index]);

    bool success;
    if (is_option(option_register, list[index])) {
      success = handle_option(list, index, option_value);
      if (!success) {
        printf("Missing option value for '%s'; ", list[index].c_str());
        printf("Please use '%s %s'. \n", argv[0], help_option.c_str());
        throw std::runtime_error("Missing option value.");
      }
      ++index; // if it is a option, the next item is it's value
    } else {
      printf("Unknow option. Please use '%s %s'. \n", argv[0],
             help_option.c_str());
      throw std::runtime_error("Unknow option");
    }
  }
}

template <typename T> T ProgramOptions::get(const std::string &name) {
  if (option_value.find(name) == option_value.end()) {
    throw std::runtime_error("No this option.");
  }

  auto convert = Converter<T>::get();
  return convert(option_value[name]);
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
