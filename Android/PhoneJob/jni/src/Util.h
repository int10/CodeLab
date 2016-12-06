#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
//#include <sys/types.h>
//#include <regex.h>
//#include <memory.h>
//#include <stdlib.h>
#include <string>
#include <iostream>
#include "des/des.h"
#include "des/base64.h"

using namespace std;

struct ForeachContainerBase {};

template <typename T>
class ForeachContainer : public ForeachContainerBase {
public:
	inline ForeachContainer(const T& t): c(t), brk(0), i(c.begin()), e(c.end()){};
	const T c;
	mutable int brk;
	mutable typename T::const_iterator i, e;
	inline bool condition() const { return (!brk++ && i != e); }
};

template <typename T> inline T *mForeachPointer(const T &) { return 0; }

template <typename T> inline ForeachContainer<T> mForeachContainerNew(const T& t)
{ return ForeachContainer<T>(t); }

template <typename T>
inline const ForeachContainer<T> *mForeachContainer(const ForeachContainerBase *base, const T *)
{ return static_cast<const ForeachContainer<T> *>(base); }

#define foreach(variable, container) \
	for (const ForeachContainerBase &_container_ = mForeachContainerNew(container); \
		 mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->condition();       \
		 ++mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->i)               \
		for (variable = *mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->i; \
			 mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->brk;           \
			 --mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->brk)

class Util
{
public:
	static string DES3Decode(const string command,const string key,const string iv);
	static string DES3Encode(const string code,const string key,const string iv);

};

#endif // UTIL_H
