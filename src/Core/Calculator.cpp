#include <stdio.h>
#include "Core/Calculator.hpp"

int calculate(int num1, int num2, char operation, int *result)
{
    int temp = 0;
    if (result == NULL)
        result = &temp;

    if ((operation == '/' || operation == '%') && num2 == 0)
    {
        printf("Error: division by zero\n");
        return -1;
    }

    switch (operation)
    {
    case '+':
        *result = num1 + num2;
        break;
    case '-':
        *result = num1 - num2;
        break;
    case '*':
        *result = num1 * num2;
        break;
    case '/':
        *result = num1 / num2;
        break;
    case '%':
        *result = num1 % num2;
        break;
    default:
        return -1;
    }

    return *result;
}
