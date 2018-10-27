#ifndef _HEAP_H
#define _HEAP_H
#include"iterator.h"
using namespace MiniSTL;

//max_heap
namespace MiniSTL {
	//�˺���������ʱ����Ԫ����λ�ڵײ���������β��
	template<class RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
		_push_heap_aux(first, last, distance_type(first), value_type(first));
	}

	//��ֵλ����β�� (last - first)- 1
	template<class RandomAccessIterator, class Distance, class T>
	inline void _push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance *, T*) {
		push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
	}

	template<class RandomAccessIterator, class Distance, class T>
	void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
		//�ҳ����ڵ�
		Distance parent = (holeIndex - 1) / 2;
		//δ���ﶥ�ˣ��Ҹ��ڵ�ֵС����ֵ
		while (holeIndex > topIndex && *(first + parent) < value) {
			//�ֵΪ��ֵ
			*(first + holeIndex) = *(first + parent);
			//��������Ϊ���ڵ�
			holeIndex = parent;
			//�µĸ��ڵ�
			parent = (holeIndex - 1) / 2;
		}
		//���������ˣ�������heap ������Ϊֹ
		*(first + holeIndex) = value;
	}

	template<class RandomAccessIterator>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
		_pop_heap_aux(first, last, value_type(first));
	}




	template<class RandomAccessIterator, class Distance, class T>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance *) {

		*result = *first;
		_adjust_heap(first, Distance(0), Distance(last - first), value);
	}
	template<class RandomAccessIterator, class T>
	inline void _pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
		pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
	}

	template<class RandomAccessIterator, class T, class Distance>
	void _adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
		Distance topIndex = holeIndex;

		//���ڵ���ҽڵ�
		Distance secondChild = 2 * holeIndex + 2;
		while (secondChild < len) {
			//�Ƚ����������ӽڵ㣬secondChiled����ϴ���ӽڵ�
			if (*(first + secondChild) < *(first + secondChild - 1))
				secondChild--;
			//��ϴ��ӽڵ�Ϊ��ֵ
			*(first + holeIndex) = *(first + secondChild);
			holeIndex = secondChild;
			secondChild = 2 * (secondChild + 1);
		}
		//û�����ӽڵ㣬ֻ�����ӽڵ�
		if (secondChild == len) {
			//������ֵΪ��ֵ
			*(first + holeIndex) = *(first + (secondChild - 1));
			holeIndex = secondChild - 1;
		}

		_push_heap(first, holeIndex, topIndex, value);
	}

	template<class RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
		while (last - first > 1) {
			pop_heap(first, last--);
		}
	}

	template<class RandomAccessIterator>
	inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
		_make_heap(first, last, value_type(first), distance_type(first));
	}

	template<class RandomAccessIterator, class T, class Distance>
	void _make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*) {
		if (last - first < 2) return;
		Distance len = last - first;
		Distance parent = (len - 2) / 2;

		while (true) {
			_adjust_heap(first, parent, len, T(*(first + parent)));
			if (parent == 0) return;
			parent--;
		}
	}
}
#endif // !_HEAP_H
