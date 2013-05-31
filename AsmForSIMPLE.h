#pragma once
//AsmForSIMPLE:20130525
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <algorithm>
#include <exception>
#include "Routine.h"
#include "Exception.h"

namespace asm_for_simple
{

class Command {
public:
	using Ptr = std::shared_ptr<Command>;

	Command(){}

	auto CheckCommand(const std::vector<std::string>& tokens) -> void {
		this->DoCheckCommand(tokens);	
	}

	auto Convert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		return this->DoConvert(x, y, z);	
	}
private:
	virtual auto DoCheckCommand(const std::vector<std::string>& tokens) -> void = 0;
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string = 0;

};

auto CheckArgumentNum(unsigned int size, unsigned int need) -> void {
	/*
	if(size > need){
		throw TooMutchArgumentsException(size, need);
	}
	*/
	if(size < need){
		throw TooFewArgumentsException(size, need);	
	}
}

class OpCommand : public Command {
public:
	static auto Create(const std::string& op3) -> Command::Ptr {
		return Command::Ptr(new OpCommand(op3));
	}
private:
	OpCommand(const std::string& op3) : Command(), op3(op3){}
	virtual auto DoCheckCommand(const std::vector<std::string>& tokens) -> void {
		unsigned int need = 2;
		if(tokens.at(0) == "OUT"){ need = 1; }
		if(tokens.at(0) == "HLT"){ need = 0; }
		CheckArgumentNum(tokens.size()-1, need);
	}
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		return "11"+ToBitString(ToValue<int>(y), 3)+ToBitString(ToValue<int>(x), 3)+op3+"0000";
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
	virtual auto DoCheckCommand(const std::vector<std::string>& tokens) -> void {
		CheckArgumentNum(tokens.size()-1, 2);
	}
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		return "11000"+ToBitString(ToValue<int>(x), 3)+op3+ToBitString(ToValue<int>(y), 4);
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
	virtual auto DoCheckCommand(const std::vector<std::string>& tokens) -> void {
		CheckArgumentNum(tokens.size()-1, 3);
	}
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		return op1+ToBitString(ToValue<int>(x), 3)+
			ToBitString(ToValue<int>(z), 3)+ToBitString(ToValue<int>(y), 8);
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
	virtual auto DoCheckCommand(const std::vector<std::string>& tokens) -> void {
		CheckArgumentNum(tokens.size()-1, 2);
	}
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		return "10"+op2+ToBitString(ToValue<int>(x), 3)+ToBitString(ToValue<int>(y), 8);
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
	virtual auto DoCheckCommand(const std::vector<std::string>& tokens) -> void {
		CheckArgumentNum(tokens.size()-1, 1);
	}
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		return "10111"+cond+ToBitString(ToValue<int>(x), 8);
	}

	std::string cond;
};
class LabelJumpCommand : public Command {
public:
	static auto Create(
			const std::shared_ptr<std::map<std::string, int>>& label_map,
			const std::shared_ptr<int>& current_line_num) -> Command::Ptr {
		return Command::Ptr(new LabelJumpCommand(label_map, current_line_num));
	}
private:
	LabelJumpCommand(
		const std::shared_ptr<std::map<std::string, int>>& label_map,
		const std::shared_ptr<int>& current_line_num) : 
			Command(), label_map_(label_map), current_line_num_(current_line_num), 
			jump_command_(LoadImAndJumpCommand::Create("100")){}
	auto DoCheckCommand(const std::vector<std::string>& tokens) -> void {
		CheckArgumentNum(tokens.size()-1, 1);
	}
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		if(label_map_->find(x) == label_map_->end()){
			throw InvalidLabelException(x);	
		}
		const auto new_x_str = ToString((*label_map_)[x]-(*current_line_num_));
		//std::cout << new_x_str << std::endl;
		return jump_command_->Convert("0", new_x_str, "0");
	}

	std::shared_ptr<std::map<std::string, int>> label_map_;
	std::shared_ptr<int> current_line_num_;
	Command::Ptr jump_command_;
};
class LabelBranchCommand : public Command {
public:
	static auto Create(
			const std::string& cond,
			const std::shared_ptr<std::map<std::string, int>>& label_map,
			const std::shared_ptr<int>& current_line_num) -> Command::Ptr {
		return Command::Ptr(new LabelBranchCommand(cond, label_map, current_line_num));
	}
private:
	LabelBranchCommand(
		const std::string& cond,
		const std::shared_ptr<std::map<std::string, int>>& label_map,
		const std::shared_ptr<int>& current_line_num) : 
			Command(), label_map_(label_map), current_line_num_(current_line_num), 
			branch_command_(BranchCommand::Create(cond)){}
	auto DoCheckCommand(const std::vector<std::string>& tokens) -> void {
		CheckArgumentNum(tokens.size()-1, 1);
	}
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		if(label_map_->find(x) == label_map_->end()){
			throw InvalidLabelException(x);	
		}
		const auto new_x_str = ToString((*label_map_)[x]-(*current_line_num_));
		//std::cout << new_x_str << std::endl;
		return branch_command_->Convert(new_x_str, "0", "0");
	}

	std::shared_ptr<std::map<std::string, int>> label_map_;
	std::shared_ptr<int> current_line_num_;
	Command::Ptr branch_command_;
};
class ImmediateLabelBranchCommand : public Command {
public:
	static auto Create(
			const std::string& op2,
			const std::shared_ptr<std::map<std::string, int>>& label_map,
			const std::shared_ptr<int>& current_line_num) -> Command::Ptr {
		return Command::Ptr(new ImmediateLabelBranchCommand(op2, label_map, current_line_num));
	}
private:
	ImmediateLabelBranchCommand(
		const std::string& op2_,
		const std::shared_ptr<std::map<std::string, int>>& label_map,
		const std::shared_ptr<int>& current_line_num) : 
			Command(), label_map_(label_map), current_line_num_(current_line_num), 
			op2_(op2_){}
	auto DoCheckCommand(const std::vector<std::string>& tokens) -> void {
		CheckArgumentNum(tokens.size()-1, 3);
	}
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		if(label_map_->find(z) == label_map_->end()){
			throw InvalidLabelException(z);	
		}
		const auto new_z = (*label_map_)[z]-(*current_line_num_);
		//std::cout << new_x_str << std::endl;
		return "10"+ToBitString(ToValue<int>(y), 3)
			+ToBitString(ToValue<int>(x), 3)+ToBitString(new_z, 5);
	}

	std::shared_ptr<std::map<std::string, int>> label_map_;
	std::shared_ptr<int> current_line_num_;
	std::string op2_;
};
class ImmediateAddCommand : public Command {
public:
	static auto Create() -> Command::Ptr {
		return Command::Ptr(new ImmediateAddCommand());
	}
private:
	ImmediateAddCommand() : Command(){}
	auto DoCheckCommand(const std::vector<std::string>& tokens) -> void {
		CheckArgumentNum(tokens.size()-1, 2);
	}
	virtual auto DoConvert(
			const std::string& x, 
			const std::string& y, 
			const std::string& z) -> std::string {
		return "10001"+ToBitString(ToValue<int>(x), 3)+
			ToBitString(ToValue<int>(y), 8);
	}
};
class AsmForSIMPLE{
public:
    AsmForSIMPLE() : 
			label_map_(new std::map<std::string, int>), current_line_num_(new int(0)){
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

		command_map_["&B"] = LabelJumpCommand::Create(label_map_, current_line_num_);
		command_map_["&BE"] = 
			LabelBranchCommand::Create("000", label_map_, current_line_num_);
		command_map_["&BLT"] = 
			LabelBranchCommand::Create("001", label_map_, current_line_num_);
		command_map_["&BLE"] = 
			LabelBranchCommand::Create("010", label_map_, current_line_num_);
		command_map_["&BNE"] = 
			LabelBranchCommand::Create("011", label_map_, current_line_num_);
		
		command_map_["&BEI"] = 
			ImmediateLabelBranchCommand::Create("010", label_map_, current_line_num_);
		command_map_["&BLTI"] = 
			ImmediateLabelBranchCommand::Create("011", label_map_, current_line_num_);
		command_map_["&BLEI"] = 
			ImmediateLabelBranchCommand::Create("101", label_map_, current_line_num_);
		command_map_["&BNEI"] = 
			ImmediateLabelBranchCommand::Create("110", label_map_, current_line_num_);
		command_map_["ADDI"] = 
			ImmediateAddCommand::Create();
	}

    ~AsmForSIMPLE(){}

	auto StartInteractiveShell(std::istream& is, const std::string& save_command) -> void {
		std::cout << "\"!" << save_command << "\" is save and quit." << std::endl;
		while(true){
			++(*current_line_num_);
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
			try{
				const auto bin_line = this->CompileLine((*current_line_num_), line);
				std::cout << bin_line << std::endl;
				binary_lines_.push_back(bin_line);
			}
			catch(const std::exception& e){
				std::cout << "Error:" << e.what() << std::endl;
			}
		}
	}

	auto CheckLine(int line_num, const std::string& asm_code) -> void {
		auto tokens = SplitAndTrimString(asm_code, " ");
		if(tokens.at(0) == "&LABEL"){ 
			const auto size = label_map_->size();
			(*label_map_)[tokens.at(1)] = line_num-size-1;
		}	
	}

	auto CompileLine(int line_num, const std::string& asm_code) -> std::string {
		auto tokens = SplitAndTrimString(asm_code, " ");
		if(tokens.at(0).empty() || tokens.at(0) == "&LABEL"){
			return "";	
		} 
		if(command_map_.find(tokens.at(0)) == command_map_.end()){
			throw InvalidCommandKeywordException(tokens.at(0));	
		}
		command_map_[tokens.at(0)]->CheckCommand(tokens);
		const unsigned int append_token_num = 4 - tokens.size();
		for(unsigned int i=0; i < append_token_num; ++i){
			tokens.push_back("0");
		}
		return command_map_[tokens.at(0)]->Convert(
			tokens.at(1), tokens.at(2), tokens.at(3));
	}

	auto CompileAsmFile(std::ifstream& ifs, std::ofstream& ofs) -> void {
		(*current_line_num_) = 0;
		while(ifs){
			++(*current_line_num_);
			std::string line;
			std::getline(ifs, line);
			CheckLine((*current_line_num_), line);
		}
		ifs.clear();
		ifs.seekg(0, std::ios_base::beg);
		(*current_line_num_) = 0;
		while(ifs){
			++(*current_line_num_);
			std::string line;
			std::getline(ifs, line);
			try{
				const auto bin_code = this->CompileLine(*current_line_num_, line); 
				if(bin_code.empty()){
					--(*current_line_num_);
				}
				else {
					ofs << (*current_line_num_)-1 << ":" << bin_code << ";" << std::endl;
				}
			}
			catch(const std::exception& e){
				std::cout << (*current_line_num_) << ":Error:" << e.what() << std::endl;
			}
		}
	}
private:
	std::map<std::string, Command::Ptr> command_map_;
	std::shared_ptr<std::map<std::string, int>> label_map_;
	std::shared_ptr<int> current_line_num_;
	std::vector<std::string> binary_lines_;
};
}
