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
  }                                                                            \
  template <> T ProgramOptions::get(std::string name) {                        \
    if (!options.contains(name)) {                                             \
      throw std::runtime_error("No this option.");                             \
    }                                                                          \
    auto register_type = options[name]->type;                                  \
    auto ask_type = TypeMap<T>::getType();                                     \
    if (ask_type != register_type) {                                           \
      throw std::runtime_error("Rigester data type is not correct.");          \
    }                                                                          \
    if (!(options[name]->value).empty()) {                                     \
      auto convert = ::Converter<T>::get();                                    \
      return convert(options[name]->value);                                    \
    }                                                                          \
    return *reinterpret_cast<T *>((options[name]->info)->quiet);               \
  }

#define CHECK_NO_DUPLICATED_OPTION(options, name)                              \
  do {                                                                         \
    if (options.contains(name))                                                \
      throw std::runtime_error("Detect deplicated option name.");              \
  } while (0)

namespace {

template <typename T> struct TypeMap {};

template <> struct TypeMap<int> {
  static ProgramOptions::DataType getType() {
    return ProgramOptions::DataType::INT;
  }
};

template <> struct TypeMap<float> {
  static ProgramOptions::DataType getType() {
    return ProgramOptions::DataType::FLOAT;
  }
};

template <typename T> struct Converter {};

template <> struct Converter<int> {
  static std::function<int(const std::string &)> get() {
    return [](const std::string &in) { return std::stoi(in); };
  }
};

template <> struct Converter<float> {
  static std::function<float(const std::string &)> get() {
    return [](const std::string &in) { return std::stof(in); };
  }
};

inline bool is_option(
    std::unordered_map<std::string, ProgramOptions::RegisterOption *> &options,
    const std::string &target) {
  return (options[target] != nullptr);
}

inline bool assign_option_value(
    const std::vector<std::string> &list, const uint32_t index,
    std::unordered_map<std::string, ProgramOptions::RegisterOption *>
        &options) {
  if (index + 1 >= list.size()) {
    return false;
  }
  options[list[index]]->value = list[index + 1];
  return true;
}

} // namespace

struct RegisterOptionImpl {
public:
  template <typename T> static void parse() {}

  template <typename T>
  static void parse(ProgramOptions::OptionDataTypeInfo<T> *info) {}

  template <typename T>
  static void parse(ProgramOptions::OptionDataTypeInfo<T> *info,
                    const T quiet) {
    info->quiet = new T[1]{quiet};
  }

  template <typename T>
  static void parse(ProgramOptions::OptionDataTypeInfo<T> *info,
                    bool (*check_func)(T)) {
    if (info->quiet != nullptr) {
      T val = *reinterpret_cast<T *>(info->quiet);
      if (!check_func(val))
        throw std::runtime_error("Default value is not valid.");
    }
    info->check_func = check_func;
  }

  template <typename T, typename... Args>
  static void parse(ProgramOptions::OptionDataTypeInfo<T> *info, const T quiet,
                    Args... rest) {
    info->quiet = new T[1]{quiet};
    parse(info, rest...);
  }

  template <typename T, typename... Args>
  static void parse(ProgramOptions *program_options, const std::string &name,
                    const std::string &desc, Args... rest) {
    if (program_options->find_help)
      return;

    CHECK_NO_DUPLICATED_OPTION(program_options->options, name);

    ProgramOptions::DataType type = TypeMap<T>::getType();
    ProgramOptions::RegisterOption *register_option =
        new ProgramOptions::RegisterOption(name, desc, type);
    ProgramOptions::OptionDataTypeInfo<T> *info =
        new ProgramOptions::OptionDataTypeInfo<T>();

    parse(info, rest...);
    register_option->info =
        dynamic_cast<ProgramOptions::OptionBaseDataTypeInfo *>(info);
    program_options->options[name] = register_option;
  }
};

ProgramOptions::ProgramOptions() : help_option("--help"), find_help(false) {}

ProgramOptions::~ProgramOptions() {}

REGISTER_OPTION_CODE_GEN(int)
REGISTER_OPTION_CODE_GEN(float)

void ProgramOptions::set_help(std::string _help_option) {
  help_option = std::move(_help_option);
}

void ProgramOptions::read(const int argc, const char **argv) {
  std::vector<std::string> input_list;
  std::unordered_set<std::string> input_items;
  for (int index = 1; index < argc; ++index) {
    std::string item = argv[index];
    if (help_option == item) {
      find_help = true;
      show_help(argv[0]);
      return;
    }
    input_list.push_back(item);
  }

  for (uint32_t index = 0; index < input_list.size(); ++index) {
    CHECK_NO_DUPLICATED_OPTION(input_items, input_list[index]);
    input_items.insert(input_list[index]);

    bool success;
    if (::is_option(options, input_list[index])) {
      success = ::assign_option_value(input_list, index, options);
      if (!success) {
        printf("Missing option value for '%s'; ", input_list[index].c_str());
        printf("Please use '%s %s'. \n", argv[0], help_option.c_str());
        throw std::runtime_error("Missing option value.");
      }
      ++index; // if it is a option, the next item would be it's value
    } else {
      printf("Unknow option. Please use '%s %s'. \n", argv[0],
             help_option.c_str());
      throw std::runtime_error("Unknow option");
    }
  }
}

void ProgramOptions::show_help(std::string app_name) {
  printf("Usage: %s [option] ...", app_name.c_str());
  printf("Options and Arguments: \n");

  auto it = options.begin();
  for (; it != options.end(); ++it) {
    printf("%-15s: %s \n", (it->first).c_str(), ((it->second)->desc).c_str());
  }
}

#undef REGISTER_OPTION_CODE_GEN
#undef CHECK_NO_DUPLICATED_OPTION
