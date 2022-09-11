# Program Options

## Requirements
- CMake 3.24
- cpp compiler

## Build
```
cmake -G "Unix Makefiles" \
  -B build \
  -D CMAKE_CXX_COMPILER=clang++-16 \
  -D CMAKE_CXX_STANDARD=17 \
  -D CMAKE_BUILD_TYPE=Release \
  .
```

```
cmake -B build .
```

```
cmake --build ./build -j 4
```

## Use

```cpp

```

## Run the Tests

Build the tests:
```
cmake -B build -D BUILD_TEST=ON .
```

Run the teste:
```
./build/test/UnitTests
```

rm -rf build && \
cmake -G "Unix Makefiles" \
  -B build \
  -D CMAKE_CXX_COMPILER=clang++-16 \
  -D CMAKE_CXX_STANDARD=17 \
  -D CMAKE_BUILD_TYPE=Release \
  . && \
cmake --build ./build -j 4 && \
./build/test/UnitTests
