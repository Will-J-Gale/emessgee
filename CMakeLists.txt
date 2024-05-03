# cmake_minimum_required(VERSION 3.16)
# project("emessgee")

# add_library(${PROJECT_NAME} STATIC)

# file(GLOB_RECURSE SRC_FILES    
#     "src/*.cpp"
# )

# target_sources(
#     ${PROJECT_NAME}
#     PRIVATE
#     ${SRC_FILES}
# )

# target_include_directories(
#     ${PROJECT_NAME}
#     PUBLIC
#     include/
# )

cmake_minimum_required(VERSION 3.16)
project("emessgee")

add_executable(${PROJECT_NAME} main.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES    
    "src/*.cpp"
)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${SRC_FILES}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    include/
)