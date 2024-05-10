cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

project(pub_sub)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} emessgee)