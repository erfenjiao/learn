#include <iostream>

using namespace std;

int main()
{
    int n , count , c = 1 , b = 1;
    cin >> n;
    int a[n+1][n+1];
    for(int i = 1 ; i <= n ; i++)
    {
        count = i+1;
        a[i][1] = c;
        c += b;
        b++;
        for(int j = 2 ; j <= n ; j++)
        {
            a[i][j] = a[i][j-1]+count;
            count++;
        }
    }
    for(int i = 1 ; i <= n ; i++)
    {
        for(int j = 1 ; j <= n-i+1 ; j++)
        {
            cout << a[i][j] << " ";
        }
        cout << endl;
    }
}