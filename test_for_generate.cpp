
#include "pch.h"

#include <iostream>
#include "Generate.h"

int main()
{
	std::cout << generate("Broj") << std::endl;
	std::cout << generate("Kviz") << std::endl;
	std::cout << generate("Loto") << std::endl;
	std::cout << generate("Poker") << std::endl;
}