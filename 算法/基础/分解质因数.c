#include<stdio.h>

int prime(int n)
{
    for(int i = 2 ; i < n/2 ; i++)
    {
        if(n % i == 0)
            return 1;
        return 0;
    }
}

int func(int n)
{
    int m = n;
    for(int j = 2 ; j < m/2 ; j++)
    {
        while(n % j == 0)
        {
            printf("%d " , j);
            if(n / j > 2)
                printf("* ");
            n/=j;
        }
    }
}

int main()
{
    int n;
    scanf("%d" , &n);
    func(n);
}          