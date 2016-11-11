#include "Application.h"

#include <iostream>
#include <exception>

using namespace std;

int main()
{
	try
	{
		Application app;
		app.Start();
	}
	catch (const exception& ex)
	{
		cout << "Application throws exception:" << endl;
		cout << ex.what() << endl;
		cout << "Application will be closed" << endl;
		return 1;
	}

    return 0;
}
