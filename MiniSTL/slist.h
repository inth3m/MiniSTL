#pragma once
#include"iterator.h"
#include"allocator.h"
#include"construct.h"

using namespace MiniSTL;
namespace MiniSTL {


	//slist 节点
	struct _slist_node_base
	{
		_slist_node_base *next;

	};

	template<class T>
	struct _slist_node : public _slist_node_base
	{
		T data;
	};

	inline _slist_node_base * slist_make_link(
		_slist_node_base *prev_node,
		_slist_node_base *new_node) 
	{
		new_node->next = prev_node->next;
		prev_node->next = new_node;
		return new_node;
	}

	inline size_t _slist_size(_slist_node_base *node) {
		size_t reslut = 0;
		for (; node != 0; node = node->next)
			++reslut;
		return reslut;
	}

	//slist迭代器

	struct _slist_iterator_base{
		typedef size_t size_type;
		typedef ptrdiff_t diffrence_type;
		typedef forward_iterator_tag iterator_category;	 //单向

		//指向节点基本结构
		_slist_node_base *node;

		_slist_iterator_base(_slist_node_base *x):node(x){}
		void incr() { node = node->next; }

		bool operator== (const _slist_iterator_base &x) const {
			return node == x.node;
		}

		bool operator != (const _slist_iterator_base &x) const {
			return node != x.node;
		}
	};

	template<class T, class Ref, class Ptr>
	struct _slist_iterator: public _slist_iterator_base
	{
		typedef _slist_iterator<T, T&, T*> iterator;
		typedef _slist_iterator<T, T&, const T*> const_iterator;
		typedef _slist_iterator<T, Ref, Ptr> self;

		typedef T value_type;
		typedef Ptr pointer;
		typedef Ref reference;
		typedef _slist_node<T> list_node;

		_slist_iterator(list_node *x): _slist_iterator_base(x){}
		_slist_iterator():_slist_iterator_base(0){}
		_slist_iterator(const iterator &x):_slist_iterator_base(x.node){}

		reference operator*()const { return ((list_node*)node)->data; }
		pointer operator->()const { return &(operator()); }

		self& operator++() {
			incr();
			return *this;
		}
		self& operator++(int) {
			self tmp = *this;
			incr();
			return tmp;
		}
	};

	//slit
	template<class T, class Alloc = MiniSTL::allocator<_slist_node<T>>>
	class slist {
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_poniter;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		typedef _slist_iterator<T, T&, T*> iterator;
		typedef _slist_iterator<T, T&, const T*> const_iterator;

	private:
		typedef _slist_node<T> list_node;
		typedef _slist_node_base list_node_base;
		typedef _slist_iterator_base ierator_base;
		typedef Alloc list_node_allocator;

		static list_node* creat_node(const value_type &x) {
			list_node *node = list_node_allocator::allocate();
			construct(&node->data, x);
			node->next = 0;
			return node;
		}

		static void destory_node(list_node *node) {
			destroy(&node->data);
			list_node_allocator::deallocate(node);
		}

	private:
		list_node_base head;	//头部

	public:
		slist() { head.next = 0; }
		~slist() { clear(); }

	public:
		iterator begin() { return iterator((list_node*)head.next); }
		iterator end() { return iterator(0); }
		size_type size()const { return _slist_size(head.next); }
		bool empty()const { return head.next == 0; }

		//交换list就是交换头部
		void swap(slist &L) {
			list_node_base* tmp = head.next;
			head.next = L.head.next;
			L.head.next = tmp;
		}

	public:
		reference front() { return ((list_node*)head.next)->data; }

		void push_front(const value_type &x) {
			slist_make_link(&head, creat_node(x));
		}

		void pop_front() {
			list_node *node = (list_node*)head.next;
			head.next = node->next;
			destory_node(node);
		}
		void clear() {
			while (size())
				pop_front();
		}
	};

}