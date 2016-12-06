#ifndef MAPEX_H
#define MAPEX_H

#include <stdio.h>
#include <iostream>
#include <map>
#include "ListEx.h"

using namespace std;

template<typename TK, typename TV> class MapEx : public map<TK, TV>
{
public:
	MapEx();
	~MapEx();
	TV At(TK tk);				//get value
	void Append(TK tk, TV tv);	//add value
	TK Key(TV tv);				//get a key
	ListEx<TK> Keys();			//get all keys
	void Remove(TK tk);			//remove one record
	size_t Size();				//get size
	bool IsEmpty();				//whether empty
	bool Exist(TK tk);			//whether exist this key
	void Clear();				//remove all record
};

/////////////////////////////////////////////////////
///
/// functions
template<typename TK, typename TV> MapEx<TK, TV>::MapEx() : map<TK, TV>()
{
}

template<typename TK, typename TV> MapEx<TK, TV>::~MapEx()
{
}

template<typename TK, typename TV> TV MapEx<TK, TV>::At(TK tk)
{
	return (*this)[tk];
}

template<typename TK, typename TV> void MapEx<TK, TV>::Append(TK tk, TV tv)
{
	(*this)[tk] = tv;
}

template<typename TK, typename TV> ListEx<TK> MapEx<TK, TV>::Keys()
{
	ListEx<TK> result;

	typename MapEx<TK, TV>::iterator it;
	it = this->begin();

	for(it = this->begin(); it != this->end(); it++) {
		result.push_back(it->first);
	}
	return result;
}

template<typename TK, typename TV> void MapEx<TK, TV>::Remove(TK tk)
{
	this->erase(tk);
}

template<typename TK, typename TV> size_t MapEx<TK, TV>::Size()
{
	return this->size();
}

template<typename TK, typename TV> bool MapEx<TK, TV>::IsEmpty()
{
	return this->empty();
}

template<typename TK, typename TV> bool MapEx<TK, TV>::Exist(TK tk)
{
	typename MapEx<TK, TV>::iterator it;
	it = this->begin();

	for(it = this->begin(); it != this->end(); it++) {
		if(tk == it->first) return true;
	}
	return false;
}

template<typename TK, typename TV> TK MapEx<TK, TV>::Key(TV tv)
{
	typename MapEx<TK, TV>::iterator it;
	it = this->begin();

	for(it = this->begin(); it != this->end(); it++) {
		if(tv == it->second) return it->first;
	}

	return TK();
}

template<typename TK, typename TV> void MapEx<TK, TV>::Clear()
{
	this->clear();
}

#endif // MAPEX_H
