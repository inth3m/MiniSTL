#ifndef _RB_TREE_H
#define _RB_TREE_H
#include"iterator.h"
#include"allocator.h"
#include"construct.h"
using namespace MiniSTL;

namespace MiniSTL {
	typedef bool _rb_tree_color_type;
	const _rb_tree_color_type _rb_tree_red = false;		//红色为0
	const _rb_tree_color_type _rb_tree_black = true;	//黑色为1

	struct _rb_tree_node_base {
		typedef _rb_tree_color_type color_type;
		typedef _rb_tree_node_base* base_ptr;

		color_type color;		//节点颜色
		base_ptr parent;		//父节点
		base_ptr left;			//左节点
		base_ptr right;			//右节点

		static base_ptr minmum(base_ptr x) {
			while (x->left != 0) x = x->left;
			return x;
		}
		static base_ptr maxmum(base_ptr x) {
			while (x->right != 0) x = x->right;
			return x;
		}
	};

	template <class Value>
	struct _rb_tree_node :public _rb_tree_node_base
	{
		typedef _rb_tree_node<Value>* link_type;
		Value value_field;		//节点值
	};

	struct _rb_tree_base_iterator{

		typedef _rb_tree_node_base::base_ptr base_ptr;
		typedef bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t difference_type;

		base_ptr node;			//指向base_node的指针
		
		//找下一个比当前节点大的节点
		void increment() {
			//如果有右节点，就往右走，然后一直左子树走到底
			if (node->right != 0) {
				node = node->right;
				while (node->left != 0)
					node = node->left;
			}
			//没有右节点
			else {
				base_ptr y = node->parent;
				//如果本身是右节点，找到第一个不为右节点的祖先
				while (node == y->right) {
					node = y;
					y = y->parent;
				}
				//若此时右子节点不等于此时父节点，父节点为解答，否则node为解答
				if (node->right != y)
					node = y;
			}
		}

		//找到下一个比自己小的节点
		void decrement() {
			//下述情况发生于node为header时（node为end()时）
			//header的右子节点为mostright，指向整棵树的max节点
			//如果是红节点，且父节点的父亲等于自己，（情况1）右子节点 即为解答
			if (node->color == _rb_tree_red &&
				node->parent->parent == node)
				node = node->right;
			
			else if (node->left != 0) {			//如果有左子节点
				base_ptr y = node->left;		//往左走一步，一直往右
				while (y->right != 0)
					y = y->right;
				node = y;
			}
			//既不是根节点，也有左子节点，找到第一个不是左节点的父节点
			else {		
				base_ptr y = node->parent;
				while (node == y->left) {
					node = y;
					y = y->parent;
				}
				node = y;
			}
		}
	};

	template<class Value, class Ref, class Ptr>
	struct _rb_tree_iterator:public _rb_tree_base_iterator{
		typedef Value value_type;
		typedef Ref reference;
		typedef Ptr pointer;
		typedef _rb_tree_iterator<Value, Value&, Value*> iterator;
		typedef _rb_tree_iterator<Value, Value&, const Value*> const_iterator;
		typedef _rb_tree_node<Value>* link_type;
		typedef _rb_tree_iterator<Value, Ref, Ptr> self;

		_rb_tree_iterator(){}
		_rb_tree_iterator(link_type x) { node = x; }
		_rb_tree_iterator(const iterator &it) { node = it.node; }

		reference operator*()const { return link_type(node)->value_field; }
		pointer operator->()const { return &(operator*()); }

		self& operator++() { increment(); return *this; }
		self operator++(int) { 
			self temp = *this;
			increment(); 
			return temp;
		}
		self& operator--() { decrement(); return *this; }
		self operator--(int) { 
			self temp = *this;
			decrement(); 
			return temp;
		}

		
		friend	inline bool operator ==(const _rb_tree_base_iterator& __x, const _rb_tree_base_iterator& __y) {
			return __x.node == __y.node;
		}
		friend inline bool operator !=(const _rb_tree_base_iterator& __x, const _rb_tree_base_iterator& __y) {
			return __x.node != __y.node;
		}

	};

	template<class Key, class Value, class KeyOfValue, class Compare>
	class rb_tree {
	protected:
		typedef void* void_pointer;
		typedef _rb_tree_node_base* base_ptr;
		typedef _rb_tree_node<Value> rb_tree_node;
		typedef allocator<rb_tree_node> rb_tree_node_allocator;
		typedef _rb_tree_color_type color_type;

	public:
		typedef Key key_type;
		typedef Value value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef rb_tree_node* link_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
	
	protected:
		link_type get_node() { return rb_tree_node_allocator::allocate(); }
		void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }
		link_type creat_node(const value_type &x) {
			link_type tmp = get_node();
			construct(&tmp->value_field, x);
			return tmp;
		}

		link_type clone_node(link_type x) {
			link_type tmp = creat_node(x->value_field);
			tmp->color = x->color;
			tmp->left = 0;
			tmp->right = 0;
			return tmp;
		}

		void destory_node(link_type p) {
			destroy(p->value_field);
			put_node(p);
		}

	protected:

		size_type node_count;		//节点数量
		link_type header;
		Compare key_compare;	//键值大小比较准则

		link_type& root() const { return (link_type&)header->parent; }
		link_type& left_most() const { return (link_type&)header->left; }
		link_type& right_most() const { return (link_type&)header->right; }

		//获得节点x 成员
		static link_type& left(link_type x) { return (link_type&)(x->left); }
		static link_type& right(link_type x) { return (link_type&)(x->right); }
		static link_type& parent(link_type x) { return (link_type&)(x->parent); }
		static reference value(link_type x) { return x->value_field; }
		static const Key& key(link_type x){ return KeyOfValue()(value(x)); }
		static color_type& color(link_type x) { return (color_type&)(x->color); }

		static link_type& left(base_ptr x) { return (link_type&)(x->left); }
		static link_type& right(base_ptr x) { return (link_type&)(x->right); }
		static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
		static reference value(base_ptr x) { return x->value_field; }
		static const Key& key(base_ptr x) { return KeyOfValue()(value(x)); }
		static color_type& color(base_ptr x) { return (color_type&)(x->color); }

		//求取极大值和极小值
		static link_type minmum(link_type x) {
			return (link_type)_rb_tree_node_base::minmum();
		}
		static link_type maxmum(link_type x) {
			return (link_type)_rb_tree_node_base::maxmum();
		}

	public:
		typedef _rb_tree_iterator<value_type, reference, pointer> iterator;

	private:/*
		link_type _copy(link_type x, link_type p);
		void _erase(link_type x) {
		}*/
		void init() {
			header = get_node();					//产生一个节点空间
			color(header) = _rb_tree_red;		//header为红色，以此区分root

			root() = 0;
			left_most() = header;					//令header的左子节点为自己
			right_most() = header;				//令header的右子节点为自己
		}

	public:/*
		void clear() {
			if (node_count != 0) {
				erase(root());
				left_most() = header;
				root() = 0;
				right_most() = header;
				node_count = 0;
			}
		}*/
		rb_tree(const Compare& comp = Compare()) :node_count(0), key_compare(comp) {
			init();
		}
		~rb_tree() {
			//clear();
			//put_node(header);
		}

		rb_tree<Key, Value, KeyOfValue, Compare>&
			operator=(const rb_tree<Key, Value, KeyOfValue, Compare>& x);

	public:
		Compare key_com()const { return key_compare; }
		iterator begin() { return left_most(); }
		iterator end() { return header;}
		bool empty()const { return node_count == 0; }
		size_type size() const { return node_count; }
		size_type max_szie()const { return size_type(-1); }
/*
		void erase(iterator position);
		size_type erase(const key_type &x);
		void erase(iterator first, iterator last);
		void erase(const key_type * first, const key_type* last);*/


	public:

		iterator _insert(base_ptr x_, base_ptr y_, const Value& v) {
			//x为新插入节点，y为插入节点之父节点，参数v为新值
			link_type x = (link_type)x_;
			link_type y = (link_type)y_;

			link_type z;

			if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
				z = creat_node(v);		//产生一个新节点
				left(y) = z;
				if (y == header) {
					root() = z;
					right_most() = z;
				}
				else if (y == left_most())
					left_most() = z;
			}
			else {
				z = creat_node(v);
				right(y) = z;
				if (y == right_most())
					right_most() = z;
			}
			parent(z) = y;				//设定新节点的父节点
			left(z) = 0;					//设定新节点的左节点
			right(z) = 0;					//设定新节点的右节点

			_rb_tree_rebalance(z, header->parent);
			++node_count;
			return iterator(z);
		}

		//插入的节点必须独一无二
		pair<iterator, bool> insert_unique(const value_type &v) {
			link_type y = header;
			link_type x = root();
			bool comp = true;
			while (x != 0) {
				y = x;
				comp = key_compare(KeyOfValue()(v), key(x));
				x = comp ? left(x) : right(x);
			}
			//离开循环后，y所指的插入节点的父节点，此时比为叶节点

			iterator j = iterator(y);	//迭代器指向插入节点的父节点
			if (comp)						//comp为真，表示遇大，插入与左侧
				if (j == begin())		//如果插入节点之父节点为最左节点
					return pair<iterator, bool>(_insert(x, y, v), true);
				else
					--j;
			if (key_compare(key(link_type(j.node)), KeyOfValue()(v)))	//插入右侧
				return pair<iterator, bool>(_insert(x, y, v), true);

			//进行至此，表示新值一定与树中键值重复，不插入
			return pair<iterator, bool>(j, false);
		}

		//允许键值重复
		//返回值是一个RB-tree 迭代器，指向插入节点
		iterator insert_equal(const value_type &v) {
			link_type y = header;
			link_type x = root();		//从根节点开始
			while (x != 0) {
				y = x;
				x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
				//遇大往左，遇小往右
			}
			return _insert(x, y, v);
			//x为新值插入点，y为插入点的父节点，v为新值
		}

		
		inline void _rb_tree_rebalance(_rb_tree_node_base *x, _rb_tree_node_base*& root) {
			x->color = _rb_tree_red;																//新节点必须为红色
			while (x != root && x->parent->color == _rb_tree_red) {			//父节点为红
				if (x->parent == x->parent->parent->left) {							//父节点为祖父节点的左子节点
					_rb_tree_node_base *y = x->parent->parent->right;			//y为伯父节点
					if (y&&y->color == _rb_tree_red) {										//伯父节点存在，且为红色
						x->parent->color = _rb_tree_black;								//更改父节点为黑
						y->color = _rb_tree_black;												//更改伯父节点为黑
						x->parent->parent->color = _rb_tree_red;					//更改祖父节点为红
						x = x->parent->parent;
					}
					else {																					//无伯父节点，或伯父节点为黑色
						if (x == x->parent->right) {												//新节点为父节点的右子节点
							x = x->parent;
							_rb_tree_rotate_left(x, root);											//左旋
						}
					}
					x->parent->color = _rb_tree_black;									//父节点更改为红色
					x->parent->parent->color = _rb_tree_red;						//祖父更改为红色
					_rb_tree_rotate_right(x->parent->parent, root);					//右旋
				}
				else {																						//父节点为祖父节点的右节点
					_rb_tree_node_base *y = x->parent->parent->left;			//y为伯父节点
					if (y && y->color == _rb_tree_red) {									//伯父节点存在，且为红色
						x->parent->color = _rb_tree_black;								//更改父节点为黑色
						y->color = _rb_tree_black;												//更改伯父节点为黑色
						x->parent->parent->color = _rb_tree_red;					//祖父节点为红色
						x = x->parent->parent;													//准备继续往上层检查
					}
					else {																					//无伯父节点或伯父节点为黑色
						if (x == x->parent->left){														
							x = x->parent;													
							_rb_tree_rotate_right(x, root);										//右旋
						}
						x->parent->color = _rb_tree_black;
						x->parent->parent->color = _rb_tree_red;
						_rb_tree_rotate_left(x->parent->parent, root);				//左旋
					}
				}
			
			}//while结束
			root->color = _rb_tree_black;
		}
		
		inline void _rb_tree_rotate_left(_rb_tree_node_base *x, _rb_tree_node_base*& root) {
			//x为旋转节点
			_rb_tree_node_base *y = x->right;
			x->right = y->left;
			if (y->left != 0)
				y->left->parent = x;
			y->parent = x->parent;

			if (x == root) root = y;
			else if (x == x->parent->left)
				x->parent->left = y;
			else
				x->parent->right = y;
			y->left = x;
			x->parent = y;
		}

		inline void _rb_tree_rotate_right(_rb_tree_node_base *x, _rb_tree_node_base*& root) {
			//x为旋转节点
			_rb_tree_node_base *y = x->left;
			x->left = y->right;
			if (y->right != 0)
				y->right->parent = x;
			y->parent = x->parent;

			if (x == root) root = y;
			else if (x == x->parent->right)
				x->parent->right = y;
			else
				x->parent->left = y;
			y->right = x;
			x->parent = y;
		}
	};
}
#endif // !_RB_TREE_H



