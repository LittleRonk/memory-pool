cmake_minimum_required(VERSION 3.30)

project(memory_pool, LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR "C compiler not found")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/logger)

add_library(pool_errors
    STATIC
    ${PROJECT_SOURCE_DIR}/src/pool_errors.c)
target_include_directories(pool_errors
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

add_library(pool_logger
    STATIC
    ${PROJECT_SOURCE_DIR}/src/pool_logger.c)
target_include_directories(pool_logger
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/logger)

add_library(block_pool
    STATIC
    ${PROJECT_SOURCE_DIR}/src/block_pool.c)
target_include_directories(block_pool
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/logger)
target_link_libraries(block_pool PRIVATE pool_errors logger)

add_library(dynamic_pool
    STATIC
    ${PROJECT_SOURCE_DIR}/src/dynamic_pool.c)
target_include_directories(dynamic_pool
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/logger)

target_link_libraries(dynamic_pool PRIVATE pool_errors logger)

add_library(pool INTERFACE)
target_link_libraries(pool INTERFACE block_pool dynamic_pool)

add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
