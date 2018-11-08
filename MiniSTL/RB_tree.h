#ifndef _rb_TREE_H
#define _rb_TREE_H
#include"iterator.h"
#include"allocator.h"
#include"construct.h"
using namespace MiniSTL;

namespace MiniSTL {
	typedef bool _rb_tree_color_type;
	const _rb_tree_color_type _rb_tree_red = false;		//��ɫΪ0
	const _rb_tree_color_type _rb_tree_black = true;	//��ɫΪ1

	struct _rb_tree_node_base {
		typedef _rb_tree_color_type color_type;
		typedef _rb_tree_node_base* base_ptr;

		color_type color;		//�ڵ���ɫ
		base_ptr parent;		//���ڵ�
		base_ptr left;			//��ڵ�
		base_ptr right;			//�ҽڵ�

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
		Value value_field;		//�ڵ�ֵ
	};

	struct _rb_tree_base_iterator{

		typedef _rb_tree_node_base::base_ptr base_ptr;
		typedef bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t difference_type;

		base_ptr node;			//ָ��base_node��ָ��		
		//����һ���ȵ�ǰ�ڵ��Ľڵ�
		void increment() {
			//������ҽڵ㣬�������ߣ�Ȼ��һֱ�������ߵ���
			if (node->right != 0) {
				node = node->right;
				while (node->left != 0)
					node = node->left;
			}
			//û���ҽڵ�
			else {
				base_ptr y = node->parent;
				//����������ҽڵ㣬�ҵ���һ����Ϊ�ҽڵ������
				while (node == y->right) {
					node = y;
					y = y->parent;
				}
				//����ʱ���ӽڵ㲻���ڴ�ʱ���ڵ㣬���ڵ�Ϊ��𣬷���nodeΪ���
				if (node->right != y)
					node = y;
			}
		}

		//�ҵ���һ�����Լ�С�Ľڵ�
		void decrement() {
			//�������������nodeΪheaderʱ��nodeΪend()ʱ��
			//header�����ӽڵ�Ϊmostright��ָ����������max�ڵ�
			//����Ǻ�ڵ㣬�Ҹ��ڵ�ĸ��׵����Լ��������1�����ӽڵ� ��Ϊ���
			if (node->color == _rb_tree_red &&
				node->parent->parent == node)
				node = node->right;
			
			else if (node->left != 0) {			//��������ӽڵ�
				base_ptr y = node->left;		//������һ����һֱ����
				while (y->right != 0)
					y = y->right;
				node = y;
			}
			//�Ȳ��Ǹ��ڵ㣬Ҳ�����ӽڵ㣬�ҵ���һ��������ڵ�ĸ��ڵ�
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

		
		
	};

	/*ȫ�ֺ���*/
	 inline bool operator ==(const _rb_tree_base_iterator& __x, const _rb_tree_base_iterator& __y) {
		return __x.node == __y.node;
	}
	 inline bool operator !=(const _rb_tree_base_iterator& __x, const _rb_tree_base_iterator& __y) {
		return __x.node != __y.node;
	}


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
		template<class T>
		
		void destory_node(link_type p) {
			destroy(&(p->value_field));
			put_node(p);
		}
	

	protected:

		size_type node_count;		//�ڵ�����
		link_type header;
		Compare key_compare;	//��ֵ��С�Ƚ�׼��

		link_type& root() const { return (link_type&)header->parent; }
		link_type& left_most() const { return (link_type&)header->left; }
		link_type& right_most() const { return (link_type&)header->right; }

		//��ýڵ�x ��Ա
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

		//��ȡ����ֵ�ͼ�Сֵ
		static link_type minmum(link_type x) {
			return (link_type)_rb_tree_node_base::minmum();
		}
		static link_type maxmum(link_type x) {
			return (link_type)_rb_tree_node_base::maxmum();
		}

	public:
		typedef _rb_tree_iterator<value_type, reference, pointer> iterator;
		typedef _rb_tree_iterator<value_type, const reference, const pointer> const_iterator;
	

	private:
		void init() {
			header = get_node();					//����һ���ڵ�ռ�
			color(header) = _rb_tree_red;		//headerΪ��ɫ���Դ�����root

			root() = 0;
			left_most() = header;					//��header�����ӽڵ�Ϊ�Լ�
			right_most() = header;				//��header�����ӽڵ�Ϊ�Լ�
		}

	public:
		inline void erase(iterator position) {
			link_type y = (link_type)rb_tree_rebalance_for_erase(position.node,
				header->parent,
				header->left,
				header->right);
				destroy(&(y->value_field));
				put_node(y);
			--node_count;
		}
		size_type erase(const key_type& x);
		void erase(iterator first, iterator last) {
			if (first == begin() && last == end())
				clear();
			else
				while (first != last) erase(first++);
		}
		void erase(const key_type* first, const key_type* last);
		void clear() {
			if (node_count != 0) {
				erase(root());
				left_most() = header;
				root() = 0;
				right_most() = header;
				node_count = 0;
			}
		}

	public:
		rb_tree(const Compare& comp = Compare()) :node_count(0), key_compare(comp) {
			init();
		}
		~rb_tree() {
			clear();
		}
		rb_tree<Key, Value, KeyOfValue, Compare>&
			operator=(const rb_tree<Key, Value, KeyOfValue, Compare>& x) {
			if (this != &x) {
				clear();
				node_count = 0;
				key_compare = x.key_compare;
				if (x.root() == 0) {
					root() = 0;
					left_most() = header;
					right_most() = header;
				}
				else {
					root() = copy(x.root(), header);
					left_most = minmum(root());
					right_most = maxmum(root());
					node_count = x.node_count;
				}
			}
		}

	public:
		Compare key_com()const { return key_compare; }
		iterator begin() { return left_most(); }
		iterator end() { return header;}
		bool empty()const { return node_count == 0; }
		size_type size() const { return node_count; }
		size_type max_szie()const { return size_type(-1); }
	public:



		/***************************������صĺ���************************************/
		inline void _rb_tree_rebalance(_rb_tree_node_base *x, _rb_tree_node_base*& root) {
			x->color = _rb_tree_red;																//�½ڵ����Ϊ��ɫ
			while (x != root && x->parent->color == _rb_tree_red) {			//���ڵ�Ϊ��
				if (x->parent == x->parent->parent->left) {							//���ڵ�Ϊ�游�ڵ�����ӽڵ�
					_rb_tree_node_base *y = x->parent->parent->right;			//yΪ�����ڵ�
					if (y&&y->color == _rb_tree_red) {										//�����ڵ���ڣ���Ϊ��ɫ
						x->parent->color = _rb_tree_black;								//���ĸ��ڵ�Ϊ��
						y->color = _rb_tree_black;												//���Ĳ����ڵ�Ϊ��
						x->parent->parent->color = _rb_tree_red;					//�����游�ڵ�Ϊ��
						x = x->parent->parent;
					}
					else {																					//�޲����ڵ㣬�򲮸��ڵ�Ϊ��ɫ
						if (x == x->parent->right) {												//�½ڵ�Ϊ���ڵ�����ӽڵ�
							x = x->parent;
							_rb_tree_rotate_left(x, root);											//����
						}
					}
					x->parent->color = _rb_tree_black;									//���ڵ����Ϊ��ɫ
					x->parent->parent->color = _rb_tree_red;						//�游����Ϊ��ɫ
					_rb_tree_rotate_right(x->parent->parent, root);					//����
				}
				else {																						//���ڵ�Ϊ�游�ڵ���ҽڵ�
					_rb_tree_node_base *y = x->parent->parent->left;			//yΪ�����ڵ�
					if (y && y->color == _rb_tree_red) {									//�����ڵ���ڣ���Ϊ��ɫ
						x->parent->color = _rb_tree_black;								//���ĸ��ڵ�Ϊ��ɫ
						y->color = _rb_tree_black;												//���Ĳ����ڵ�Ϊ��ɫ
						x->parent->parent->color = _rb_tree_red;					//�游�ڵ�Ϊ��ɫ
						x = x->parent->parent;													//׼���������ϲ���
					}
					else {																					//�޲����ڵ�򲮸��ڵ�Ϊ��ɫ
						if (x == x->parent->left) {
							x = x->parent;
							_rb_tree_rotate_right(x, root);										//����
						}
						x->parent->color = _rb_tree_black;
						x->parent->parent->color = _rb_tree_red;
						_rb_tree_rotate_left(x->parent->parent, root);				//����
					}
				}

			}//while����
			root->color = _rb_tree_black;
		}
		inline void _rb_tree_rotate_left(_rb_tree_node_base *x, _rb_tree_node_base*& root) {
			//xΪ��ת�ڵ�
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
			//xΪ��ת�ڵ�
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
		/******************************erase����*************************************************/
		inline _rb_tree_node_base*
			rb_tree_rebalance_for_erase(_rb_tree_node_base* z,
				_rb_tree_node_base*& root,
				_rb_tree_node_base*&  leftmost,
				_rb_tree_node_base*&  rightmost) {
			_rb_tree_node_base* y = z;
			_rb_tree_node_base* x = 0;
			_rb_tree_node_base* x_parent = 0;
			if (y->left == 0)
				x = y->right;
			else {
				if (y->right == 0)
					x = y->left;
				else {
					y = y->right;
					while (y->right != 0)
						y = y->right;
					x = x->right;
				}
			}
			if (y != z) {          // relink y in place of z.  y is z's successor
				z->left->parent = y;
				y->left = z->left;
				if (y != z->right) {
					x->parent = y->parent;
					if (x) x->parent = y->parent;
					y->parent->left = x;      // __y must be a child of left
					y->right = z->right;
					z->right->parent = y;
				}
				else
					x_parent = y;
				if (root == z)
					root = y;
				else if (z->parent->left == z)
					z->parent->left = y;
				else
					z->parent->right = y;
				y->parent = z->parent;
				std::swap(y->color, z->color);
				y = z;
				// __y now points to node to be actually deleted
			}
			else {                        // __y == __z
				x_parent = y->parent;
				if (x) x->parent = y->parent;
				if (root == z)
					root = x;
				else
					if (z->parent->left == z)
						z->parent->left = x;
					else
						z->parent->right = x;
				if (leftmost == z)
					if (z->right == 0)        // __z->left must be null also
						leftmost = z->parent;
				// makes __leftmost == header if __z == __root
					else
						leftmost = _rb_tree_node_base::minmum(x);
				if (rightmost == z)
					if (z->left == 0)         // __z->right must be null also
						rightmost = z->parent;
				// makes __rightmost == header if __z == __root
					else                      // __x == __z->left
						rightmost = _rb_tree_node_base::maxmum(x);
			}
			if (y->color != _rb_tree_red) {
				while (x != root && (x == 0 || x->color == _rb_tree_black))
					if (x == x_parent->left) {
						_rb_tree_node_base* __w = x_parent->right;
						if (__w->color == _rb_tree_red) {
							__w->color = _rb_tree_black;
							x_parent->color = _rb_tree_red;
							_rb_tree_rotate_left(x_parent, root);
							__w = x_parent->right;
						}
						if ((__w->left == 0 ||
							__w->left->color == _rb_tree_black) &&
							(__w->right == 0 ||
								__w->right->color == _rb_tree_black)) {
							__w->color = _rb_tree_red;
							x = x_parent;
							x_parent = x_parent->parent;
						}
						else {
							if (__w->right == 0 ||
								__w->right->color == _rb_tree_black) {
								if (__w->left) __w->left->color = _rb_tree_black;
								__w->color = _rb_tree_red;
								_rb_tree_rotate_right(__w, root);
								__w = x_parent->right;
							}
							__w->color = x_parent->color;
							x_parent->color = _rb_tree_black;
							if (__w->right) __w->right->color = _rb_tree_black;
							_rb_tree_rotate_left(x_parent, root);
							break;
						}
					}
					else {                  // same as above, with right <-> left.
						_rb_tree_node_base* __w = x_parent->left;
						if (__w->color == _rb_tree_red) {
							__w->color = _rb_tree_black;
							x_parent->color = _rb_tree_red;
							_rb_tree_rotate_right(x_parent, root);
							__w = x_parent->left;
						}
						if ((__w->right == 0 ||
							__w->right->color == _rb_tree_black) &&
							(__w->left == 0 ||
								__w->left->color == _rb_tree_black)) {
							__w->color = _rb_tree_red;
							x = x_parent;
							x_parent = x_parent->parent;
						}
						else {
							if (__w->left == 0 ||
								__w->left->color == _rb_tree_black) {
								if (__w->right) __w->right->color = _rb_tree_black;
								__w->color = _rb_tree_red;
								_rb_tree_rotate_left(__w, root);
								__w = x_parent->left;
							}
							__w->color = x_parent->color;
							x_parent->color = _rb_tree_black;
							if (__w->left) __w->left->color = _rb_tree_black;
							_rb_tree_rotate_right(x_parent, root);
							break;
						}
					}
					if (x) x->color = _rb_tree_black;
			}
			return y;

		}

		/***********************************************������غ���*****************************************************/
		iterator _insert(base_ptr x_, base_ptr y_, const Value& v) {
			//xΪ�²���ڵ㣬yΪ����ڵ�֮���ڵ㣬����vΪ��ֵ
			link_type x = (link_type)x_;
			link_type y = (link_type)y_;

			link_type z;

			if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
				z = creat_node(v);		//����һ���½ڵ�
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
			parent(z) = y;				//�趨�½ڵ�ĸ��ڵ�
			left(z) = 0;					//�趨�½ڵ����ڵ�
			right(z) = 0;					//�趨�½ڵ���ҽڵ�

			_rb_tree_rebalance(z, header->parent);
			++node_count;
			return iterator(z);
		}

		pair<iterator, bool> insert_unique(const value_type &v) {
			link_type y = header;
			link_type x = root();
			bool comp = true;
			while (x != 0) {
				y = x;
				comp = key_compare(KeyOfValue()(v), key(x));
				x = comp ? left(x) : right(x);
			}
			//�뿪ѭ����y��ָ�Ĳ���ڵ�ĸ��ڵ㣬��ʱ��ΪҶ�ڵ�

			iterator j = iterator(y);	//������ָ�����ڵ�ĸ��ڵ�
			if (comp)						//compΪ�棬��ʾ���󣬲��������
				if (j == begin())		//�������ڵ�֮���ڵ�Ϊ����ڵ�
					return pair<iterator, bool>(_insert(x, y, v), true);
				else
					--j;
			if (key_compare(key(link_type(j.node)), KeyOfValue()(v)))	//�����Ҳ�
				return pair<iterator, bool>(_insert(x, y, v), true);

			//�������ˣ���ʾ��ֵһ�������м�ֵ�ظ���������
			return pair<iterator, bool>(j, false);
		}

		iterator insert_equal(const value_type &v) {
			link_type y = header;
			link_type x = root();		//�Ӹ��ڵ㿪ʼ
			while (x != 0) {
				y = x;
				x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
				//����������С����
			}
			return _insert(x, y, v);
			//xΪ��ֵ����㣬yΪ�����ĸ��ڵ㣬vΪ��ֵ
		}

		/******************************************���� ***********************************/
		iterator find(const Key &k) {
			link_type y = header;
			link_type x = root();
			while (x != 0) {
				if (!key_compare(KeyOfValue(x), k))
				{
					y = x;
					x = x->left;
				}
				else x = x->right;
			}

			iterator j = iterator(y);
			return (j == end() || key_compare(k, KeyOfValue(j.node))) ? end() : j;
		}
	};
}
#endif // !_rb_TREE_H



