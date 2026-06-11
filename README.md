# revr

Express.js-inspired HTTP framework for C.

## Build

```sh
mkdir build
cmake -S . -B build -G Ninja
cmake --build build
```

Build a specific example:

```sh
cmake --build build --target static-server
```

## Examples

```sh
./build/static-server examples/static_website
# -> http://localhost:3490
```

## Library

```cmake
find_package(revr REQUIRED)
target_link_libraries(my-app PRIVATE revr::revr)
```

## Install

```sh
cmake --install build --prefix /usr/local
```
