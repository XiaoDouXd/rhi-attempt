cmake_minimum_required(VERSION 3.26)
project(3rd)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)
set(SDL_TEST OFF)
set(SDL_TEST_ENABLED_BY_DEFAULT OFF)
set(BUILD_SHARED_LIBS OFF)
set(SDL3IMAGE_BUILD_SHARED_LIBS OFF)

add_subdirectory(SDL3)
add_subdirectory(SDL3_image)
add_subdirectory(glm)