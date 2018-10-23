#ifndef  _ALLOC_H_
#define _ALLOC_H_

#include <cstdlib>


namespace MiniSTL {

	class  alloc {
	private:
		enum EAlign { ALIGN = 8 };						//小型区块的上调边界
		enum EMaxBytes { MAXBYTES = 128 };	//小型区块的上限，超过的区块由malloc分配
		enum ENFreeLists { NFREELISTS = (EMaxBytes::MAXBYTES / EAlign::ALIGN) };//free-lists的个数
		enum ENObjs { NOBJS = 20 };					//每次增加的节点数
	private:
		//free list 节点构造
		union obj {
			union obj *next;
			char clinet[1];
		};

		static obj *free_list[NFREELISTS];
		static char *start_free;
		static char *end_free;
		static size_t heap_size;

		//将bytes上调至8的倍数
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes + EAlign::ALIGN - 1) & ~(EAlign::ALIGN - 1));
		}
		//根据bytes大小，使用n号freelist
		static size_t FREELIST_INDEX(size_t bytes) {
			return (((bytes)+EAlign::ALIGN - 1) / EAlign::ALIGN - 1);
		}

		//返回一个大小为n 的对象，并可能加入大小为n 的其他区块到 freelist
		static void *refill(size_t n);

		//配置一大块空间，可容纳nobjs 个大小为bytes 的区块	
		static char *chunk_alloc(size_t bytes, size_t &nobjs);

	public:
		static void *allocate(size_t bytes);
		static void *deallocate(void *ptr, size_t bytes);
		static void *reallocate(void *ptr, size_t old_size, size_t new_size);
	};
}

#endif // ! _ALLOC_H_
