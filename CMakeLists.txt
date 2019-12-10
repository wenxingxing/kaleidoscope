cmake_minimum_required(VERSION 3.0)
project(k)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -iquote /usr/include/")

set(LLVM_DIR /usr/lib/cmake/llvm/)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LVMConfig.cmake in: ${LLVM_DIR}")

message(${LLVM_INCLUDE_DIRS})
message(${LLVM_DEFINITIONS})

add_definitions(${LLVM_DEFINITIONS})

#add_executable(main main.cpp)
#add_executable(ch2 ch2.cpp)
#add_executable(ch3 ch3.cpp)
#add_executable(ch4 ch4.cpp)
#add_executable(ch5 ch5.cpp)
#add_executable(ch6 ch6.cpp)
#add_executable(ch7 ch7.cpp)
add_executable(ch9 ch9.cpp)

llvm_map_components_to_libnames(llvm_libs support core irreader mcjit native orcjit)

#target_link_libraries(ch3 ${llvm_libs})
#target_link_libraries(ch4 ${llvm_libs})
#target_link_libraries(ch5 ${llvm_libs})
#target_link_libraries(ch6 ${llvm_libs})
#target_link_libraries(ch7 ${llvm_libs})
target_link_libraries(ch9 ${llvm_libs})

