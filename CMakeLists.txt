cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER g++-16)

set(CMAKE_CXX_MODULE_STD ON)

project(serde LANGUAGES CXX)

string(APPEND CMAKE_CXX_FLAGS " -freflection")

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

file(GLOB_RECURSE ALL_MODULE_INTERFACES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cppm"
)

add_executable(serde-test)

target_sources(serde-test
    PRIVATE
        ${ALL_SOURCES}
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${ALL_MODULE_INTERFACES}
)

target_compile_options(serde-test PRIVATE -Wall -Wextra)
