# Revr

A lightweight and ergonomic framework for building HTTP servers in C.

## Demo

```c
#include "revr/revr.h"

void hello(RevrRequest *req, RevrResponse *res) {
    revr_send_text(res, 200, "Hello, World!");
}

int main() {
    RevrApp *app = revr_app_create();

    revr_get(app, "/hello", hello);
    revr_static(app, "/", "./public");

    revr_listen(app, "3490");
    revr_app_free(app);
}
```

## Build

```sh
mkdir build
cmake -S . -B build -G Ninja
cmake --build build
```

Build examples:

```sh
cmake --build build --target helloworld
cmake --build build --target static-website
```

## Examples

```sh
# Hello World + static file server
./build/helloworld
# -> http://localhost:3490/hello
# -> http://localhost:3490/index.html

# Static file server only
./build/static_website
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

## TODO

- [x] GET handler
- [x] Send files
- [x] Static mounting directories
- [x] URI decoding
- [x] Path normalization
- [x] Standard status code helpers
- [ ] Middleware
- [ ] Route parameters
- [x] Query parsing
- [x] Request headers
- [ ] POST body support
- [ ] Keep-Alive
- [ ] Thread pool
- [ ] WebSockets
