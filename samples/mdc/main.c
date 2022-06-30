#include <stdio.h>

int mdc(int a, int b);

int main()
{
    int a1 = 12, b1 = 9;
    int mdc1 = mdc(a1, b1);
    printf("mdc (9, 12) == %d\n", mdc1);

    int a2 = 40, b2 = 15;
    int mdc2 = mdc(a2, b2);
    printf("mdc (40, 15) == %d\n", mdc2);
}
