cmake_minimum_required(VERSION 3.10)
project(diag_middleware LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(diag_middleware
    src/main.cpp
    src/diagnostic_middleware.cpp
)

target_include_directories(diag_middleware
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
