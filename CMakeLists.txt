cmake_minimum_required(VERSION 3.0)
project(k)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS} -rdynamic)


add_executable(ch2 ch2.cpp)
add_executable(ch3 ch3.cpp)
add_executable(ch4 ch4.cpp)

llvm_map_components_to_libnames(llvm_libs support core irreader mcjit native orcjit)

target_link_libraries(ch3 ${llvm_libs})
target_link_libraries(ch4 ${llvm_libs})

