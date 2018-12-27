cmake_minimum_required(VERSION 3.12)
project(finalMCPB)

set(CMAKE_CXX_STANDARD 14)

add_executable(finalMCPB main.cpp Node.cpp Node.h Reader.cpp Reader.h ProblemInstance.cpp ProblemInstance.h Truck.cpp Truck.h Trip.cpp Trip.h Route.cpp Route.h Solution.cpp Solution.h Construction.cpp Construction.h ReorderNodes.cpp ReorderNodes.h RemoveNodes.cpp RemoveNodes.h AddNodes.cpp AddNodes.h)