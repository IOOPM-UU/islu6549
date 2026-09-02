#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc > 3 || argc < 3)
    {
        printf("only 2 inputs allowed!\n");
        exit(1);
    }

    int rows = atoi(argv[1]);
    int count = atoi(argv[2]);
    int a = 0;
    for (int i = 1; i <= rows; i++)
    {
        for (int j = 1; j <= count * i; j++)
        {
            printf("*", j);
            a++;
        }
        printf("\n");
    }

    printf("total is: %d\n", a);

    return 0;
}