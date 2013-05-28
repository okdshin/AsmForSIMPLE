#ifdef ROUTINE_UNIT_TEST
#include "Routine.h"
#include <iostream>

using namespace asm_for_simple;

int main(int argc, char* argv[])
{
	std::cout << ToBitString(ToValue<int>("100"), 16) << std::endl;

    return 0;
}

#endif
