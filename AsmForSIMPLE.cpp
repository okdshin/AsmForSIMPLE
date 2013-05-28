#ifdef ASMFORSIMPLE_UNIT_TEST
#include "AsmForSIMPLE.h"
#include <iostream>

using namespace asm_for_simple;

int main(int argc, char* argv[])
{
	AsmForSIMPLE afs;
	if(argc != 1 && argc != 3){
		std::cout << "usage" << std::endl;	
	}
	if(argc==1){
		afs.StartInteractiveShell(std::cin, "save");
	}
	else {
		std::string src_file_name(argv[1]);
		std::string dst_file_name(argv[2]);
		std::ifstream src_file(src_file_name.c_str());
		std::ofstream dst_file(dst_file_name.c_str());
		afs.CompileAsmFile(src_file, dst_file);
    }
	return 0;
}

#endif
