#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<fstream>
#include<string>
using namespace std;

class Sales_data
{
private: //共有成员，外部接口
	string bookNo;
	unsigned units_sold;
	double price;
	double discount;
	double average;
	double revenue;
public:  //私有成员，外部无法访问
	Sales_data();
	friend istream& operator >> (istream&, Sales_data&);
	friend ostream& operator << (ostream&, Sales_data&);
	string isbn() const;
	friend Sales_data operator + (Sales_data &, Sales_data &);
	Sales_data& operator = (const Sales_data &);
	friend bool operator == (Sales_data &, Sales_data &);
	Sales_data & operator += (const Sales_data &);
	double avg_price();
	Sales_data& combine(const Sales_data &);
	friend istream & read(istream &, Sales_data &);
	friend ostream & print(ostream &, Sales_data &);
	friend Sales_data add(const Sales_data &, const Sales_data &);
};
Sales_data::Sales_data()
{
	bookNo = "NUll";
	units_sold = 0;
	price = 0.0;
}
istream& operator >> (istream &is, Sales_data &item)
{
	is >> item.bookNo >> item.units_sold >> item.price;
	return is;
}
ostream& operator <<(ostream &os, Sales_data &item)
{
	os << item.isbn() << " " << item.units_sold << " " << item.price;
	return os;
}
string Sales_data::isbn()const
{
	return bookNo;
}
Sales_data operator + (Sales_data & lhs, Sales_data & rhs)
{
	Sales_data ret;
	ret.bookNo = lhs.bookNo;
	ret.units_sold = lhs.units_sold + rhs.units_sold;
	ret.revenue = lhs.revenue + rhs.revenue;
	ret.avg_price();
	return ret;
}
bool operator == (Sales_data &lhs, Sales_data &rhs)
{
	return lhs.units_sold == rhs.units_sold&&lhs.price == rhs.price&&lhs.isbn() == rhs.isbn();
}
Sales_data& Sales_data::operator=(const Sales_data &lhs)
{
	bookNo = lhs.bookNo;
	units_sold = lhs.units_sold;
	price = lhs.price;
	return *this;
}
Sales_data& Sales_data::operator+=(const Sales_data &rhs)
{
	units_sold += rhs.units_sold;
	revenue += rhs.revenue;
	return *this;
}
double Sales_data::avg_price()
{
	average = revenue / units_sold;
	return average;
}
Sales_data& Sales_data::combine(const Sales_data &rhs)
{
	units_sold += rhs.units_sold;
	revenue += rhs.revenue;
	return *this;
}
istream & read(istream & is, Sales_data &rhs)
{
	is >> rhs.bookNo >> rhs.units_sold >> rhs.price;
	rhs.revenue = rhs.units_sold*rhs.price;
	return is;
}
Sales_data add(const Sales_data &lhs, const Sales_data &rhs)
{
	Sales_data sum = lhs;
	sum.combine(rhs);
	sum.avg_price();
	return sum;
}


inline bool compareIsbn(const Sales_data &lhs , const Sales_data &rhs)
{
    return lhs.isbn() < rhs.isbn();
}

int main(int argc , char * argv[])
{
    ifstream in(argv[1]);
    if(!in){
        cout << "open file flase!" << endl;
        exit(1);
    }

    vector<Sales_data> sds;
    Sales_data sd;
    while(read(in , sd))
    {
        sds.push_back(sd);
    } 
    //sort(sds.begin() , sds.end() , compareIsbn);
    sort(sds.begin() , sds.end() , [](const Sales_data & lhs , const Sales_data & rhs)
    {return lhs.isbn() < rhs.isbn();});

    for(const auto &s : sds)
    {
        //print(cout , s);
        cout << endl;
    }
}