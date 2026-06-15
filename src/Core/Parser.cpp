#include <string.h>
#include "Core/Parser.hpp"
#include "Core/Calculator.hpp"

int parse_and_calculate(char str[])
{
    int num1 = 0, num2 = 0;
    char operation = ' ';
    int done = 0;
    bool evaluated = false;
    bool num1_negative = false;
    bool num2_negative = false;
    bool num1_started = false;
    bool num2_started = false;
    int i = 0;
    int len = strlen(str);
    int ans = -1;

    while (i < len)
    {
        char ch = str[i++];

        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
        {
            continue;
        }
        else if (ch == '+' || ch == '*' || ch == '%' || ch == '/' || ch == '-')
        {
            if (done == 0)
            {
                if (!num1_started && (ch == '-' || ch == '+'))
                {
                    num1_negative = (ch == '-');
                    num1_started = true;
                    continue;
                }

                done = 1;
                operation = ch;
            }
            else
            {
                if (!num2_started && (ch == '-' || ch == '+'))
                {
                    num2_negative = (ch == '-');
                    num2_started = true;
                    continue;
                }

                operation = ch;
            }
        }
        else if (ch >= '0' && ch <= '9')
        {
            if (done == 0)
            {
                num1 = num1 * 10 + (ch - '0');
                num1_started = true;
            }
            else
            {
                num2 = num2 * 10 + (ch - '0');
                num2_started = true;
            }
        }
        else if (ch == '=')
        {
            if (num1_negative)
                num1 = -num1;
            if (num2_negative)
                num2 = -num2;

            ans = calculate(num1, num2, operation, NULL);
            evaluated = true;
            break;
        }
    }

    if (!evaluated && done == 1)
    {
        if (num1_negative)
            num1 = -num1;
        if (num2_negative)
            num2 = -num2;

        ans = calculate(num1, num2, operation, NULL);
    }

    return ans;
}

int Client_Server(char str[])
{
    return parse_and_calculate(str);
}
