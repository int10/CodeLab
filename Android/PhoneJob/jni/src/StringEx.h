#ifndef STRINGEX_H
#define STRINGEX_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cctype>
#include <algorithm>
#include "PcreReg.h"
#include "ListEx.h"

using namespace std;

class StringEx : public string
{
public:
	StringEx();
	StringEx(string s);
	StringEx(const char *s);
	~StringEx();

	bool Contain(string str, bool casesensitive = true);
	bool Contain(PcreReg &reg, bool casesensitive = true);
	string Replace(string oldstr, string newstr);
	string Replace(PcreReg &reg, string newstr);
	StringEx ToUpper();
	StringEx ToLower();
	ListEx<string> Split(const string str);
	StringEx operator = (string str);
	StringEx operator = (const char *str);
};

#endif // STRINGEX_H
