cmake_minimum_required(VERSION 3.16)

project(revr C)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(revr STATIC
    src/server.c
    src/http.c
    src/uri.c
    src/fs.c
)
target_include_directories(revr
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(revr PROPERTIES
    EXPORT_NAME revr
)

add_executable(static-server examples/static_server.c)
target_link_libraries(static-server PRIVATE revr)
install(TARGETS revr static-server
    EXPORT  revr-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(
    DIRECTORY include/revr
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT revr-targets
    FILE    revr-config.cmake
    NAMESPACE revr::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/revr
)
