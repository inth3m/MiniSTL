#ifndef _STACK_H
#define _STACK_H
#include"allocator.h"
#include"construct.h"
#include"type_traits.h"
#include"deque.h"

//stackÊÇÒÔdeque×÷Îªµ×²ãÈÝÆ÷

using namespace MiniSTL;

namespace MiniSTL{
template<class T, class Sequence = MiniSTL::deque<T> >
class stack {
	/*friend bool operator== (const stack &, const stack&);
	friend bool operator< (const stack &, const stack&);
	*///typedef MiniSTL::deque<T> Sequence;
	typedef typename Sequence::value_type value_type;
	typedef typename Sequence::size_type size_type;
	typedef typename Sequence::reference reference;
	typedef typename Sequence::const_reference const_reference;

protected:
	//µ×²ãÈÝÆ÷
	Sequence c;
public:
	bool empty()const { return c.empty(); }
	size_type size()const { return c.size(); }
	reference top() { return c.back(); }
	const_reference top()const { return c.back(); }
	void push(const value_type &x) { c.push_back(x); }
	void pop() { c.pop_back(); }
	//template<class T, class Sequence>
	//bool operator== (const stack<T, Sequence>&x, const stack<T, Sequence>&y) {
	//	return x.c == y.c;
	//}
	//bool operator== (const stack<T, Sequence>&x, const stack<T, Sequence>&y) {
	//	return x.c < y.c;
	//}
};
}
#endif // !_STACK_H
