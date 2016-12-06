#include "StringEx.h"

StringEx::StringEx() : string()
{
}

StringEx::StringEx(string s) : string(s)
{
}

StringEx::StringEx(const char *s) : string(s)
{
}

StringEx::~StringEx()
{
}

StringEx StringEx::operator = (string str)
{
	this->string::operator = (str);
	return *this;
}

StringEx StringEx::operator = (const char *str)
{
	this->string::operator = (str);
	return *this;
}

bool StringEx::Contain(string str, bool casesensitive)
{
	if(casesensitive) {
		if((size_t) -1 == this->find(str, 0)) {
			return false;
		}
	} else {
		StringEx tmp = *this;
		StringEx tmp2;
		tmp2 = str;
		tmp.ToLower();
		tmp2.ToLower();

		if((size_t) -1 == tmp.find(tmp2, 0)) {
			return false;
		}
	}
	return true;
}

bool StringEx::Contain(PcreReg &reg, bool casesensitive)
{
	reg.SetCaseSensitive(casesensitive);
	if(reg.IndexIn(*this, 0) == -1) return false;
	return true;
}

StringEx StringEx::ToUpper()
{
	transform(this->begin(), this->end(), this->begin(), (int(*)(int))toupper);
	return *this;
}

StringEx StringEx::ToLower()
{
	transform(this->begin(), this->end(), this->begin(), (int(*)(int))tolower);
	return *this;
}

ListEx<string> StringEx::Split(const string str)
{
	ListEx<string> result;
	string tmp;
	size_t begin, end;
	result.Clear();
	for(begin = 0;;){
		end = this->find(str, begin);
		if((size_t)-1 == end) break;
		tmp = this->substr(begin, end - begin);
		if("" != tmp) result.Append(tmp);
		begin = end + str.size();
	}
	//handle the last str
	//找不到对应字符，则返回空list
	if(begin)
	{
		tmp = this->substr(begin, this->size() - begin);
		if("" != tmp) result.Append(tmp);
	}

	return result;
}

string StringEx::Replace(string oldstr, string newstr)
{
	int index = 0;
	for(;;)	{
		index = this->find(oldstr, index);
		if(-1 == index) break;
		this->replace(index, oldstr.size(), newstr);
		index += newstr.size();
	}
	return *this;
}

string StringEx::Replace(PcreReg &reg, string newstr)
{
	int index = 0;
	for(;;)
	{
		index = reg.IndexIn(*this, index);
		if(-1 == index) break;
		this->replace(index, reg.Cap(0).size(), newstr);
		index += newstr.size();
	}
	return *this;
}

