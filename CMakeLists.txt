cmake_minimum_required(VERSION 3.5)

project(Calculator)

set(CMAKE_CXX_STANDARD 23)

set(COMPILE_FLAGS_RELEASE "-O3 -static -s -static-libgcc -mwindows")
set(COMPILE_FLAGS_DEBUG "-O0 -DDEBUG -static -static-libgcc")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILE_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${COMPILE_FLAGS_RELEASE}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILE_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COMPILE_FLAGS_DEBUG}")

include_directories(${CMAKE_SOURCE_DIR} Vendor)
set_property(GLOBAL PROPERTY CXX_STANDARD 20)

# GLAD
file(GLOB_RECURSE GLAD_SRC "Vendor/GLAD/*.h" "Vendor/GLAD/*.c")
add_library(GLAD STATIC ${GLAD_SRC})

# SDL
add_library(SDL STATIC IMPORTED)
set_property(TARGET SDL PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SDL3/bin/SDL3.dll")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SDL3/bin/SDL3.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# CalculatorProcessor
file(GLOB_RECURSE CALC_PROC_SRC "Calculator/Core/*.h" "Calculator/Core/*.inl" "Calculator/Core/*.cpp")
add_library(CalculatorProc STATIC ${CALC_PROC_SRC})
target_link_libraries(CalculatorProc GLAD SDL)

# Calculator
add_executable(Calculator "Calculator/main.cpp")
target_link_libraries(Calculator CalculatorProc)

# Unit Tests
add_subdirectory(CalculatorTests)