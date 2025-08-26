//
// Created by scion on 6/23/2025.
//

#include <Calculator/Core/CalculatorFunctions.h>
#include <Calculator/Core/Utility/UIBuilder.h>

#include "Catch2/catch_test_macros.hpp"

TEST_CASE("Calculator Button Factory")
{
    CalculatorButton button = ButtonFactory::CreateButton();
    SECTION("Default Constructor")
    {
        REQUIRE(button.Position == Size2D());
        REQUIRE(button.Text == "");
        REQUIRE(button.Size == Size2D(1,1));
        REQUIRE(button.OnClick == nullptr);
        REQUIRE(button.Color == vec3(1.f));
    }

    button = ButtonFactory::CreateButton(Size2D(0, 4), "Test Button Name", NumberButtonClick);
    SECTION("Partial Constructor")
    {
        REQUIRE(button.Position == Size2D(0, 4));
        REQUIRE(button.Text == "Test Button Name");
        REQUIRE(button.Size == Size2D(1));
        REQUIRE(button.OnClick == NumberButtonClick);
        REQUIRE(button.Color == vec3(1.f));
    }

    button = ButtonFactory::CreateButton(Size2D(1, 4), "Test Button Name 2", DecimalButtonClick, Size2D(1, 2));
    SECTION("Full Constructor")
    {
        REQUIRE(button.Position == Size2D(1, 4));
        REQUIRE(button.Text == "Test Button Name 2");
        REQUIRE(button.Size == Size2D(1,2));
        REQUIRE(button.OnClick == DecimalButtonClick);
        REQUIRE(button.Color == vec3(1.f));
    }
}