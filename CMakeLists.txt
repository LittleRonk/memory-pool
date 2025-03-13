cmake_minimum_required(VERSION 3.1)

project(memory_pool, LANGUAGES C)

add_library(pool
    STATIC
    ${PROJECT_SOURCE_DIR}/src/memory_pool.c)

target_include_directories(pool
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

add_executable(driver ${PROJECT_SOURCE_DIR}/examples/driver.c)

target_link_libraries(driver PRIVATE pool)
