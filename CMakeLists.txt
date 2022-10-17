cmake_minimum_required(VERSION 3.10)
project (type_list)
add_executable (type_list main.cpp type_list.hpp)
set_target_properties (type_list PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED Yes
)
