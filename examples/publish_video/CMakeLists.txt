cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl -lpthread")

project(publish_video)

find_package(OpenCV REQUIRED)

include_directories(
    ${CUDA_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} emessgee ${OpenCV_LIBRARIES})