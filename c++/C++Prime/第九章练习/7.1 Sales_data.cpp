#include<iostream>
#include<string>
using namespace std;


struct Sales_data
{
    Sales_data();
    string bookNo;            //ISBN编号
    unsigned units_sold;      //本书销量
    double revenue;           //总收入
};
Sales_data::Sales_data()
{
    units_sold = 0;
    revenue = 0.0;
}

int main()
{
    Sales_data total;
    if(cin >> total.bookNo >> total.revenue >> total.units_sold)
    {
        Sales_data trans;
        while(cin >> trans.bookNo >> trans.units_sold >> trans.revenue)
        {
            if(trans.bookNo == total.bookNo)
            {
                total.bookNo += trans.bookNo;
                total.revenue += trans.revenue;
            }
            else
            {
                //输出结果
                cout << "total.bookNo = " << total.bookNo << "  total.revenue = " << total.revenue 
                << "   total.units.sold = " << total.units_sold << endl;
                total = trans;             //处理下一本书
            }           
        }
        cout << total.bookNo << " " << total.units_sold << " " << total.revenue
			<< endl;
    }
    else
    {
        cout << "No data !!" << endl;
    }
    return 0;
}
