#include<stdio.h>
void func_1(int);
int main() {
int x = 10;
    printf("Before function call\n");
    printf("x = %d\n", x);
func_1(x);
    printf("After function call\n");
    printf("x = %d\n", x);
return 0; }
void func_1(int a)
{
    a += 1;
    a++;
    printf("\na = %d\n\n", a);
}
