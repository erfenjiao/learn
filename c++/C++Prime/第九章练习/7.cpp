#include<iostream>
#include<vector>
#include<string>
using namespace std;

/*
struct Sales_data
{
   string isbn() const {return bookNo;}
   Sales_data& combin(const Sales_data&);
   double avg_price() const;
   string bookNo;
   unsigned units_sold = 0;
   double revenue = 0.0;
};

Sales_data add(const Sales_data& , const Sales_data&);
ostream &print(ostream& , const Sales_data&);
istream &read(istream& , Sales_data&);
*/
//-----------------7.2\3--------------------------
/*
struct Sales_data
{
   string isbn() const {return bookNo;}
   Sales_data& combin(const Sales_data&);
   double avg_price() const;
   string bookNo;
   unsigned units_sold = 0;
   double revenue = 0.0;
};
 
int main(int argc, char **argv)
{
	Sales_data total;//此部分和第一章21页的内容相同
	if (cin >> total.bookNo >> total.units_sold >> total.revenue) {
		Sales_data trans;
		while (cin >> trans.bookNo >> trans.units_sold >> trans.revenue) {
			if (total.bookNo == trans.bookNo) {
				total.units_sold += trans.units_sold;
				total.revenue += trans.revenue;//第三题的使用修改之处
			}
			else {
				cout << total.bookNo << " " << total.units_sold << " "
					<< total.revenue << endl;
				total = trans;
			}
		}
		cout << total.bookNo << " " << total.units_sold << " " << total.revenue
			<< endl;
	}
	else {
		std::cerr << "No data?!" << std::endl;
		return -1;
	}
	return 0;
}
*/

//-----------------7.4--------------------------
/*
class Person
{
public:
   Person(string client_name , string client_address)
   {
         name = client_name;
         address = client_address;
   }
   void Show_Info()
   {
      cout << "name = " << name << "address = " << address << endl;
   }
   string name;
   string address;
};

istream &read(istream &is , Person& person)
{
   is >>person.name >> person.address ;
   // if(is)
   //    person = person();
   return is;
}

ostream& print(ostream& os, const Person& person)
{
    os << person.name << " " << person.address;
    return os;
}

int main()
{
   Person p1("zhangsan" , "xiyou");
   p1.Show_Info();
}
*/
//-----------------7.23--------------------------

class Screen
{
   public:
      typedef string::size_type pos;                    //using pos = string::size_type
      Screen() = default;                               //  =　default 告诉编译器为我们生成默认的构造函数
      Screen(pos ht , pos wd , char c):h(ht) , w(wd),contents(ht * wd , c){}                          //用于保存Screen内容
      char get() const{return contents[cursor];}       //隐式内联

      inline char get(pos ht , pos wd) const;          //显式内联
      Screen &move(pos r , pos c);                    //能在之后被设置为内联
      Screen &set(char);                               //负责光标所处位置的字符和其他任意位置的字符
      Screen &set(pos , pos , char);

   private:
      pos cursor = 0;         //光标的位置
      pos h = 0 , w = 0;      // 高和宽
      string contents;        //用于保存Screen内容
};

inline Screen &Screen::set(char c)
{
   contents[cursor] = c;       //设置当前光标位置所处的新值
   return *this;
}


inline Screen &Screen::set(pos r , pos col , char ch)
{
   contents[r*w + col] = ch;
   return *this;
}
{
   contents[cursor] = c;       //设置当前光标位置所处的新值
   return *this;
}

class Window_mgr{
   private:
      vector<Screen> screens{Screen{24 , 80 , ' '}};
};