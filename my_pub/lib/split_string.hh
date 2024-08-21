#pragma once
#include <iostream>
#include <vector>
#include <string>

using namespace std;
vector<string> split_string(const string &str, string delims, int inc = 0);
vector<string> split_string(const string &str, string delims, int inc)
{
	std::vector<std::string> result;
	size_t i = str.rfind(delims, str.length());
	if (i == str.length() - 1 || i == string::npos)
	{
		result.push_back(str);
	}
	else
	{
		result.push_back(str.substr(i + inc, str.length() - i));
		result.push_back(str.substr(0, i));
	}
	return result;
}