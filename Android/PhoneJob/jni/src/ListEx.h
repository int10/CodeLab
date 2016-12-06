#ifndef LISTEX_H
#define LISTEX_H

#include <stdio.h>
#include <iostream>
#include <list>

#include "PcreReg.h"

using namespace std;

template<typename T> class ListEx : public list<T>
{
public:
	ListEx();
	~ListEx();
	void Append(T t);
	void Append(ListEx<T> lt);
	T &At(int index);
	bool Contain(T t);
	bool Contain(PcreReg &reg);
	int IndexOf(PcreReg &reg);
	void Clear();
	size_t Size();
	size_t Count(const T & t);
	bool IsEmpty();
	void Remove(T t);
	void RemoveAt(int index);
	ListEx<T> operator + (ListEx<T> lt);
	T& operator[](int index);
};

////////////////////////////////////////////////////////////////////////
///
/// functions
template<typename T> ListEx<T>::ListEx() : list<T>()
{
}

template<typename T> ListEx<T>::~ListEx()
{
}

template<typename T> void ListEx<T>::Append(T t)
{
	list<T>::push_back(t);
}

template<typename T> void ListEx<T>::Append(ListEx<T> lt)
{
	typename ListEx<T>::iterator it;

	for(it = lt.begin(); it != lt.end(); it++) {
		this->Append(*it);
	}
}

template<typename T> T & ListEx<T>::At(int index)
{
	typename ListEx<T>::iterator it;
	it = this->begin();
	if(index >= this->size()) index = this->size() - 1;
	while(index--){
		it++;
	}
	return *it;
}

template<typename T> bool ListEx<T>::Contain(T t)
{
	typename ListEx<T>::iterator it;
	it = this->begin();

	for(it = this->begin(); it != this->end(); it++) {
		if(t == *it) {
			return true;
		}
	}
	return false;
}

template<typename T> bool ListEx<T>::Contain(PcreReg &reg)
{
	typename ListEx<T>::iterator it;

	for(it = this->begin(); it != this->end(); it++) {
		if(reg.IndexIn(*it, 0) != -1) return true;
	}
	return false;
}

template<typename T> int ListEx<T>::IndexOf(PcreReg &reg)
{
	typename ListEx<T>::iterator it;
	size_t cnt = 0;

	for(it = this->begin(); it != this->end(); it++, cnt++) {
		if(reg.IndexIn(*it, 0) != -1) return cnt;
	}
	return -1;
}

template<typename T> void ListEx<T>::Clear()
{
	this->clear();
}

template<typename T> size_t ListEx<T>::Size()
{
	return this->size();
}

template<typename T> bool ListEx<T>::IsEmpty()
{
	return this->empty();
}

template<typename T> void ListEx<T>::Remove(T t)
{
	this->remove(t);
}

template<typename T> void ListEx<T>::RemoveAt(int index)
{
	if(index >= this->size()) return;

	typename ListEx<T>::iterator it;
	it = this->begin();
	while(index--) it++;
	this->erase(it);
}

template<typename T> size_t ListEx<T>::Count(const T & t)
{
	size_t cnt = 0;
	typename ListEx<T>::iterator it;

	for(it = this->begin(); it != this->end(); it++) {
		if(t == *it) {
			++cnt;
		}
	}
	return cnt;
}

template<typename T> ListEx<T> ListEx<T>::operator + (ListEx<T> lt)
{
	typename ListEx<T>::iterator it;
	ListEx<T> tmplist;

	for(it = this->begin(); it != this->end(); it++) {
		tmplist.push_back(*it);
	}

	for(it = lt.begin(); it != lt.end(); it++) {
		tmplist.push_back(*it);
	}

	return tmplist;
}

template<typename T> T& ListEx<T>::operator[](int index)
{
	typename ListEx<T>::iterator it;
	it = this->begin();
	if(index >= this->size()) index = this->size() - 1;
	while(index--){
		it++;
	}
	return *it;
}


#endif // LISTEX_H
