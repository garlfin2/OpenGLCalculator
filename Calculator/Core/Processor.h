//
// Created by scion on 6/23/2025.
//

#pragma once

#include <array>
#include <variant>
#include "Utility/Math.h"

using OperatorFunc = double(*)(double a, double b);

inline constexpr std::string_view ExceptionMessage = "Evaluation Error!";
inline constexpr double ExceptionValue = std::numeric_limits<double>::infinity();

enum class CalculatorType : u8
{
    Default = 0,
    Number = 1,
    Operator = 1 << 1,
    LParenthesis = 1 << 2,
    RParenthesis = 1 << 3,
    Any = UINT8_MAX,
    NonNumber = Any & ~Number
};

constexpr CalculatorType operator&(CalculatorType a, CalculatorType b) { return (CalculatorType) ((u8) a & (u8) b); }
constexpr CalculatorType operator|(CalculatorType a, CalculatorType b) { return (CalculatorType) ((u8) a | (u8) b); }

struct Operator
{
    std::string_view Name = DEFAULT;
    u8 Importance = DEFAULT;
    CalculatorType Type = DEFAULT;
    OperatorFunc Function = DEFAULT;
    bool Unary = false;
    CalculatorType ExpectedNext = CalculatorType::Number | CalculatorType::LParenthesis;

    bool operator==(char name) const { return Name[0] == name; }
    bool operator==(std::string_view name) const { return Name == name; }
};

// 2-PE 1-MD 0-AS
inline constexpr std::array Operators
{
    Operator{ "+", 0, CalculatorType::Operator, [](double a, double b) { return a + b; } },
    Operator{ "-", 0, CalculatorType::Operator, [](double a, double b) { return a - b; } },
    Operator{ "*", 1, CalculatorType::Operator, [](double a, double b) { return a * b; } },
    Operator{ "/", 1, CalculatorType::Operator, [](double a, double b) { return a / b; } },
    Operator{ "%", 1, CalculatorType::Operator, [](double a, double b) { return std::fmod(a, b); }},
    Operator{ "sin", 2, CalculatorType::LParenthesis, [](double a, double b) { return std::sin(a); }, true },
    Operator{ "cos", 2, CalculatorType::LParenthesis, [](double a, double b) { return std::cos(a); }, true },
    Operator{ "tan", 2, CalculatorType::LParenthesis, [](double a, double b) { return std::tan(a); }, true },
    Operator{ "(", 2, CalculatorType::LParenthesis, [](double a, double b) { return ExceptionValue; }, true, CalculatorType::Any },
    Operator{ ")", 2, CalculatorType::RParenthesis, [](double a, double b) { return ExceptionValue; }, true, CalculatorType::NonNumber },
};

class CalculatorProcessor
{
public:
    using Operation = std::variant<std::monostate, double, const Operator*>;
    using Iterator = std::string_view::iterator;

    double Calculate(std::string_view);
    Operation Tokenize(std::string_view::iterator& begin, std::string_view::iterator end);
    std::string FixEquation(std::string_view);

    static CalculatorProcessor& GetInstance() { return StaticCalculatorProcessor; }

private:
    CalculatorProcessor() = default;

    static CalculatorProcessor StaticCalculatorProcessor;
};

inline CalculatorProcessor CalculatorProcessor::StaticCalculatorProcessor = DEFAULT;