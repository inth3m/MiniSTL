#include "alloc.h"
namespace MiniSTL {

	char * alloc::start_free = 0;	//��ʼ���ڴ�ص���ʼλ��
	char *alloc::end_free = 0;
	size_t alloc::heap_size = 0;

	//128/8
	alloc::obj *alloc::free_list[alloc::ENFreeLists::NFREELISTS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	void *alloc::allocate(size_t bytes) {
		if (bytes > EMaxBytes::MAXBYTES) return malloc(bytes);	//����128����mallo����

		//ѡ��16��freelist ���ʵ���һ��
		size_t index = FREELIST_INDEX(bytes);
		obj *list = free_list[index];
		if (list) {
			//ʹ��ǰfreelist �ڵ� ָ����һ���飨��֪���ǲ���0��
			free_list[index] = list->next;
			return list;
		}
		//��ǰreelist��0��û�п��������ˣ�����
		else return refill(ROUND_UP(bytes));
	}

	void * alloc::deallocate(void * ptr, size_t bytes)
	{
		if (bytes > EMaxBytes::MAXBYTES) free(ptr);
		else {

			//���ݴ�С�ҵ���ǰ��С��freelist�ڵ�
			size_t index = FREELIST_INDEX(bytes);

			//ptr �� node ��ָ��Ҫ�ͷŵ��ڴ�����
			obj *node = (obj*)(ptr);
			node->next = free_list[index];
			//������ռ����
			free_list[index] = node;
		}
		return nullptr;
	}

	void * alloc::reallocate(void * ptr, size_t old_size, size_t new_size)
	{
		deallocate(ptr, old_size);
		ptr = allocate(new_size);

		return ptr;
	}


	//������freelist û�п�������ʱ������fefill Ϊfreelist ���ռ�
	void * MiniSTL::alloc::refill(size_t bytes)
	{
		size_t nobjs = ENObjs::NOBJS;
		//���ڴ��ȡ�� nobjs�� pass by reference ֵ��ı�
		char *chunk = chunk_alloc(bytes, nobjs);
		obj **my_free_list = 0;
		obj *result = 0;
		obj *cur_obj = 0, *next_obj = 0;

		//���ֻ���һ�����飬����������������� freelist���½ڵ�
		if (nobjs == 1) return chunk;

		//�������freelist �����½ڵ�
		else {
			//myfreelist ��ָ�� bytes��С��freelist�ڵ��ָ��
			my_free_list = free_list + FREELIST_INDEX(bytes);
			result = (obj *)(chunk);

			//�ӵ�1����ʼ ��0�����ظ��ͻ���
			*my_free_list = next_obj = (obj *)(chunk + bytes);
			for (int i = 1;; ++i) {
				cur_obj = next_obj;
				next_obj = (obj *)((char *)next_obj + bytes);
				if (nobjs - 1 == i) {
					cur_obj->next = 0;
					break;
				}
				else {
					cur_obj->next = next_obj;
				}
			}
			return result;
		}

	}
	char * alloc::chunk_alloc(size_t bytes, size_t & nobjs)
	{
		char *result = 0;
		size_t total_bytes = bytes * nobjs;			//��Ҫ���ܿռ�
		size_t bytes_left = end_free - start_free;	//�ڴ��ʣ��ռ�

		if (bytes_left >= total_bytes) {					//�ڴ��ʣ��ռ���ȫ������Ҫ
			result = start_free;
			start_free = start_free + total_bytes;
			return result;
		}
		else if (bytes_left >= bytes) {					//�ڴ��ʣ��ռ䲻����ȫ������Ҫ�����㹻��Ӧһ�������ϵ�����
			nobjs = bytes_left / bytes;					//������Թ�Ӧ������
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		//�ڴ��ʣ��ռ���һ������Ĵ�С���޷��ṩ
		else {
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			//�������ڴ���е���ͷ���м�ֵ
			if (bytes_left > 0) {
				obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);
				//����freelist�����ڴ���еĲ���ռ����
				((obj *)start_free)->next = *my_free_list;
				*my_free_list = (obj *)start_free;
			}

			//����heap�ռ䲹���ڴ��
			start_free = (char *)malloc(bytes_to_get);

			//malloc ʧ�� heap�ռ䲻��
			if (!start_free) {
				obj **my_free_list = 0, *p = 0;

				//���ӻ���û��δ������
				for (int i = 0; i <= EMaxBytes::MAXBYTES; i += EAlign::ALIGN) {
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != 0) {
						*my_free_list = p->next;
						start_free = (char *)p;
						end_free = start_free + i;
						return chunk_alloc(bytes, nobjs);
					}
				}
				end_free = 0;
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(bytes, nobjs);
		}
	}
}