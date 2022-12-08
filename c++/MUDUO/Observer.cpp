#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<fstream>
#include<string>
using namespace std;

class Observer  // : boost::noncopyable
{
    public:
        virtual ~Observer();
        virtual void update() = 0;
};

class Observer  // : boost::noncopyable
{
    public:
        void register_(Observer* x);
        void unregister(Observer* x);

        void notifyObservers()
        {
            for(Observer* x : Observers_)
            {
                x->update();
            }
        }
    private:
        std::vector<Observer*> Observers;
};







