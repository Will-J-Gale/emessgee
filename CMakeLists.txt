cmake_minimum_required(VERSION 3.16)
project("emessgee")

add_library(${PROJECT_NAME} STATIC)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

file(
    GLOB_RECURSE 
    SRC_FILES    
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

if(EXAMPLES)
    add_subdirectory(examples/pub_sub)
    add_subdirectory(examples/pub_sub_threads)
    add_subdirectory(examples/multiple_topics_1)
    add_subdirectory(examples/multiple_topics_2)
    add_subdirectory(examples/publish_image)
endif()