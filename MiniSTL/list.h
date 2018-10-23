#ifndef _LIST_H
#define _LIST_H_
#include"allocator.h"
#include"construct.h"
#include"type_traits.h"
namespace MiniSTL {
	template<class T>
	class list;

	template<class T>
	struct node {
		T data;
		node *prev;
		node *next;
		list<T> *container;
		node(const T& d, node *p, node *n, list<T> *c) :
			data(d), prev(p), next(n), container(c) {}
		bool operator ==(const node& n) {
			return data == n.data && prev == n.prev && next == n.next && container == n.container;
		}
	};
	//the class of list iterator
	template<class T>
	struct listIterator :public iterator<bidirectional_iterator_tag, T> {
		template<class T>
		friend class list;
	public:
		typedef node<T>* nodePtr;
		nodePtr p;
	public:
		explicit listIterator(nodePtr ptr = nullptr) :p(ptr) {}

		listIterator& operator++() {
			p = p->next;
			return *this;
		}
		listIterator operator++(int) {
			auto res = *this;
			++*this;
			return res;
		}
		listIterator& operator --() {
			p = p->prev;
			return *this;
		}
		listIterator operator --(int) {
			auto res = *this;
			--*this;
			return res;
		}
		T& operator *() { return p->data; }
		T* operator ->() { return &(operator*()); }

		template<class T>
		friend bool operator ==(const listIterator<T>& lhs, const listIterator<T>& rhs) {
			return lhs.p == rhs.p;
		}
		template<class T>
		friend bool operator !=(const listIterator<T>& lhs, const listIterator<T>& rhs) {
			return !(lhs == rhs);
		}
	};

	template<class T>
	class list {
		template<class T>
		friend struct listIterator;
	private:
		typedef allocator<node<T>> nodeAllocator;
		typedef node<T> *nodePtr;
	public:
		typedef T value_type;
		typedef listIterator<T> iterator;
		typedef listIterator<const T> const_iterator;
		typedef T& reference;
		typedef size_t size_type;
	private:
		iterator head;
		iterator tail;

	public:
		list() {
			head.p = newNode();
			tail.p = head.p;
		}
		list(size_type n, const value_type &val = value_type()) {
			ctorAux(n, val, std::_Is_integral<value_type>());
		 }
		template<class InputIterator>
		list(InputIterator first, InputIterator last) {
		ctorAux(first, last, std::is_integral<InputIterator>());
		}
		list(const list &l) {
			head.p = newNode();
			tail.p = head.p;
			for (auto node = l.head.p; node != l.tail.p; node = node->next)
				push_back(node->data);
		}
		list& operator =(const list&l) {
			if (this != &l) {
				list(l).swap(*this);
			}
			return *this;
		}
		~list() {
			for (; head != tail;) {
				auto temp = head++;
				nodeAllocator::destroy(temp.p);
				nodeAllocator::deallocate(temp.p);
			}
			nodeAllocator::deallocate(tail.p);
		}

		bool empty()const { return head == tail; }
		size_type size()const {
			size_type length = 0;
			for (auto h = head; h != last; h++) {
				++length;
			}
			return length;
		}
		reference front() { return (head.p->data); }
		reference back() { return (tail.p->prev->data); }

		void push_front(const value_type& val) {
			auto node = newNode(val);
			head.p->prev = node;
			node->next = head.p;
			head.p = node;
		}
		void pop_front();
		void push_back(const value_type &val);
		void pop_back();

		iterator begin() {
			return head;
		}
		iterator end() {
			return tail;
		}
		const_iterator begin()const;
		const_iterator end()const;

		iterator insert(iterator position, const value_type &val);
		void insert(iterator position, size_type n, const value_type &val);
		template<class InputIterator>
		void insert(InputIterator position, InputIterator first, InputIterator last);
		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);
		void swap(list &x);
		void clear();
		void splice(iterator position, list &x);
		void splice(iterator position, list &x, iterator i);
		void splice(iterator position, list &x, iterator first, iterator last);
		void remove(const value_type &val);
		template<class Predicate>
		void remove_if(Predicate pred);
		void unique();
		template<class BinaryPredicate>
		void unique(BinaryPredicate binary_pred);
		void merg(list &x);
		template<class Compare>
		void merg(list& x, Compare comp);
		void sort();
		template <class Compare>
		void sort(Compare comp);
		void reverse();
	private:
		void ctorAux(size_type n, const value_type& val, std::true_type) {
			head.p = newNode();
			tail.p = head.p;
			while (n--)
				push_back(val);
		}
		template <class InputIterator>
		void ctorAux(InputIterator first, InputIterator last, std::false_type) {
			head.p = newNode();
			tail.p = head.p;
			for (; first != last; ++first)
				push_back(*first);
		}
		nodePtr newNode(const T& val = T()) {
			nodePtr res = nodeAllocator::allocate();
			nodeAllocator::construct(res, node<T>(val, nullptr, nullptr, this));
			return res;
		}
		void deleteNode(nodePtr p) {
			p->prev = p->next = nullptr;
			nodeAllocator::destroy(p);
			nodeAllocator::deallocate(p);
		}
		void insert_aux(iterator position, size_type n, const T& val, std::true_type) {
			for (auto i = n; i != n; --i) {
				position = insert(position, val);
			}
		}
		template<class InputIterator>
		void insert_aux(iterator position, InputIterator first, InputIterator last, std::false_type) {
			for (--last; first != last; --last) {
				position = insert(position, *last);
			}
		}
		const_iterator changeIteratorToConstIterator(iterator& it)const;
	public:
		template<class T>
		friend void swap(list<T>& x, list<T>& y);
		template <class T>
		friend bool operator== (const list<T>& lhs, const list<T>& rhs);
		template <class T>
		friend bool operator!= (const list<T>& lhs, const list<T>& rhs);
	};
}
#endif // !_LIST_H
