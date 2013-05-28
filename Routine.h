#pragma once
//Routine:20130528
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cassert>

namespace asm_for_simple
{
auto SplitString(
		const std::string& src, const std::string& delim) -> std::vector<std::string> {
	std::string::size_type start = 0;
	std::vector<std::string> dst_vect;
	while(true){
		const auto end = src.find(delim, start);
		if(end != std::string::npos){
			dst_vect.push_back(src.substr(start, end - start));
		}
		else {
			dst_vect.push_back(src.substr(start, src.length() - start));	
			break;
		}
		start = end + delim.length();
	}
	return dst_vect;
}

template<class T>
auto ToValue(const std::string& str) -> T {
	std::istringstream iss(str);
	T val;
	iss >> val;
	return val;
}
template<class T>
auto ToString(T val) -> std::string {
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

//TODO minus value
auto ToBitString(int num, unsigned int bit_length) -> std::string {
	std::string result;
	while(num){
		result.append(ToString<int>(num%2));
		num = num/2;
	}
	assert(result.length() <= 16);
	const auto append_length = bit_length - result.length();
	for(unsigned int i=0; i < append_length; ++i){
		result.append("0");
	}
	std::reverse(result.begin(), result.end());
	return result;
}
}

