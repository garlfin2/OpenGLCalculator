//
// Created by scion on 6/23/2025.
//

#include <Calculator/Core/Processor.h>

#include "Catch2/catch_test_macros.hpp"
#include "Catch2/catch_approx.hpp"

TEST_CASE("Calculator Processor")
{
    CalculatorProcessor& processor = CalculatorProcessor::GetInstance();

    SECTION("Simple Math")
    {
        REQUIRE(processor.Calculate("2+2") == 4.0);
        REQUIRE(processor.Calculate("9*3") == 27.0);
        REQUIRE(processor.Calculate("9/23") == Catch::Approx(0.391304348));
    }

    SECTION("PEMDAS")
    {
        REQUIRE(processor.Calculate("3+3*3") == 12.0);
        REQUIRE(processor.Calculate("92/(20+3)+4") == 8.0);
        REQUIRE(processor.Calculate("66+(2/34)-64/3") == Catch::Approx(44.7254902));
        REQUIRE(processor.Calculate("24*32/40-60+666*(2000-90*3)") == Catch::Approx(1152139.2));
    }

    SECTION("Trigonometry")
    {
        REQUIRE(processor.Calculate("sin(64/2)") == Catch::Approx(0.551426681));
        REQUIRE(processor.Calculate("cos(0)") == 1.0);
        REQUIRE(processor.Calculate("tan(3.14159/4)") == Catch::Approx(1.0));
    }
}