project(CalculatorTests)

add_subdirectory(Catch2)
include(Catch2/extras/Catch.cmake)

add_executable(UnitTests CalculatorProcessorTests.cpp ButtonFactoryTests.cpp)
target_link_libraries(UnitTests PRIVATE CalculatorProc Catch2::Catch2WithMain)

FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/SDL3/bin/SDL3.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

catch_discover_tests(UnitTests)