cmake_minimum_required(VERSION 3.0)
project(lapath)
set(SOURCE main.cpp lasystem.cpp adaptivesystem.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -stdlib=libc++ -Wall")

