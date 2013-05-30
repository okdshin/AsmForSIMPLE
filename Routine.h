#pragma once
//Routine:20130528
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cassert>

namespace asm_for_simple
{
inline auto TrimStringLeft(std::string s) -> std::string {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

inline auto TrimStringRight(std::string s) -> std::string {
	s.erase(std::find_if(s.rbegin(), s.rend(), 
	std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

inline auto TrimString(const std::string &s) -> std::string {
        return TrimStringLeft(TrimStringRight(s));
}

inline auto SplitAndTrimString(
		const std::string& src, const std::string& delim) -> std::vector<std::string> {
	std::string::size_type start = 0;
	std::vector<std::string> dst_vect;
	while(true){
		const auto end = src.find(delim, start);
		if(end != std::string::npos){
			dst_vect.push_back(src.substr(start, end - start));
		}
		else {
			dst_vect.push_back(
				TrimString(src.substr(start, src.length() - start)));	
			break;
		}
		start = end + delim.length();
	}
	/*
	dst_vect.erase(std::remove(dst_vect.begin(), dst_vect.end(), ""), 
		dst_vect.end());
	*/
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
inline auto ToBitString(int num, unsigned int bit_length) -> std::string {
	std::string result;
	bool is_signed = false;
	if(num < 0){
		is_signed = true;
		num = -num;
	}
	while(num){
		result.append(ToString<int>(num%2));
		num = num/2;
	}
	assert(result.length() <= 16);
	const auto append_length = bit_length - result.length();
	for(unsigned int i=0; i < append_length; ++i){
		result.append("0");
	}
	if(is_signed){
		for(unsigned int i=0; i < result.length(); ++i){
			result.at(i) = (result.at(i) == '1') ? '0' : '1';	
		}
		unsigned int i = 0;
		while(result.at(i) == '1'){
			result.at(i) = '0';
			++i;
		}
		result.at(i) = '1';
	}
	std::reverse(result.begin(), result.end());
	return result;
}
}

