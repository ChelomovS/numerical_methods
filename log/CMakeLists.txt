cmake_minimum_required(VERSION 3.10)
project(LogImplementation C)

add_library(log_lib STATIC
    log.c
    table.c
    log_halley.c
)

target_compile_features(log_lib PRIVATE c_std_11)
target_link_libraries(log_lib PRIVATE m) # link libm, exp() used

add_executable(log_interactive main.c)
target_link_libraries(log_interactive PRIVATE log_lib)

add_executable(log_impl main.c)
target_compile_definitions(log_impl PRIVATE TESTING)
target_link_libraries(log_impl PRIVATE log_lib)