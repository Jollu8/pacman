cmake_minimum_required(VERSION 3.22)

include(FetchContent)
project(pacman)

set(SFML_VERSION "2.5.1")


set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.5.1
)
FetchContent_MakeAvailable(SFML)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(pacman main.cpp Entity.h np.h Game.h Ghost.h Pacman.h PIckup.h Info.h Tile.h Manager.h)
target_link_libraries(pacman
        sfml-graphics
        )
