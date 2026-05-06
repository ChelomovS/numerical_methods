cmake_minimum_required(VERSION 3.14)
project(RNG_Validation LANGUAGES CXX)

include_directories(include)

add_executable(rng_validation src/main.cxx)

target_compile_features(rng_validation PRIVATE cxx_std_20)
set_target_properties(rng_validation PROPERTIES CXX_EXTENSIONS OFF)