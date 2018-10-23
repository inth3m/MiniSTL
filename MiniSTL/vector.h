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

	iterator start;						//Ŀǰʹ�ÿռ��ͷ
	iterator finish;						//Ŀǰʹ�ÿռ��β
	iterator end_of_storage;		//Ŀǰ���ÿռ��β

public:

	//*******************���ơ����졢������غ���***************************************************
	//���캯��	
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
	
	//************************������********************************************************************

	iterator begin() { return start; }
	const_iterator begin()const { return start; }
	const_iterator cbegin()const { return start; }
	reverse_iterator<value_type> rbegin() { return reverse_iterator<value_type>(start); }
	iterator end() { return finish; }
	const_iterator cend()const { return finish; }
	const_iterator end()const { return finish; }
	reverse_iterator<value_type> rend() { return reverse_iterator<value_type>(finish); }

	//**********************************�Ƚ�************************************************************
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

	//*****************************�������*************************************************************
	
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

	//*******************************����Ԫ�����******************************************************

	reference front() { return *(begin()); }
	reference back() { return *(end() - 1); }
	pointer data() { return start; }
	reference operator[](const difference_type n) { return *(begin() + n); }
	const_reference operator[](const difference_type i)const { return *(begin() + i); }

	//****************************************�޸�******************************************************
	
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
			copy(position + 1, finish, position);		//����Ԫ����ǰ�ƶ�
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
			//���ڿռ����Ҫ�����Ԫ�ظ���
			if (size_type(end_of_storage - finish) >= n) {
				T x_copy = x;
				const size_type elems_after = finish - postion;
				iterator old_finish = finish;

				//����ڵ�֮�������Ԫ�ظ�����������Ԫ�ظ���
				if (elems_after > n) {
					//�� [finsh - n, finsh) ���Ƶ� finsh ֮�� 
					uninitialized_copy(finish - n, finish, finish);
					finish += n;
					//��[postion, old_finish - n�����Ƶ�old_finish ֮ǰ [old_finsh -(postion- old_finish - n) , old_finsh)
					copy_backward(postion, old_finish - n, old_finish);
					fill(postion, postion + n, x_copy);			//�Ӳ���ڵ㿪ʼ������ֵ
				}
				//����ڵ�֮������Ԫ�ظ���С�ڵ���n
				else {
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n - elems_after;
					uninitialized_copy(postion, old_finish, finish);
					finish += elems_after;
					fill(postion, postion + n, x_copy);
				}
			}
			else {
				//���ÿռ�С��n
				//�Ⱦ����³��ȣ��ɳ��ȵ���������ɳ��� + ����Ԫ�ظ���
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);

				//�����µ�vector�ռ�
				iterator new_start = data_allocator::allocator(len);
				iterator new_finish = new_start;

				//���ƾ�vector �����֮ǰ��Ԫ��
				new_finish = uninitialized_copy(start, postion, new_start);
				//����Ԫ�ز���
				uninitialized_fill_n(new_finish, n, x);
				//��Ԫ�ظ���
				new_finish = uninitialized_copy(postion, finish, new_finish);

				//����ͷž�vector
				destory(start, finsh);
				deallocate();

				//����ˮλ���
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

	//*************************��������*********************************************
	
	iterator allocate_and_fill(size_type n, const T&x) {
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}
	template<class T>
	void insert_aux(iterator position, const T&x) {
		if (finish != end_of_storage) {	//���б��ÿռ�
										//�ڱ��ÿռ���ʼ����ʼ����һ��Ԫ�أ�����vector���һ��Ԫ����Ϊ���ֵ
			construct(finish, *(finish - 1));
			//����ˮλ
			++finish;

			T x_copy = x;
			copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
		}
		else {//�ޱ��ÿռ�
			const size_type old_size = size();
			//���ԭ��СΪ0�����ô�СΪ1
			//���ԭ��С��Ϊ0������Ϊԭ��������
			//ǰ�����������ԭ�������ݣ�������������������
			const size_type len = old_size != 0 ? 2 * old_size : 1;

			//ʵ�����ã�����ռ�
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;

			//�ѵ�дһ��try���
			try {
				//��ԭvector���ݿ�������vector
				new_finish = uninitialized_copy(start, position, new_start);
				//Ϊ��Ԫ�����ó�ֵx
				construct(new_finish, x);
				//����ˮλ
				++new_finish;
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (const std::exception&) {
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			//�����ͷ�ԭvector
			destroy(begin(), end());
			deallocate();

			//������������ָ����vector
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
