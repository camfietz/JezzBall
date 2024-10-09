cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(cpp_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SDL REQUIRED)

include_directories(include)

add_executable(jezzball src/main.cpp include/input.hpp include/window.hpp include/game.hpp)

target_include_directories(jezzball PUBLIC ${SDL_INCLUDE_DIR})
target_link_libraries(jezzball ${SDL_LIBRARIES})

install(DIRECTORY assets DESTINATION bin)
install(TARGETS jezzball DESTINATION bin)

# g++ -Wall -Wextra -Wpedantic -std=c++20 -o jezzball src/main.cpp -Iinclude -lSDL
# clang++ -Wall -Wextra -Wpedantic -std=c++20 -o jezzball src/main.cpp -Iinclude -lSDL
