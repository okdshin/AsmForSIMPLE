#ifdef ROUTINE_UNIT_TEST
#include "Routine.h"
#include <iostream>

using namespace asm_for_simple;

int main(int argc, char* argv[])
{
	std::cout << ToBitString(ToValue<int>("4"), 16) << std::endl;
	std::cout << ToBitString(ToValue<int>("-4"), 16) << std::endl;
	std::cout << ToBitString(ToValue<int>("1"), 16) << std::endl;
	std::cout << ToBitString(ToValue<int>("-1"), 16) << std::endl;

    return 0;
}

#endif
