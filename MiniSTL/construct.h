/*定义了基本构造函数 construct 和 析构函数 destroy*/
#ifndef _STL_CONSTRUCT_H
#define _STL_CONSTRUCT_H

#include<new.h>
#include"type_traits.h"

namespace MiniSTL {
	template <class ForwardIterator, class T>
	inline void _destroy_aux(ForwardIterator first, ForwardIterator last, _true_type) {}

	inline void destroy(char *, char *) {}
	inline void destroy(wchar_t *, wchar_t *) {}

	template<class T1, class T2>
	inline void construct(T1 *ptr1, const T2& value) {
		new(ptr1) T1(value);
	}

	template<class T>
	inline void destroy(T *ptr) {
		ptr->~T();
	}

	template<class ForwardIterator>
	inline void _destroy(ForwardIterator first, ForwardIterator last, _true_type) {}

	template<class ForwardIterator>
	inline void _destroy(ForwardIterator first, ForwardIterator last, _false_type) {
		for (; first != last; ++first) {
			destroy(&*first);
		}
	}

	template<class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last) {
		typedef typename _type_traits<ForwardIterator>::is_POD_type is_POD_type;
		_destroy(first, last, is_POD_type());
	}
};
#endif // !_STL_CONSTRUCT_H

