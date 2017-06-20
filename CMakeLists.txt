cmake_minimum_required(VERSION 2.8.11)
project(Symbolic)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-fno-elide-constructors")

include_directories(include ${catkin_INCLUDE_DIRS} headers/)

add_executable(symbolicc++ symbolicc++.cc)
