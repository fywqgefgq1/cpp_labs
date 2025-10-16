#include "module1.h"
#include "module2.h"
#include <iostream>

namespace Module3
{
    std::string getMyName()
	{
		std::string name = "Peter";
		return name;
	}
}

int main(int argc, char** argv)
{

    using namespace std; //можем так сделать т.к. в std нет функций getMyName

	cout <<  "Hello world!" << "\n";
	
	cout << Module1::getMyName() << "\n";
	cout << Module2::getMyName() << "\n";

	using namespace Module1;
	cout << getMyName() << "\n"; // (A)
	cout << Module2::getMyName() << "\n";

	using namespace Module2; // (B)
	// cout << getMyName() << "\n"; 
    //getmyname есть и в поле Module1, и в поле Module2, и они после использования Module2 будет конфликт 

	using Module2::getMyName;
	cout << getMyName() << "\n"; // (D)

    cout<<Module3::getMyName()<<"\n";

}
