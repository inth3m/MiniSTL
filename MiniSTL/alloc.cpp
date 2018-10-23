#include "alloc.h"
namespace MiniSTL {

	char * alloc::start_free = 0;	//初始化内存池的起始位置
	char *alloc::end_free = 0;
	size_t alloc::heap_size = 0;

	//128/8
	alloc::obj *alloc::free_list[alloc::ENFreeLists::NFREELISTS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	void *alloc::allocate(size_t bytes) {
		if (bytes > EMaxBytes::MAXBYTES) return malloc(bytes);	//超过128的用mallo申请

		//选择16个freelist 中适当的一个
		size_t index = FREELIST_INDEX(bytes);
		obj *list = free_list[index];
		if (list) {
			//使当前freelist 节点 指向下一区块（不知道是不是0）
			free_list[index] = list->next;
			return list;
		}
		//当前reelist是0（没有可用区块了）申请
		else return refill(ROUND_UP(bytes));
	}

	void * alloc::deallocate(void * ptr, size_t bytes)
	{
		if (bytes > EMaxBytes::MAXBYTES) free(ptr);
		else {

			//根据大小找到当前大小的freelist节点
			size_t index = FREELIST_INDEX(bytes);

			//ptr 和 node 都指向要释放的内存区块
			obj *node = (obj*)(ptr);
			node->next = free_list[index];
			//标记这块空间可用
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


	//当发现freelist 没有可用区块时，调用fefill 为freelist 填充空间
	void * MiniSTL::alloc::refill(size_t bytes)
	{
		size_t nobjs = ENObjs::NOBJS;
		//从内存池取， nobjs是 pass by reference 值会改变
		char *chunk = chunk_alloc(bytes, nobjs);
		obj **my_free_list = 0;
		obj *result = 0;
		obj *cur_obj = 0, *next_obj = 0;

		//如果只获得一个区块，这个区块分配给调用者 freelist无新节点
		if (nobjs == 1) return chunk;

		//否则调整freelist 纳入新节点
		else {
			//myfreelist 是指向 bytes大小的freelist节点的指针
			my_free_list = free_list + FREELIST_INDEX(bytes);
			result = (obj *)(chunk);

			//从第1个开始 第0个返回给客户端
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
		size_t total_bytes = bytes * nobjs;			//需要的总空间
		size_t bytes_left = end_free - start_free;	//内存池剩余空间

		if (bytes_left >= total_bytes) {					//内存池剩余空间完全满足需要
			result = start_free;
			start_free = start_free + total_bytes;
			return result;
		}
		else if (bytes_left >= bytes) {					//内存池剩余空间不能完全满足需要，但足够供应一个或以上的区块
			nobjs = bytes_left / bytes;					//计算可以供应的区块
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		//内存池剩余空间连一个区块的大小都无法提供
		else {
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			//试着让内存池中的零头还有价值
			if (bytes_left > 0) {
				obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);
				//调整freelist，将内存池中的参与空间编入
				((obj *)start_free)->next = *my_free_list;
				*my_free_list = (obj *)start_free;
			}

			//配置heap空间补充内存池
			start_free = (char *)malloc(bytes_to_get);

			//malloc 失败 heap空间不足
			if (!start_free) {
				obj **my_free_list = 0, *p = 0;

				//检视还有没有未用区块
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