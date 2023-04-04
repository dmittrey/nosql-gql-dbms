cmake_minimum_required(VERSION 3.16)

project(dbms LANGUAGES C CXX)

file(GLOB_RECURSE PROJECT_HEADERS include/*.h)
file(GLOB_RECURSE PROJECT_SOURCES src/*.c)

add_executable(${PROJECT_NAME} ${PROJECT_HEADERS} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ./src)
target_include_directories(${PROJECT_NAME} PUBLIC ./include)
