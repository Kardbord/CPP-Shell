cmake_minimum_required (VERSION 2.4)
project (Assign4)

if (CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_CXX_FLAGS "-std=c++11 -O3 ${CMAKE_CXX_FLAGS}")
    endif()

    add_executable(Assign4
                   main.cpp
                   Utils.hpp
                   Shell.hpp Shell.cpp
                  )
