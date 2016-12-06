#ifndef PCREREG_H
#define PCREREG_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <list>
#include "pcre/pcre2.h"

using namespace std;

class PcreReg
{
public:
	PcreReg();
	PcreReg(string pattern);
	~PcreReg();
	int IndexIn(const string str, int pos);
	string Cap(int index);
	void SetGreedy(bool greedy);
	void SetCaseSensitive(bool sen);
private:
	string m_str;
	string m_pattern;
	list<string> m_capstrlist;
	bool m_greedy;
	bool m_casesensitive;
};

#endif // PCREREG_H
