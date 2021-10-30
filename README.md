# Program Options

Simple command line options parser based on std string.

## Build with CMake
```cmake
target_link_libraries(<PROJECT> program_options)
```

## Usage
```cpp
#include <program_options>

int main(int argc, char** argv)
{
    ProgramOptions options;
    options.registerOption("--input", "input path");
    options.registerOption("--output", "input path");
    options.registerOption("--filename", "output filename; default: output", "output");
    options.registerOption("--mode", "program mode, it should be 1 or 2; default: 1", 
        "1", {"1", "2"});
    options.registerOption("--score", "program score; default: 1.0", "1.0");
    options.read(argc, argv);

    std::string inputpath = options.parse("--input");
    std::string outpath = options.parse("--output");
    std::string filename = options.parse("--filename");
    int mode = std::stoi(options.parse("--mode"));
    float score = std::stof(options.parse("--score"));

    return 0;
}
```