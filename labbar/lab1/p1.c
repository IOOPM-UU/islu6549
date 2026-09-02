#include <stdio.h>

int main(void)
{
    int a = 0;
    for (int i = 1; i <= 10; i++)
    {
        for (int j = 1; j <= i; j++)
        {
            printf("*", j);
            a++;
        }
        printf("\n");
    }

    printf("total is: %d\n", a);

    return 0;
}