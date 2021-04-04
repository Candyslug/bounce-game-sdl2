cmake_minimum_required(VERSION 3.0.0)
project(bounce-game-sdl VERSION 0.1.0)

add_executable(bounce-game-sdl src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

target_link_libraries(bounce-game-sdl SDL2 SDL2main)