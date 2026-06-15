#include <stdio.h>
#include <stdlib.h>
#include "io/Input.hpp"
#include "Core/Parser.hpp"
#include "Core/Calculator.hpp"
#include "conio.hpp"
#include "Network/Server.hpp"
#include "Network/Client.hpp"

void FileInput()
{
    FILE *in_file = fopen("file.txt", "r");
    if (in_file == NULL)
    {
        printf("file.txt file failed to open.\n");
        return;
    }

    char expr[100];
    int index = 0;
    int c;
    char ch;

    while ((c = fgetc(in_file)) != EOF)
    {
        ch = (char)c;

        if (index >= (int)sizeof(expr) - 1)
        {
            fprintf(stderr, "Expression too long, resetting buffer.\n");
            index = 0;
            continue;
        }

        expr[index++] = ch;

        if (ch == '=')
        {
            expr[index] = '\0';

            int result = parse_and_calculate(expr);
            printf("%s %d\n", expr, result);

            index = 0;
        }
    }

    fclose(in_file);
}

int convert(int *num1, int *num2, char *op)
{
    int done = 0;
    *num2 = 0;
    int first_key = 1;

    while (1)
    {
        printf(">> ");
        char ch = getch();
        printf("%c\n", ch);

        if (ch == 27)
        {
            printf("Thank you for using this calculator\n");
            printf("Exiting Bye!.\n");
            return 0;
        }

        if (ch == '+' || ch == '*' || ch == '%' || ch == '/' || ch == '-')
        {
            *op = ch;
            done = 1;
            first_key = 0;
            continue;
        }

        if (ch >= '0' && ch <= '9')
        {
            if (done == 0)
            {
                if (first_key)
                {
                    *num1 = 0;
                    first_key = 0;
                }
                *num1 = (*num1) * 10 + (ch - '0');
                printf("%d\n", *num1);
            }
            else
            {
                *num2 = (*num2) * 10 + (ch - '0');
                printf("%d\n", *num2);
            }
        }
        else if (ch == 13 || ch == 10)
        {
            return 1;
        }
    }
}

void CheckInput(int *n1, int *n2, char *operatorion, int *result, int argc, char *argv[])
{
    int Input = 0;

    if (argc <= 1)
    {
        printf("Invalid input method\n");
        return;
    }

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == 'k')
            Input = 2;
        else if (argv[i][0] == '-' && argv[i][1] == 'f')
            Input = 1;
        else if (argv[i][0] == '-' && argv[i][1] == 'C')
            Input = 3;
    }

    if (Input == 1)
    {
        FileInput();
    }
    else if (Input == 2)
    {
        while (1)
        {
            int status = convert(n1, n2, operatorion);
            if (status == 0)
                break;

            printf("%d %c %d = %d\n", *n1, *operatorion, *n2,
                   calculate(*n1, *n2, *operatorion, result));
            *n1 = 0;
        }
    }
    else if (Input == 3)
    {
        printf("1 => Start Server \n 2 => Start Client\n");
        int ch;
        scanf("%d", &ch);

        if (ch == 1)
            server();
        else
            client();
    }
    else
    {
        printf("Invalid input method\n");
    }
}
