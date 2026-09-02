#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("give a command line value!\n");
    }
    else
    {
        for (int i = 1; i <= atoi(argv[1]); i++)
        {
            if (i % 3 == 0 && i % 5 != 0)
            {
                printf("Fizz, ");
            }
            else if (i % 5 == 0 && i % 3 != 0)
            {
                printf("Buzz, ");
            }
            else if (i % 5 == 0 && i % 3 == 0)
            {
                printf("Fizz Buzz, ");
            }
            else
            {
                printf("%d, ", i);
            }
        }
    }
    return 0;
}