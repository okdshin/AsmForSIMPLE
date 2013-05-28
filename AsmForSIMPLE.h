#pragma once
//AsmForSIMPLE:20130525
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <algorithm>
#include "Routine.h"

namespace asm_for_simple
{

class Command {
public:
	using Ptr = std::shared_ptr<Command>;

	Command(){}
	auto Convert(int x, int y, int z) -> std::string {
		return this->DoConvert(x, y, z);	
	}
private:
	virtual auto DoConvert(int x, int y, int z) -> std::string = 0;

};

class OpCommand : public Command {
public:
	static auto Create(const std::string& op3) -> Command::Ptr {
		return Command::Ptr(new OpCommand(op3));
	}
private:
	OpCommand(const std::string& op3) : Command(), op3(op3){}
	virtual auto DoConvert(int x, int y, int z) -> std::string {
		return "11"+ToBitString(y, 3)+ToBitString(x, 3)+op3+"0000";
	}

	std::string op3;
};

class ShiftCommand : public Command {
public:
	static auto Create(const std::string& op3) -> Command::Ptr {
		return Command::Ptr(new ShiftCommand(op3));
	}
private:
	ShiftCommand(const std::string& op3) : Command(), op3(op3){}
	virtual auto DoConvert(int x, int y, int z) -> std::string {
		return "11000"+ToBitString(x, 3)+op3+ToBitString(y, 4);
	}

	std::string op3;
};

class LoadStoreCommand : public Command {
public:
	static auto Create(const std::string& op1) -> Command::Ptr {
		return Command::Ptr(new LoadStoreCommand(op1));
	}
private:
	LoadStoreCommand(const std::string& op1) : Command(), op1(op1){}
	virtual auto DoConvert(int x, int y, int z) -> std::string {
		return op1+ToBitString(x, 3)+ToBitString(z, 3)+ToBitString(y, 8);
	}

	std::string op1;
};

class LoadImAndJumpCommand : public Command {
public:
	static auto Create(const std::string& op2) -> Command::Ptr {
		return Command::Ptr(new LoadImAndJumpCommand(op2));
	}
private:
	LoadImAndJumpCommand(const std::string& op2) : Command(), op2(op2){}
	virtual auto DoConvert(int x, int y, int z) -> std::string {
		return "10"+op2+ToBitString(x, 3)+ToBitString(y, 8);
	}

	std::string op2;
};

class BranchCommand : public Command {
public:
	static auto Create(const std::string& cond) -> Command::Ptr {
		return Command::Ptr(new BranchCommand(cond));
	}
private:
	BranchCommand(const std::string& cond) : Command(), cond(cond){}
	virtual auto DoConvert(int x, int y, int z) -> std::string {
		return "10111"+cond+ToBitString(y, 8);
	}

	std::string cond;
};

class AsmForSIMPLE{
public:
    AsmForSIMPLE(){
		command_map_["ADD"] = OpCommand::Create("0000");
		command_map_["SUB"] = OpCommand::Create("0001");
		command_map_["AND"] = OpCommand::Create("0010");
		command_map_["OR"] =  OpCommand::Create("0011");
		command_map_["XOR"] = OpCommand::Create("0100");
		command_map_["CMP"] = OpCommand::Create("0101");
		command_map_["MOV"] = OpCommand::Create("0110");
		command_map_["SLL"] = ShiftCommand::Create("1000");
		command_map_["SLR"] = ShiftCommand::Create("1001");
		command_map_["SRL"] = ShiftCommand::Create("1010");
		command_map_["SRA"] = ShiftCommand::Create("1011");
		command_map_["IN"] =  OpCommand::Create("1100");
		command_map_["OUT"] = OpCommand::Create("1101");
		command_map_["HLT"] = OpCommand::Create("1111");
		command_map_["LD"] =  LoadStoreCommand::Create("00");
		command_map_["ST"] =  LoadStoreCommand::Create("01");
		command_map_["LI"] =  LoadImAndJumpCommand::Create("000");
		command_map_["B"] =   LoadImAndJumpCommand::Create("100");
		command_map_["BE"] =  BranchCommand::Create("000");
		command_map_["BLT"] = BranchCommand::Create("001");
		command_map_["BLE"] = BranchCommand::Create("010");
		command_map_["BNE"] = BranchCommand::Create("011");
	}

    ~AsmForSIMPLE(){}

	auto StartInteractiveShell(std::istream& is, const std::string& save_command) -> void {
		std::cout << "\"!" << save_command << "\" is save and quit." << std::endl;
		while(true){
			std::string line;
			std::getline(is, line);
			if(line == "!"+save_command){
				std::cout << "SaveFileName?: " << std::flush;
				std::string file_name;
				std::cin >> file_name;
				std::ofstream ofs(file_name.c_str());
				for(const auto& bin_line : binary_lines_){
					ofs << bin_line << std::endl;
				}
				break;
			}
			const auto bin_line = this->CompileLine(line);
			std::cout << bin_line << std::endl;
			binary_lines_.push_back(bin_line);
		}
	}


	auto CompileLine(const std::string& asm_code) -> std::string {
		auto tokens = SplitString(asm_code, " ");
		assert(tokens.size() <= 4);
		const unsigned int append_token_num = 4 - tokens.size();
		for(unsigned int i=0; i < append_token_num; ++i){
			tokens.push_back("0");
		}
		return command_map_[tokens.at(0)]->Convert(
			ToValue<int>(tokens.at(1)), 
			ToValue<int>(tokens.at(2)), 
			ToValue<int>(tokens.at(3)));
	}

	auto CompileAsmFile(std::ifstream& ifs, std::ofstream& ofs) -> void {
		while(ifs){
			std::string line;
			std::getline(ifs, line);
			ofs << this->CompileLine(line) << std::endl;
		}
	}
private:
	std::map<std::string, Command::Ptr> command_map_;
	std::vector<std::string> binary_lines_;
};
}
