// test_dll.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "Start.h"
#include <iostream> //cout


int main()
{
	std::cout << "From dll:" << getX() << std::endl;
	int x;
	std::cin >> x;

	std::cout << "From dll: Start??";
	start();
	std::cin >> x;
	return 0;
}

