#ifndef linkqueue_h
#define linkqueue_h
#include "xcept.h"
template<class>
class LinkQueue;

template <class T>
class Node{
	friend LinkQueue<T>;
private:
	T data;
	Node<T> *link;
};
template <class T>
class LinkQueue{
public:
	LinkQueue(){front=rear=0;}
	~LinkQueue();
	bool IsEmpty()const{return front?false:true;}
	bool IsFull()const;
	T First()const;
	T Last()const;
	LinkQueue<T>& Add(const T& x);
	LinkQueue<T>& Delete(T& x);
private:
	Node<T> *front;
	Node<T> *rear;
};

template<class T>
LinkQueue<T>::~LinkQueue(){
	Node<T> *next;
	while(front){
		next=front->link;
		delete front;
		front=next;
	}
}

template<class T>
bool LinkQueue<T>::IsFull()const{
	Node<T> *p;
	try{
		p=new Node<T>;
		delete p;
		return false;
	}
	catch(NoMem){return true;}
}

template<class T>
T LinkQueue<T>::First()const{
	if(IsEmpty())
		throw OutOfBounds();
	return first->data;
}

template<class T>
T LinkQueue<T>::Last()const{
	if(IsEmpty())
		throw OutOfBounds();
	return rear->data;
}

template<class T>
LinkQueue<T>& LinkQueue<T>::Add(const T &x){
	Node<T> *p=new Node<T>;
	p->data=x;
	p->link=0;
	if(front)
		rear->link=p;
	else front=p;
	rear=p;
	return *this;
}

template<class T>
LinkQueue<T>& LinkQueue<T>::Delete(T &x){
	if(IsEmpty())
		throw OutOfBounds();
	x=front->data;
	Node<T> *p=front;
	front=front->link;
	delete p;
	return *this;
}

#endif