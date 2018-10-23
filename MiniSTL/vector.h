#ifndef _VECTOR_H
#define _VECTOR_H

#include"alloc.h"
#include"allocator.h"
#include"iterator.h"
#include"construct.h"
#include<cstddef>
using namespace MiniSTL;

template<class T, class Alloc = MiniSTL::allocator<T>>
class vector {
public:
	typedef T							value_type;
	typedef value_type*		pointer;
	typedef value_type*		iterator;
	typedef value_type&		reference;
	typedef size_t					size_type;
	typedef ptrdiff_t				difference_type;
	typedef const iterator		const_iterator;
	typedef const reference const_reference;
	typedef Alloc					data_allocator;
protected:

	iterator start;						//目前使用空间的头
	iterator finish;						//目前使用空间的尾
	iterator end_of_storage;		//目前可用空间的尾

public:

	//*******************复制、构造、析构相关函数***************************************************
	//构造函数	
	vector() :start(0), finish(0), end_of_storage(0) {}
	explicit vector(const size_type n) {
		fill_initialize(n, T());
	}
	vector(const size_type n, const value_type& value) {
		fill_initialize(n, value);
	}
	vector(int  n, const value_type& value) {
		fill_initialize(n, value);
	}
	vector(long n, const value_type& value) {
		fill_initialize(n, value);
	}
	vector(const vector &v) {
		size_type n = v.size();
		start = allocate_and_copy(n, v.start, v.finish);
		finish = start + v.size();
		end_of_storage = finish;
	}
	vector(vector &&v) {
			start = v.start;
			finish = v.finish;
			end_of_storage = v.end_of_storage;
			v.start = v.finish = v.end_of_storage = 0;
	}
	vector(iterator first, iterator last) {
		size_type n = last - first;
		start = allocate_and_copy(n, first, last);
		finish = start + n;
		end_of_storage = finish;
	}
	vector& operator =(const vector &v) {
		this(v);
	}
	vector& operator =(vector &&v) {
			destroy_and_deallocate_all();
			start = v.start;
			finish = v.finish;
			end_of_storage = v.end_of_storage;
			v.start = v.finish = v.end_of_storage = 0;
			return v;
	}
	~vector() {
		destroy(start, finish);
		deallocate();
	}
	
	//************************迭代器********************************************************************

	iterator begin() { return start; }
	const_iterator begin()const { return start; }
	const_iterator cbegin()const { return start; }
	reverse_iterator<value_type> rbegin() { return reverse_iterator<value_type>(start); }
	iterator end() { return finish; }
	const_iterator cend()const { return finish; }
	const_iterator end()const { return finish; }
	reverse_iterator<value_type> rend() { return reverse_iterator<value_type>(finish); }

	//**********************************比较************************************************************
	bool operator ==(const vector &v)const {
		if (size() != v.size()) return false;
		else {
			auto ptr1 = start;
			auto ptr2 = v.start;
			for (; ptr1 != finish && ptr2 != v.finish; ++ptr1, ++ptr2) {
				if (*ptr1 != *ptr2)
					return false;
			}
			return true;
		}
	}
	bool operator !=(const vector &v)const
	{
		return !(*this == v);
	}

	//*****************************容量相关*************************************************************
	
	size_type size()const { return size_type(cend() - cbegin()); }
	size_type capactity() const {
		return size_type(end_of_storage - cbegin());
	}
	bool empty() const { return begin() == end(); }
	void reserve(size_type n) {
		if (n <= capactity()) return;
		iterator newStart = data_allocator::allocate(n);
		iterator newFinish = uninitialized_copy(begin(), end(), newStart);
		destroy_and_deallocate_all();
		start = newStart;
		finish = newFinish;
		end_of_storage = start + n;
	}
	void resize(size_type new_size, const T&x) {
		if (new_size < size())
			erase(begin() + new_size, end());
		else
			insert(end(), new_size - size(), x);
	}
	void shrink_to_fit() {
		data_allocator::deallocate(finish, end_of_storage - finish);
		end_of_storage = finish;
	}

	//*******************************访问元素相关******************************************************

	reference front() { return *(begin()); }
	reference back() { return *(end() - 1); }
	pointer data() { return start; }
	reference operator[](const difference_type n) { return *(begin() + n); }
	const_reference operator[](const difference_type i)const { return *(begin() + i); }

	//****************************************修改******************************************************
	
	void push_back(const T& x) {
		if (finish != end_of_storage) {
			construct(finish, x);
			++finish;
		}
		else {
			insert_aux(end(), x);
		}
	}
	void pop_back() {
		if (size()) {
			--finish;
			destroy(finish);
		}
	}
	iterator erase(iterator position) {
		if (position + 1 != end())
			copy(position + 1, finish, position);		//后续元素往前移动
		--finish;
		destroy(finish);
		return position;
	}
	iterator erase(iterator first, iterator last) {
		difference_type len_of_tail = end() - last;
		difference_type len_of_remove = last - first;
		finish = finish - len_of_remove;
		for (; len_of_tail != 0; --len_of_tail) {
			auto temp = (last - len_of_remove);
			*temp = *(last++);
		}
		return first;
	}
	void new_size(size_type new_size) { resize(new_size, T()); }
	void clear() { erase(begin(), end()); }
	template<class T, class Alloc>
	void instert(iterator postion, size_type n, const T &x) {
		if (n != 0) {
			//设于空间大于要插入的元素个数
			if (size_type(end_of_storage - finish) >= n) {
				T x_copy = x;
				const size_type elems_after = finish - postion;
				iterator old_finish = finish;

				//插入节点之后的现有元素个数大于新增元素个数
				if (elems_after > n) {
					//将 [finsh - n, finsh) 复制到 finsh 之后 
					uninitialized_copy(finish - n, finish, finish);
					finish += n;
					//将[postion, old_finish - n）复制到old_finish 之前 [old_finsh -(postion- old_finish - n) , old_finsh)
					copy_backward(postion, old_finish - n, old_finish);
					fill(postion, postion + n, x_copy);			//从插入节点开始填入新值
				}
				//插入节点之后现有元素个数小于等于n
				else {
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n - elems_after;
					uninitialized_copy(postion, old_finish, finish);
					finish += elems_after;
					fill(postion, postion + n, x_copy);
				}
			}
			else {
				//备用空间小于n
				//先决定新长度，旧长度的两倍，或旧长度 + 新增元素个数
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);

				//配置新的vector空间
				iterator new_start = data_allocator::allocator(len);
				iterator new_finish = new_start;

				//复制旧vector 插入点之前的元素
				new_finish = uninitialized_copy(start, postion, new_start);
				//将新元素插入
				uninitialized_fill_n(new_finish, n, x);
				//旧元素复制
				new_finish = uninitialized_copy(postion, finish, new_finish);

				//清除释放旧vector
				destory(start, finsh);
				deallocate();

				//调整水位标记
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}
	}
	void swap(vector &v) {
		iterator temp = start;
		start = v.start;
		v.start = temp;

		temp = finish;
		finish = v.finish;
		v.finish = temp;

		tmep = end_of_storage;
		end_of_storage = v.end_of_storage;
		v.end_of_storage = temp;
	}
	
	
protected:

	//*************************辅助操作*********************************************
	
	iterator allocate_and_fill(size_type n, const T&x) {
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}
	template<class T>
	void insert_aux(iterator position, const T&x) {
		if (finish != end_of_storage) {	//还有备用空间
										//在备用空间起始处开始构造一个元素，并以vector最后一个元素作为其初值
			construct(finish, *(finish - 1));
			//调整水位
			++finish;

			T x_copy = x;
			copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
		}
		else {//无备用空间
			const size_type old_size = size();
			//如果原大小为0，配置大小为1
			//如果原大小不为0，配置为原来的两倍
			//前半段用来放置原来的数据，后半段用来放置新数据
			const size_type len = old_size != 0 ? 2 * old_size : 1;

			//实际配置，申请空间
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;

			//难得写一个try语句
			try {
				//将原vector内容拷贝到新vector
				new_finish = uninitialized_copy(start, position, new_start);
				//为新元素设置初值x
				construct(new_finish, x);
				//调整水位
				++new_finish;
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (const std::exception&) {
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			//析构释放原vector
			destroy(begin(), end());
			deallocate();

			//调整迭代器，指向新vector
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
	void deallocate() {
		if (start) {
			data_allocator::deallocate(start, (end_of_storage - start));
		}
	}

	void fill_initialize(size_type n, const T&value) {
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

	iterator allocate_and_copy(size_type n,iterator first, iterator last) {

		iterator result = data_allocator::allocate(n);
		uninitialized_copy(first, last, result);
		return result;
	}
	void destroy_and_deallocate_all() {
		if (capactity() != 0) {
			data_allocator::destroy(start, finish);
			data_allocator::deallocate(start, capactity());
		}
	}
};

#endif // !_VECTOR_H
