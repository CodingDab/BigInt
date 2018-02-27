#include <iostream>
#include "BigInt.h"

int main()
{
	try
	{
		BigInt x = "15";
		BigInt y = "10000";
		std::string s = "sum";
		BigInt oldX = x;
		x += y;
		std::cout << "The " << s << " of " << oldX << " and " << y << " is " << x << std::endl;
	}
	catch(const char* e)
	{
		std::cout << e << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << "Something failed..." << std::endl;
	}
	return 0;
}
