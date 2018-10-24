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
		void pop_front() {
			auto oldNode = head.p;
			head.p = oldNode->next;
			head.p->prev = nullptr;
			deleteNode(oldNode);
		}
		void push_back(const value_type &val) {
			auto node = newNode();
			(tail.p)->data = val;
			(tail.p)->next = node;
			node->prev = tail.p;
			tail.p = node;
		}
		void pop_back() {
			auto newTail = tail.p->prev;
			newTail->next = nullptr;
			deleteNode(tail.p);
			tail.p = newTail;
		}

		iterator begin() {
			return head;
		}
		iterator end() {
			return tail;
		}
		const_iterator begin()const;
		const_iterator end()const;

		iterator insert(iterator position, const value_type &val) {
			if (position == begin()) {
				push_front(val);
				return begin();
			}
			if (position == end()) {
				push_back(val);
				return end();
			}
			auto node = newNode(val);
			auto prev = position.p->prev;
			node->next = position.p;
			node->prev = prev;
			prev->next = node;
			position.p->prev = node;
			return iterator(node);
		}
		template<class InputIterator>
		void insert(iterator position, size_type n, const value_type &val) {
			insert_aux(position, n, val, typename std::is_integral<InputIterator>::type());
		}
		template<class InputIterator>
		void insert(InputIterator position, InputIterator first, InputIterator last) {
			insert_aux(position, first, last, typename std::is_integral<InputIterator>::type());
		}
		iterator erase(iterator position)  {
			if (position == head()) {
				pop_front();
				return head;
			}
			else {
				auto prev = position.p->prev;
				prev->next = position.p->next;
				position.p->next->prev = prev;
				deleteNode(position.p);
				return iterator(prev->next);
			}
		}
		iterator erase(iterator first, iterator last) {
			iterator res;
			for (; first != last; first++)
				res = erase(first);
			return res;
		}
		void swap(list &x);
		void clear() {
			erase(begin(), end());
		}
		void splice(iterator position, list &x) {
			this->insert(position, x.begin(), x.end());
			x.head.p = x.tail.p;
		}
		void splice(iterator position, list &x, iterator i) {
			auto next = i;
			this->splice(position, x, i, ++next);
		}
		void splice(iterator position, list &x, iterator first, iterator last) {
			if (first.p == last.p) return;
			auto tailNode = last.p->prev;
			if (x.head.p = first.p) {
				x.head.p = last.p;
				x.head.p = last.p;
				x.head.p->prev = nullptr;
			}
			else {
				first.p->prev->next = last.p;
				last.p->prev = first.p->prev;
			}
			if (position.p == head.p) {
				first.p->prev = nullptr;
				tailNode->next = head.p;
				head.p->prev = tailNode;
				head.p = first.p;
			}
			else {
				position.p->prev->next = first.p;
				first.p->prev = position.p->prev;
				tailNode->next = position.p;
				position.p->prev = tailNode;
			}
		}
		void remove(const value_type &val) {
			for (auto it = begin(); it != end();) {
				if (it == val)
					it = erase(it);
				else
					++it;
			}
		}
		template<class Predicate>
		void remove_if(Predicate pred) {
			for (auto it = begin(); it != end();) {
				if (pred(*it))
					it = erase(it);
				else
					it++;
			}
		}
		void unique() {
			nodePtr curNode = head.p;
			while (curNode != tail.p) {
				nodePtr nexrNode = curNode->next;
				if (curNode->data == nexrNode->data) {
					if (nexrNode == tail.p) {
						curNode->next = nullptr;
						tail.p = curNode;
					}
					else {
						curNode->next = nexrNode->next;
						nexrNode->next->prev = curNode;
					}
					deleteNode(nexrNode);
				}
				else {
					curNode = nexrNode;
				}
			}
		}
		template<class BinaryPredicate>
		void unique(BinaryPredicate binary_pred) {
			nodePtr curNode = head.p;
			while (curNode != tail.p) {
				nodePtr nextNode = curNode->next;
				if (binary_pred(curNode->data, nextNode->data)) {
					if (nextNode == tail.p) {
						curNode->next = nullptr;
						tail.p = curNode;
					}
					else {
						curNode->next = nextNode->next;
						nextNode->next->prev = curNode;
					}
					deleteNode(nextNode);
				}
				else {
					curNode = nextNode;
				}
			}
		}
		void merg(list &x) {
			auto it1 = begin(), it2 = x.begin();
			while (it1 != end() && it2 != x.end()) {
				if (*it1 <= *it2)
					++it1;
				else {
					auto temp = it2++;
					this->splice(it1, x, temp);
				}
			}
			if (it1 == end()) {
				this->splice(it1, x, it2, x.end());
			}
		}
		template<class Compare>
		void merg(list& x, Compare comp) {
			auto it1 = begin(), it2 = x.begin();
			while (it1 != end() && it2 != x.end()) {
				if (comp(*it2, *it1)) {
					auto temp = it2++;
					this->splice(it1, x, temp);
				}
				else
					++it1;
			}
			if (it1 == end()) {
				this->splice(it1, x, it2, x.end());
			}

		}
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
