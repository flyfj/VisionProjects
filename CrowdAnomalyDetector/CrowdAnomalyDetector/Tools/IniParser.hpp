#pragma once

#include <string>
#include <windows.h>
using namespace std;

class IniParser
{
public:
	IniParser(const string& ini_file) { ini_fn = ini_file; }

	string ReadString(const string& section_name, const string& key_name);
	int ReadInt(const string& section_name, const string& key_name);
	float ReadFloat(const string& section_name, const string& key_name);

private:
	string ini_fn;
};

//////////////////////////////////////////////////////////////////////////

string IniParser::ReadString(const string& section_name, const string& key_name) {
	char str[300];
	GetPrivateProfileStringA(section_name.c_str(), key_name.c_str(), "", str, sizeof(str), ini_fn.c_str());
	return string(str);
}

int IniParser::ReadInt(const string& section_name, const string& key_name) {
	return GetPrivateProfileIntA(section_name.c_str(), key_name.c_str(), 0, ini_fn.c_str());
}

float IniParser::ReadFloat(const string& section_name, const string& key_name) {
	string str = ReadString(section_name, key_name);
	return stof(str);
}