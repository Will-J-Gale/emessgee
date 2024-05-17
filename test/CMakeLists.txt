cmake_minimum_required(VERSION 3.16)
project("emessgee_tests")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(
    GLOB_RECURSE 
    SRC_FILES    
    "src/*.cpp"
)

message(${SRC_FILES})

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

target_sources(
    ${PROJECT_NAME}
    PUBLIC
    ${SRC_FILES}
)

target_link_libraries(
    ${PROJECT_NAME}
    gtest
    emessgee
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})