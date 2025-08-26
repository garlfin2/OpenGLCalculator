//
// Created by scion on 6/13/2025.
//

#pragma once

#include "Window.h"

void ClearExceptionState(Window* window);
void NumberButtonClick(Window* window, const CalculatorButton& button);
void OperatorButtonClick(Window* window, const CalculatorButton& button);
void DecimalButtonClick(Window* window, const CalculatorButton& button);
void TrigButtonClick(Window* window, const CalculatorButton& button);
void NegateButtonClick(Window* window, const CalculatorButton& button);
void EvaluateButtonClick(Window* window, const CalculatorButton& button);