#include<stdio.h>

/*
非递归
int func(int n)
{
    int f1 = 1;
    int f2 = 1;
    int temp = 0;
    int i = 0;
    if(n < 3)
    {
        return 1;
    }
    else
    {
        for(i = 0;i <=n-3 ; i++)
        {
            temp = f1+f2;
            f1 = f2;
            f2 = temp;
        }
        return temp;
    }
}

int main()
{
    int n;
    scanf("%d",&n);
    printf("%d" , func(n));
    
}

---------------------------------------------------------------

递归
int func(int n)
{
    if(n == 1 || n == 2)
    {
        return 1;
    }
    else
    {
        return func(n-1)+func(n-2);
    }
    
}

int main()
{
    int n;
    scanf("%d",&n);
    printf("%d" , func(n));
}
*/

