
#include<iostream>
#include<cstdio>
#include<string.h>

using namespace std;
char a[1000];
int main()
{
	int n;
	string ss;
	cin>>n;
	getchar();
	for(int i=0;i<n;i++)
	{
		//fgets(a , strlen(a) , stdin); //格式错误
		//gets(a);
		puts(a);
		cout<<endl; 
	}
	while(cin>>ss)
	{
		cout<<ss<<endl << endl;
	 } 
	 return 0;
	
}
