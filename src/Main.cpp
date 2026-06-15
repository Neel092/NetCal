#include "io/Input.hpp"

int main(int argc, char *argv[])
{
    int n1 = 0, n2 = 0;
    char operation = '\0';
    int *result = 0;

    CheckInput(&n1, &n2, &operation, result, argc, argv);
    return 0;
}
