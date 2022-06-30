#include <stdio.h>

int fat(int n);

int main()
{
    int a = 5;
    int fat_a = fat(a);
    printf("5! == %d\n", fat_a);

    int b = 6;
    int fat_b = fat(b);
    printf("6! == %d\n", fat_b);
}
