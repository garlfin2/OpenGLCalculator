//
// Created by scion on 6/13/2025.
//

#include "CalculatorFunctions.h"
#include "Processor.h"

#include "Window.h"

std::string::iterator SkipParenthasesReverse(std::string::iterator searchStart, std::string::iterator begin)
{
    u64 bracketCount = 1;
    for(std::string::iterator it = searchStart; it >= begin; --it)
    {
        if(*it == '(')
            --bracketCount;
        else if(*it == ')')
            ++bracketCount;

        if(bracketCount == 0)
            return it;
    }

    return begin;
}

void NumberButtonClick(Window* window, const CalculatorButton& button)
{
    std::string& equation = window->GetEquation();

    ClearExceptionState(window);

    if(!equation.empty() && equation.back() == ')')
        return;

    equation += button.Text;
}

void OperatorButtonClick(Window* window, const CalculatorButton& button)
{
    std::string& equation = window->GetEquation();

    ClearExceptionState(window);

    if(equation.empty())
        return;

    const auto found = std::find(Operators.begin(), Operators.end(), equation.back());
    if(found != Operators.end() && found->Type != CalculatorType::RParenthesis)
        equation.back() = button.Text[0];
    else
        equation += button.Text[0];
}

void DecimalButtonClick(Window* window, const CalculatorButton& button)
{
    std::string& equation = window->GetEquation();

    ClearExceptionState(window);

    bool alreadyHasDecimal = false;
    for(auto it = equation.end(); it >= equation.begin(); --it)
    {
        if(std::find(Operators.begin(), Operators.end(), *it) != Operators.end())
            break;

        if(*it == '.')
        {
            alreadyHasDecimal = true;
            break;
        }
    }

    if(!alreadyHasDecimal)
        equation += '.';
}

void TrigButtonClick(Window* window, const CalculatorButton& button)
{
    ClearExceptionState(window);

    NumberButtonClick(window, button);

    window->SetUseSpecialButtons(false);
}

void NegateButtonClick(Window* window, const CalculatorButton& button)
{
    std::string& equation = window->GetEquation();

    ClearExceptionState(window);

    if(equation.empty())
        return;

    const auto found = std::find(Operators.begin(), Operators.end(), equation.back());
    if(found != Operators.end() && found->Type == CalculatorType::Operator)
        return;

    for(auto it = equation.end() - 1; it >= equation.begin(); --it)
    {
        if(*it == ')')
            it = SkipParenthasesReverse(it, equation.begin());

        if(isdigit(*it) || *it == '.' || isalpha(*it))
            continue;

        if(*it == '+')
            *it = '-';
        else if(*it == '-')
        {
            if(it == equation.begin() || std::find(Operators.begin(), Operators.end(), *(it - 1)) != Operators.end())
                equation.erase(it);
            else
                *it = '+';
        }
        else
            equation.insert(it + 1, '-');
        return;
    }

    equation.insert(equation.begin(), '-');
}

void EvaluateButtonClick(Window* window, const CalculatorButton& button)
{
    std::string& equation = window->GetEquation();

    ClearExceptionState(window);

    if(equation.empty())
        return;

    const double result = CalculatorProcessor::GetInstance().Calculate(equation);
    if(result == std::numeric_limits<double>::infinity())
    {
        equation = "Evaluation Error!";
        return;
    }

    equation = std::to_string(result);

    // removes trailing zeros
    auto it = equation.end() - 1;
    for(; it >= equation.begin(); --it)
    {
        if(*it == '.')
            break;
        if(*it == '0')
            continue;

        ++it;
        break;
    }

    if(it != equation.begin())
        equation.erase(it, equation.end());
}

void ClearExceptionState(Window* window)
{
    if((std::string_view) window->GetEquation() == ExceptionMessage)
        window->GetEquation().clear();
}

