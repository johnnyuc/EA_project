cmake_minimum_required(VERSION 3.27)
project(EA_project)

set(CMAKE_CXX_STANDARD 14)

add_executable(EA_project problem_A/aztec_vaults.cpp)
add_executable(Johnny problem_A/aztec_vaults_johnny.cpp)
add_executable(Sandbox problem_A/sandbox.cpp)