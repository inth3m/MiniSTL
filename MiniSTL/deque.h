#ifndef _DEQUE_H
#define _DEQUE_H
#include"allocator.h"
#include"construct.h"
#include"type_traits.h"

namespace MiniSTL {
	template<class T, class Alloc = allocator<T>>
	class deque;
	template<class T>
	class deque_iterator :public iterator<bidirectional_iterator_tag, T> {
	private:
		template<class T, class Alloc>
		friend class deque;
		typedef T* pointer;
		typedef T& reference;
	private:
		typedef const MiniSTL::deque<T>* cntrPtr;
		size_t map_index;
		T *cur;
		cntrPtr container;
	public:
		deque_iterator() :map_index(-1), cur(0), container(0) {}
		deque_iterator(size_t index, T *ptr, cntrPtr container)
			:map_index(index), cur(ptr), container(container) {}
		deque_iterator(const deque_iterator& it)
			:map_index(it.map_index), cur(it.cur), container(it.container) {}
		deque_iterator& operator = (const deque_iterator& it) {
			if (this != &it) {
				map_index = it.map_index;
				cur = it.cur;
				container = it.container;
			}
			return *this;
		}
		void swap(deque_iterator& it) {
			swap(map_index, it.map_index);
			swap(cur, it.cur);
		}
		reference operator *() { return *cur; }
		const reference operator *()const { return *cur; }
		pointer operator ->() { return &(operator*()); }
		const pointer operator ->()const { return &(operator*()); }
		deque_iterator& operator ++() {
			if (cur != getBuckTail(map_index))//+1后还在同一个桶里
				++cur;
			else if (map_index + 1 < container->map_size) {//+1后还在同一个map里
				++map_index;
				cur = getBuckHead(map_index);
			}
			else {//+1后跳出了map
				map_index = container->map_size;
				//cur_ = container_->map_[mapIndex_] + getBuckSize();//指向map_[mapSize_-1]的尾的下一个位置
				cur = container->map_[map_index];
			}
			return *this;
		}
		deque_iterator operator ++(int) {
			auto res = *this;
			++(*this);
			return res;

		}
		deque_iterator& operator --() {
			if (cur != getBuckHead(map_index))//当前不指向桶头
				--cur;
			else if (map_index - 1 >= 0) {//-1后还在map里面
				--map_index;
				cur = getBuckTail(map_index);
			}
			else {
				map_index = 0;
				cur = container->map_[map_index];//指向map_[0]的头
			}
			return *this;

		}
		deque_iterator operator --(int) {
			auto res = *this;
			--(*this);
			return res;
		}
		bool operator ==(const deque_iterator& it)const {
			return ((map_index == it.map_index) &&
				(cur == it.cur) && (container == it.container));

		}
		bool operator !=(const deque_iterator& it)const {
			return (*this == it);
		}
	private:
		T * getBuckTail(size_t mapIndex)const {
			return container->map_[mapIndex] + (container->getBuckSize() - 1);
		}
		T *getBuckHead(size_t mapIndex)const {
			return container->map_[mapIndex];
		}
		size_t getBuckSize()const {
			return container->getBuckSize();
		}
	public:
		template<class T>
		friend deque_iterator<T> operator + (const deque_iterator<T>& it, typename deque_iterator<T>::difference_type n) {
			deque_iterator<T> res(it);
			auto m = res.getBuckTail(res.map_index) - res.cur;
			if (n <= m) {//前进n步仍在同一个桶中
				res.cur += n;
			}
			else {
				n = n - m;
				res.map_index += (n / it.getBuckSize() + 1);
				auto p = res.getBuckHead(res.map_index);
				auto x = n % it.getBuckSize() - 1;
				res.cur = p + x;
			}
			return res;
		}
		template<class T>
		friend deque_iterator<T> operator + (typename deque_iterator<T>::difference_type n, const deque_iterator<T>& it) {
			return (it + n);
		}
		template<class T>
		friend deque_iterator<T> operator - (const deque_iterator<T>& it, typename deque_iterator<T>::difference_type n) {
			deque_iterator<T> res(it);
			auto m = res.cur - res.getBuckHead(res.map_index);
			if (n <= m)//后退n步还在同一个桶中
				res.cur -= n;
			else {
				n = n - m;
				res.map_index -= (n / res.getBuckSize() + 1);
				res.cur = res.getBuckTail(res.map_index) - (n % res.getBuckSize() - 1);
			}
			return res;
		}
		template<class T>
		friend deque_iterator<T> operator - (typename deque_iterator<T>::difference_type n, const deque_iterator<T>& it) {
			return (it - n);
		}
		template<class T>
		friend typename deque_iterator<T>::difference_type operator - (const deque_iterator<T>& it1, const deque_iterator<T>& it2) {
			if (it1.container == it2.container && it1.container == 0)
				return 0;
			return typename deque_iterator<T>::difference_type(it1.getBuckSize()) * (it1.map_index - it2.map_index - 1)
				+ (it1.cur - it1.getBuckHead(it1.map_index)) + (it2.getBuckTail(it2.map_index) - it2.cur) + 1;

		}
		template<class T>
		friend void swap(deque_iterator<T>& lhs, deque_iterator<T>& rhs) {
			lhs.swap(rhs);
		}
	};

	//class of deque
	template<class T, class Alloc>
	class deque {
	private:
		template<class T>
		friend class deque_iterator;
	public:
		typedef T value_type;
		typedef deque_iterator<T> iterator;
		typedef deque_iterator<const T> const_iterator;
		typedef T& reference;
		typedef const reference const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef Alloc allocator_type;
	private:
		typedef Alloc dataAllocator;
		enum class EBucksSize { BUCKSIZE = 64 };
	private:
		iterator start, finish;
		size_t map_size;
		T **map_;
	public:
		deque() :map_size(0), map_(0) {}
		explicit deque(size_type n, const value_type& val = value_type()) {
			deque();
			deque_aux(n, val, typename std::is_integral<size_type>::type());
		}
		template <class InputIterator>
		deque(InputIterator first, InputIterator last) {
			deque();
			deque_aux(first, last, typename std::is_integral<InputIterator>::type());
		}
		deque(const deque& x) {
			map_size = x.map_size;
			map_ = getANewMap(map_size);
			for (int i = 0; i + x.start.map_index != x.map_size; ++i)
				for (int j = 0; j != getBuckSize(); ++j)
					map_[x.start.map_index + i][j] = x.map_[x.start.map_index + i][j];
			start.map_index = x.start.map_index;
			finish.map_index = x.finish.map_index;
			start.cur = map_[start.map_index] + (x.start.cur - x.map_[x.start.map_index]);
			finish.cur = map_[finish.map_index] + (x.finish.cur - x.map_[x.finish.map_index]);
			start.container = finish.container = this;

		}

		~deque() {
			for (int i = 0; i != map_size; ++i) {
				for (auto p = map_[i] + 0; !p && p != map_[i] + getBuckSize(); ++p)
					dataAllocator::destroy(p);
				if (!map_[i])
					dataAllocator::deallocate(map_[i], getBuckSize());
			}
			delete[] map_;

		}

		deque& operator= (const deque& x) {
			this(x);
		}
		deque& operator= (deque&& x) {
			this(x);
		}

		iterator begin() { return start; }
		iterator end() { return finish; }
		iterator begin()const { return start; }
		iterator end()const { return finish; }
	public:
		size_type size() const { return end() - begin(); }
		bool empty() const { return begin() == end(); }

		reference operator[] (size_type n) {
			return *(begin() + n);
		}
		reference front() {
			return *begin();
		}
		reference back() {
			return *(end() - 1);
		}
		const_reference operator[] (size_type n) const {
			return *(begin() + n);
		}
		const_reference front() const {
			return *begin();
		}
		const_reference back() const {
			return *(end() - 1);
		}

		void push_back(const value_type& val) {
			if (empty()) {
				init();
			}
			else if (back_full()) {
				reallocate_and_copy();
			}
			MiniSTL::construct(finish.cur, val);
			++finish;

		}
		void push_front(const value_type& val) {
			if (empty()) {
				init();
			}
			else if (front_full()) {
				reallocate_and_copy();
			}
			--start;
			MiniSTL::construct(start.cur, val);

		}
		void pop_back() {
			--finish;
			dataAllocator::destroy(finish.cur);
		}
		void pop_front() {
			dataAllocator::destroy(start.cur);
			++start;
		}
		void swap(deque& x) {
			MiniSTL::swap(map_size, x.map_size);
			MiniSTL::swap(map_, x.map_);
			start.swap(x.start);
			finish.swap(x.finish);

		}
		void clear() {
			for (auto i = 0; i != map_size; ++i) {
				for (auto p = map_[i] + 0; !p && p != map_[i] + getBuckSize(); ++p)
					dataAllocator::destroy(p);
			}
			map_size = 0;
			start.map_index = finish.map_index = map_size / 2;
			start.cur = finish.cur = map_[map_size / 2];

		}
	private:
		T * getANewBuck() {
			return dataAllocator::allocate(getBuckSize());
		}
		T** getANewMap(const size_t size) {
			T **map = new T*[size];
			for (int i = 0; i != size; ++i)
				map[i] = getANewBuck();
			return map;

		}
		size_t getNewMapSize(const size_t size) {
			return (size == 0 ? 2 : size * 2);
		}
		size_t getBuckSize()const {
			return (size_t)EBucksSize::BUCKSIZE;
		}
		void init() {
			map_size = 2;
			map_ = getANewMap(map_size);
			start.container = finish.container = this;
			start.map_index = finish.map_index = map_size - 1;
			start.cur = finish.cur = map_[map_size - 1];
		}
		bool back_full()const {
			return map_[map_size - 1] && (map_[map_size]) == end().cur;
		}
		bool front_full()const {
			return map_[0] && map_[0] == begin().cur;
		}
		void deque_aux(size_t n, const value_type& val, std::true_type) {
			int i = 0;
			for (; i != n / 2; ++i)
				(*this).push_front(val);
			for (; i != n; ++i)
				(*this).push_back(val);
		}
		template<class Iterator>
		void deque_aux(Iterator first, Iterator last, std::false_type) {
			difference_type mid = (last - first) / 2;
			for (auto it = first + mid; it != first - 1; --it)
				(*this).push_front(*it);
			for (auto it = first + mid + 1; it != last; ++it)
				(*this).push_back(*it);
		}
		void reallocate_and_copy() {
			auto newMapSize = getNewMapSize(map_size);
			T** newMap = getANewMap(newMapSize);
			size_t startIndex = newMapSize / 4;
			for (int i = 0; i + start.map_index != map_size; ++i)
				for (int j = 0; j != getBuckSize(); ++j)
					newMap[startIndex + i][j] = map_[start.map_index + i][j];

			size_t n = start.cur - map_[start.map_index];
			auto size = this->size();
			auto b = start, e = finish;
			clear();
			map_size = newMapSize;
			map_ = newMap;
			start = iterator(startIndex, newMap[startIndex] + n, this);
			finish = start + size;


		}
	public:
		template <class T, class Alloc>
		friend bool operator== (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
			auto cit1 = lhs.begin(), cit2 = rhs.begin();
			for (; cit1 != lhs.end() && cit2 != rhs.end(); ++cit1, ++cit2) {
				if (*cit1 != *cit2)
					return false;
			}
			if (cit1 == lhs.end() && cit2 == rhs.end())
				return true;
			return false;
		}
		template <class T, class Alloc>
		friend bool operator!= (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
			return !(lhs == rhs);
		}
		template <class T, class Alloc>
		friend void swap(deque<T, Alloc>& x, deque<T, Alloc>& y) {
			x.swap(y);
		}
	};//end of deque
}
#endif // !_DEQUE_H
