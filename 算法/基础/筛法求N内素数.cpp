#include<iostream>
using namespace std;
int isPrime(int n)
{
    for(int i=2 ; i < n/2 ; i++)
    {
        if(n%i == 0)
        {
            return 0;
        }
    }
    return 1;
}
int main()
{
    int n;
    cin >> n;
    int prime[n]={0,1,1,1,0,1};
    for(int i = 6 ; i < n ; i++)
    {
        if(isPrime(i))
        {
            prime[i] = 1;
        }
        else
        {
            prime[i] = 0;
            int j = 2;
            while(j*i < n)
            {
                prime[j*i] = 0;
                j++;
            }
        }
    }
    for(int i=2;i<n;i++)
    {
        if(prime[i])
        {
            cout << i << endl;
        }
    }
}