#include <iostream>

using namespace std;

int main()
{
    int n , m , sum;
    cin >> m ;
    while(m--)
    {
        cin >> n >> sum;
        int a[n+1][n+1];
        //int m = (n+1)/2;
        for(int i = 1 ; i <= n ; i++)
        {
            for(int j = 1 ; j <= n ; j++)
            {
                a[i][j] = 0;
                if(i == j)
                {
                    a[i][j] = 1;
                }
                a[i][n-i+1] = 1;

            }
        }

        while(sum--)
        {
            for(int i = 1 ; i <= n-1 ; i++)
            {
                for(int j = 1 ; j <= n ; j++)
                {
                    if(a[i][j] == 1)
                    {
                        cout << "X";
                    }
                    else
                    {
                        cout << " ";
                    }
                }
                cout << endl;
            }
           
        }
        
        for(int i=n;i<=n;i++)
        {
            for(int j=1;j<=n;j++)
            {
                if(a[i][j] == 1)
                {
                    cout << "X";
                }
                else
                {
                    cout << " ";
                }
            }
        }
        cout << endl;
    }
    
}