# Program Options

## Build

Linux
```shell
cmake -G Ninja \
  -B build \
  .

cmake --build ./build
```

Windows
```shell
cmake -G "Visual Studio 17 2022" `
  -B build `
  .
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
