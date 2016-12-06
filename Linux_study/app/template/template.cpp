#include<iostream>
#include <string>
using namespace std;

class BaseA{
public:
    BaseA(){cout<<"BaseA founed"<<endl;}
};
class BaseB{
public:
    BaseB(){cout<<"BaseB founed"<<endl;}
};
template<typename T>
class BaseC{
private:
    T data;
public:
    BaseC(){
        cout<<"BaseC founed "<< sizeof(T) << endl;}
};
template<class T> class Derived:public BaseC<T>{
public:
    Derived(){cout<<"Derived founed"<<endl;}
};


int main()
{
    Derived<BaseA> x;// BaseA��Ϊ����
    Derived<BaseB> y;// BaseB��Ϊ����
	BaseC<int> b;
	Derived<long long> z;
    Derived<BaseC<int, 3> > z; // BaseC<int,3>��Ϊ����
}