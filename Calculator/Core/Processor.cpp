//
// Created by scion on 6/23/2025.
//

#include "Processor.h"

#include <list>
#include <vector>

using namespace std::string_view_literals;

CalculatorProcessor::Operation CalculatorProcessor::Tokenize(std::string_view::iterator& begin, std::string_view::iterator end)
{
    u64 min = std::string::npos;
    std::string_view searchSpan(begin, end);

    const Operator* operatorResult = nullptr;
    begin = end;
    for(const Operator& operation : Operators)
    {
        const u64 searchResult = searchSpan.find(operation.Name);

        if(searchResult >= min)
            continue;

        min = searchResult;
        operatorResult = &operation;
        begin = searchSpan.begin() + searchResult;
    }

    // Current position is a valid operator
    if(begin == searchSpan.begin())
    {
        begin += operatorResult->Name.length();
        return operatorResult;
    }

    double scalarResult;
    return std::from_chars(searchSpan.data(), &*begin, scalarResult).ec == (std::errc) 0 ? scalarResult : ExceptionValue;
}

std::string CalculatorProcessor::FixEquation(std::string_view equation)
{
    std::string result = (std::string) equation;

    if(result[0] == '-')
        result = '0' + result;

    for(u64 it = result.find("(-"); it != std::string::npos; it = result.find("(-"))
        result.replace(it, 2, "(0-");

    return result;
}

bool IsOperator(const CalculatorProcessor::Operation& op)
{
    return std::holds_alternative<const Operator*>(op);
}

double CalculatorProcessor::Calculate(std::string_view equation)
{
    if(equation.empty())
        return 0.0;

    std::list<Operation> outputStack;
    std::vector<const Operator*> operatorStack;

    // still lazy
    const std::string fixedEquation = FixEquation(equation);
    equation = fixedEquation;

    u64 leftBracketCount = 0, rightBracketCount = 0;

    CalculatorType expected = CalculatorType::Any;
    Iterator begin = equation.begin();
    do
    {
        const Operation operation = Tokenize(begin, equation.end());

        if(std::holds_alternative<const Operator*>(operation))
        {
            const Operator* const op = std::get<const Operator*>(operation);

            if(!(bool)(expected & op->Type))
                return ExceptionValue;

            if(op->Name == "("sv)
            {
                operatorStack.push_back(op);
                leftBracketCount++;
            }
            else if(op->Name == ")"sv)
            {
                while(!operatorStack.empty() && operatorStack.back()->Name != "(")
                {
                    outputStack.emplace_back(operatorStack.back());
                    operatorStack.pop_back();
                }

                if(operatorStack.empty())
                    return ExceptionValue;

                operatorStack.pop_back();
                rightBracketCount++;
            }
            else
            {
               while(!operatorStack.empty() && operatorStack.back()->Importance >= op->Importance && operatorStack.back()->Name != "(")
               {
                   outputStack.emplace_back(operatorStack.back());
                   operatorStack.pop_back();
               }

               operatorStack.push_back(op);
            }
            expected = op->ExpectedNext;
        }
        else
        {
            if(!(bool)(expected & CalculatorType::Number))
                return ExceptionValue;

            outputStack.push_back(operation);
            expected = CalculatorType::Operator | CalculatorType::RParenthesis;
        }
    } while(begin != equation.end());

    if(leftBracketCount != rightBracketCount)
        return ExceptionValue;

    outputStack.insert(outputStack.end(), operatorStack.rbegin(), operatorStack.rend());

    using ListIter = std::list<Operation>::iterator;

    for(auto it = std::ranges::find_if(outputStack, IsOperator); it != outputStack.end(); it = std::ranges::find_if(outputStack, IsOperator))
    {
        const Operator* const op = std::get<const Operator*>(*it);

        ListIter bIt, aIt; bIt = aIt = std::prev(it);
        if(it == outputStack.begin())
            return ExceptionValue;

        double a, b; b = a = std::get<double>(*bIt);

        if(!op->Unary)
        {
            if(bIt == outputStack.begin())
                return ExceptionValue;

            aIt = std::prev(bIt);
            a = std::get<double>(*aIt);
        }

        const auto end = std::next(it);

        outputStack.erase(bIt);
        if(!op->Unary)
            outputStack.erase(aIt);

        outputStack.emplace(end, op->Function(a, b));
        outputStack.erase(it);
    }

    if(outputStack.empty())
        return ExceptionValue;

    return std::get<double>(outputStack.front());
}