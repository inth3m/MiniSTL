#ifndef _DEQUE_H
#define _DEQUE_H
#include"allocator.h"
#include"construct.h"
#include"iterator.h"
#include"type_traits.h"

using namespace MiniSTL;

template<class T, class Ref, class Ptr, size_t BufSiz>
struct _deque_iterator
{
	typedef _deque_iterator<T, T&, T*, BufSiz> iterator;
	typedef _deque_iterator<T, const T&, T*, BufSiz> const_iterator;
	static size_t buffer_size() { return _deque_buf_size(BufSiz, sizeof(T)); }

	typedef random_access_iterator_tag iterator_catergoy;
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t diffrence_type;
	typedef T** map_pointer;

	typedef _deque_iterator self;

	//与容器保持的链接
	T *cur;		//此迭代器指的缓冲区现行元素
	T *first;		//此迭代器所指缓冲区的头
	T *last;		//此迭代器指的缓冲器的尾（包含备用空间）
	map_pointer node;	//指向管控中心

						//决定缓冲区大小的函数
						//如果m不为0 传回n， 表示buffer size由用户自定义
						//不为0，使用默认值
						//元素大小小于512，传回512/sz
						//不小于512传回n
	inline size_t _deque_buf_size(size_t n, size_t sz) {
		return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
	}

	//跳一个缓冲区
	void set_node(map_pointer new_node) {
		node = new_node;
		first = *new_node;
		last = first + diffrence_type(buffer_size());
	}

	//重载运算子
	reference operator*() const { return *cur; }
	pointer operator->() const { return &operator*(); }
	diffrence_type operator-(const self& x) const {
		return diffrence_type(buffer_size())*(node - x.node - 1) +
			(cur - first) + (x.last - x.cur);
	}

	self& operator++() {
		++cur;
		if (cur == last) {
			set_node(node + 1);
			cur = first;
		}
		return *this;
	}
	self operator++(int) {
		self tmp = *this;
		++*this;
		return tmp;
	}
	self& operator--() {
		if (cur == first) {
			set_node(node - 1);
			cur = last;
		}
		--cur;
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		--*this;
		return tmp;
	}
	self& operator +=(diffrence_type n) {
		diffrence_type offset = n + (cur - first);
		//目标位置在同一缓冲区内
		if (offset >= 0 && offset < diffrence_type(buffer_size()))
			cur += n;
		else {
			diffrence_type node_offset =
				offset > 0 ? offset / diffrence_type((buffer_size())) :
				-diffrence_type((-offset - 1) / buffer_size()) - 1;
			//切换至正确缓冲区
			set_node(node + node_offset);
			//切换至正确元素
			cur = first + (offset - node_offset * diffrence_type(buffer_size()));
		}
		return *this;
	}
	self operator+(diffrence_type n) const {
		self tmp = *this;
		return tmp += n;
	}
	self operator-=(diffrence_type n) {
		return *this += -n;
	}
	self operator-(diffrence_type n) const {
		self tmp = *this;
		return tmp -= n;
	}
	reference operator[](diffrence_type n) const { return *(*this + n); }
	bool operator ==(const self& x) const { return cur == x.cur; }
	bool operator !=(const self& x) const { return !(*this == x); }
	bool operator<(const self& x)const {
		return (node == x.node) ? (cur < x.cur) : (node < x.node);
	}

};

template<class T, class Alloc = MiniSTL::allocator<T>, size_t Bufsiz = 0>
class deque {
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef T& reference;
	typedef size_t size_type;
	typedef ptrdiff_t diffrence_type;

	typedef _deque_iterator<T, T&, T*, Bufsiz> iterator;
protected:
	//元素指针的指针
	typedef pointer* map_pointer;

protected:
	iterator start;		//表现第一个节点
	iterator finish;		//表现后一个节点
	map_pointer map;
	size_type map_size;

public:
	iterator begin() { return start; }
	iterator end() { return finish; }
	reference operator[](size_type n) {
		return start[diffrence_type(n)];
	}
	reference front() { return *start; }
	reference back() {
		iterator tmp = finish;
		--tmp;				//调用operator--
		return *tmp;		//调用operator*
	}
	size_type size()const { return finish - start; }
	size_type max_size()const { return size_type(-1); }
	bool empty()const { return finish == start; }

	//空间配置
protected:
	//专属空间配置器，每次配置一个元素大小
	typedef MiniSTL::allocator<T> data_allocator;
	//专属空间配置器，每次配置一个指针大小
	typedef MiniSTL::allocator<pointer> map_allocator;

	deque(int n, const value_type &value) : start(), finish(), map(0), map_size(0) {
		fill_initialize(n, value);
	}

	template<class T, class Alloc, size_t BufSize>
	void fill_initialize(size_type n, const value_type &value) {
		create_map_and_node(n);
		map_pointer cur;

		for (cur = start.node; cur < finish.node; ++cur)
			uninitialized_fill(*cur, *cur + buffer_size(), value);
		uninitialized_fill(finish.first, finish.cr, value);
	}

	//负责产生并安排好deque的结构
	void create_map_and_node(size_type num_elements) {

		//需要节点数 = （元素个数/每个缓冲区可容纳的元素个数 + 1）
		//刚好整除会多分配一个节点
		size_type num_nodes = num_elements / buffer_size() + 1;

		//一个map管理几个节点，最少8个，最多是所需节点数加2
		//前后各预留一个扩充时可用
		map_size = max(initial_map_size(), num_nodes + 2);
		map = map_allocator::allocate(map_size);

		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		map_pointer cur;

		for (cur = nstart; cur <= nfinish; ++cur)
			*cur = alloca_node();

		start.set_node(nstart);
		finish.set_node(nfinish);
		start.cur = start.first;
		finish.cur = finish.first + num_elements % buffer_size();
	}
};
#endif // !_DEQUE_H
