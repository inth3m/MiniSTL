#include"vector.h"
#include"heap.h"
#include<algorithm>

using namespace MiniSTL;

template<class T, class Sequence = MiniSTL::vector<T>>
class priortyu_queue{
public:
	typedef typename Sequence::value_type value_type;
	typedef typename Sequence::size_type size_type;
	typedef typename Sequence::reference reference;
	typedef typename Sequence::const_reference const_reference;

protected:
	Sequence c;
	//Compare comp;

public:
	priortyu_queue():c(){}
	template<class InputIterator>
	priortyu_queue(InputIterator first, InputIterator last) :
		c(first, last) {
		MiniSTL::make_heap(c.begin(), c.end());
		int i = 0;
		while (i < c.size()) {
			cout << c[i] << " ";
			i++;
		}
		cout << endl;
	}

	bool empty()const { return c.empty(); }
	size_type size()const { return c.size(); }
	const_reference top() { return c.front(); }
	void push(const value_type &x) {
		c.push_back(x);
		MiniSTL::push_heap(c.begin(), c.end());
	}

	void pop() {
		MiniSTL::pop_heap(c.begin(), c.end());
		int i = 0;
		c.pop_back();
	}
};

