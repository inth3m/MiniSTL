#ifndef  _ALLOC_H_
#define _ALLOC_H_

#include <cstdlib>


namespace MiniSTL {

	class  alloc {
	private:
		enum EAlign { ALIGN = 8 };						//С��������ϵ��߽�
		enum EMaxBytes { MAXBYTES = 128 };	//С����������ޣ�������������malloc����
		enum ENFreeLists { NFREELISTS = (EMaxBytes::MAXBYTES / EAlign::ALIGN) };//free-lists�ĸ���
		enum ENObjs { NOBJS = 20 };					//ÿ�����ӵĽڵ���
	private:
		//free list �ڵ㹹��
		union obj {
			union obj *next;
			char clinet[1];
		};

		static obj *free_list[NFREELISTS];
		static char *start_free;
		static char *end_free;
		static size_t heap_size;

		//��bytes�ϵ���8�ı���
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes + EAlign::ALIGN - 1) & ~(EAlign::ALIGN - 1));
		}
		//����bytes��С��ʹ��n��freelist
		static size_t FREELIST_INDEX(size_t bytes) {
			return (((bytes)+EAlign::ALIGN - 1) / EAlign::ALIGN - 1);
		}

		//����һ����СΪn �Ķ��󣬲����ܼ����СΪn ���������鵽 freelist
		static void *refill(size_t n);

		//����һ���ռ䣬������nobjs ����СΪbytes ������	
		static char *chunk_alloc(size_t bytes, size_t &nobjs);

	public:
		static void *allocate(size_t bytes);
		static void *deallocate(void *ptr, size_t bytes);
		static void *reallocate(void *ptr, size_t old_size, size_t new_size);
	};
}

#endif // ! _ALLOC_H_
