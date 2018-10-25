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

	//���������ֵ�����
	T *cur;		//�˵�����ָ�Ļ���������Ԫ��
	T *first;		//�˵�������ָ��������ͷ
	T *last;		//�˵�����ָ�Ļ�������β���������ÿռ䣩
	map_pointer node;	//ָ��ܿ�����

						//������������С�ĺ���
						//���m��Ϊ0 ����n�� ��ʾbuffer size���û��Զ���
						//��Ϊ0��ʹ��Ĭ��ֵ
						//Ԫ�ش�СС��512������512/sz
						//��С��512����n
	inline size_t _deque_buf_size(size_t n, size_t sz) {
		return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
	}

	//��һ��������
	void set_node(map_pointer new_node) {
		node = new_node;
		first = *new_node;
		last = first + diffrence_type(buffer_size());
	}

	//����������
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
		//Ŀ��λ����ͬһ��������
		if (offset >= 0 && offset < diffrence_type(buffer_size()))
			cur += n;
		else {
			diffrence_type node_offset =
				offset > 0 ? offset / diffrence_type((buffer_size())) :
				-diffrence_type((-offset - 1) / buffer_size()) - 1;
			//�л�����ȷ������
			set_node(node + node_offset);
			//�л�����ȷԪ��
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
	//Ԫ��ָ���ָ��
	typedef pointer* map_pointer;

protected:
	iterator start;		//���ֵ�һ���ڵ�
	iterator finish;		//���ֺ�һ���ڵ�
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
		--tmp;				//����operator--
		return *tmp;		//����operator*
	}
	size_type size()const { return finish - start; }
	size_type max_size()const { return size_type(-1); }
	bool empty()const { return finish == start; }

	//�ռ�����
protected:
	//ר���ռ���������ÿ������һ��Ԫ�ش�С
	typedef MiniSTL::allocator<T> data_allocator;
	//ר���ռ���������ÿ������һ��ָ���С
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

	//������������ź�deque�Ľṹ
	void create_map_and_node(size_type num_elements) {

		//��Ҫ�ڵ��� = ��Ԫ�ظ���/ÿ�������������ɵ�Ԫ�ظ��� + 1��
		//�պ�����������һ���ڵ�
		size_type num_nodes = num_elements / buffer_size() + 1;

		//һ��map�������ڵ㣬����8�������������ڵ�����2
		//ǰ���Ԥ��һ������ʱ����
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
