cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER g++-16)

set(CMAKE_CXX_MODULE_STD ON)

project(serde LANGUAGES CXX)

string(APPEND CMAKE_CXX_FLAGS " -freflection")

file(GLOB_RECURSE SERDE_MODULE_INTERFACES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cppm"
)

add_library(serde)
add_library(serde::serde ALIAS serde)

target_sources(serde
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src"
        FILES ${SERDE_MODULE_INTERFACES}
)

target_compile_options(serde PUBLIC -freflection)
target_compile_options(serde PRIVATE -Wall -Wextra)

add_library(nlohmann_json)

target_sources(nlohmann_json
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/test"
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/test/json.cppm"
)

add_library(doctest INTERFACE)

target_include_directories(doctest INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/test")

enable_testing()

file(GLOB_RECURSE SERDE_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
)

add_executable(serde-test)

target_sources(serde-test PRIVATE ${SERDE_TEST_SOURCES})

target_sources(serde-test
    PRIVATE FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/test"
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/test/json_serde.cppm"
)

target_link_libraries(serde-test PRIVATE serde::serde nlohmann_json doctest)

target_compile_options(serde-test PRIVATE -Wall -Wextra)

add_test(NAME serde COMMAND serde-test)
