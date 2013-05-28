#pragma once
//Exception:20130529
#include <iostream>
#include "Routine.h"
namespace asm_for_simple
{
class ArgumentNumException{
public:
	ArgumentNumException(const std::string& comment, int size, int need) : 
		comment_(comment), size_(size), need_(need){}

	auto what()const -> const char* {
		return std::string(
			comment_+"(need is "+ToString(need_)+" but your size is "+ToString(size_)+")").c_str();	
	}
private:
	std::string comment_;
	int size_, need_;
};

class TooFewArgumentsException : public std::exception {
public:
	TooFewArgumentsException(int size, int need) : std::exception(), 
		e_("too few arguments.", size, need){}

	virtual auto what()const throw() -> const char* {
		return e_.what();	
	}
private:
	ArgumentNumException e_;
};

class TooMutchArgumentsException : public std::exception {
public:
	TooMutchArgumentsException(int size, int need) : std::exception(), 
		e_("too mutch arguments.", size, need){}

	virtual auto what()const throw() -> const char* {
		return e_.what();	
	}
private:
	ArgumentNumException e_;
};

class InvalidCommandKeywordException : public std::exception {
public:
	InvalidCommandKeywordException(const std::string& invalid_command) : 
		std::exception(), invalid_command_(invalid_command){}
	virtual auto what()const throw() -> const char* {
		return ("invalid command keyword:" + invalid_command_).c_str();
	}
private:
	std::string invalid_command_;
};

}

