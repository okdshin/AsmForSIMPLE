#ifdef ASMFORSIMPLE_UNIT_TEST
#include "AsmForSIMPLE.h"
#include <iostream>

using namespace asm_for_simple;

int main(int argc, char* argv[])
{
	AsmForSIMPLE afs;
	if(argc != 1 && argc != 3){
		std::cout << "usage:\n\t"
			<< "no argument -> run interactive shell\n\t" 
			<< "<src> <dst> -> compile src file and write it dst file." 
			<< std::endl;	
		return 0;
	}
	if(argc==1){
		afs.StartInteractiveShell(std::cin, "save");
	}
	else {
		std::ifstream src_file(argv[1]);
		std::ofstream dst_file(argv[2]);
		afs.CompileAsmFile(src_file, dst_file);
    }
	return 0;
}

#endif
