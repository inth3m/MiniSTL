#include"deque.h"
#include"vector.h"
#include"stack.h"
#include"queue.h"
#include"heap.h"
#include"priority_queue.h"
#include"slist.h"
#include"RB_tree.h"
#include<type_traits>

using namespace MiniSTL;
void vectorTest() {

	int i;
	vector<int> iv(2, 9);
	cout << "size = " << iv.size() << endl;
	cout << "capacity = " << iv.capactity() << endl;

	iv.push_back(2);
	cout << "size = " << iv.size() << endl;
	cout << "capacity = " << iv.capactity() << endl;

	iv.push_back(3);
	cout << "size = " << iv.size() << endl;
	cout << "capacity = " << iv.capactity() << endl;

	iv.push_back(4);
	cout << "size = " << iv.size() << endl;
	cout << "capacity = " << iv.capactity() << endl;

	for (i = 0; i < iv.size(); ++i)
		cout << iv[i] << " ";
	cout << endl;

	iv.push_back(5);
	cout << "size = " << iv.size() << endl;
	cout << "capacity = " << iv.capactity() << endl;

	for (i = 0; i < iv.size(); ++i)
		cout << iv[i] << " ";
	cout << endl;

	iv.pop_back();
	iv.pop_back();
	cout << "size = " << iv.size() << endl;
	cout << "capacity = " << iv.capactity() << endl;


	iv.pop_back();
	cout << "size = " << iv.size() << endl;
	cout << "capacity = " << iv.capactity() << endl;


	cout << "size = " << iv.size() << endl;
	cout << "capacity = " << iv.capactity() << endl;
	for (i = 0; i < iv.size(); ++i)
		cout << iv[i] << " ";
	cout << endl;


	vector<int>::iterator it = iv.begin();
	iv.erase(it);

	for (i = 0; i < iv.size(); ++i)
		cout << iv[i] << " ";
	cout << endl;

}

void dequeTest() {

	MiniSTL::deque<int, MiniSTL::allocator<int>> ideq(20,9);
	cout << "size = " << ideq.size() << endl;

	for (int i = 0; i < ideq.size(); ++i) {
		ideq[i] = i;
	}
	cout << endl;

	for (int i = 0; i < 3; i++) {
		ideq.push_back(i);
	}

	for (int i = 0; i < ideq.size(); i++) {
		cout << ideq[i] << " ";
	}
	cout << endl;
	cout << "size() = " << ideq.size() << endl;
}

void stackTest() {
	stack<int> s;
	s.push(1);
	s.push(3); 
	s.push(5); 
	s.push(7);
	cout << "size = "<<s.size() << endl;;
	s.pop();
	cout << s.top() << endl;;
	cout << "size = " << s.size() << endl;;
	s.pop();
	cout << s.top() << endl;;
	cout << "size = " << s.size() << endl;;
	s.pop();
	cout << s.top() << endl;;
	cout << "size = " << s.size() << endl;;
	s.pop();
}

void queueTest() {
	queue<int> iqueue;
	iqueue.push(1);
	iqueue.push(3);
	iqueue.push(5);
	iqueue.push(7);

	cout << "size = " << iqueue.size() << endl;
	cout << "front = " << iqueue.front() << endl;

	iqueue.pop(); cout << "front = " << iqueue.front() << endl;
	iqueue.pop(); cout << "front = " << iqueue.front() << endl;
	iqueue.pop(); cout << "front = " << iqueue.front() << endl;
	cout << "size = " << iqueue.size() << endl;
}

void heapTest() {

	int ia[9] = { 0,1,2,3,4,8,9,3,5 };
	vector<int> ivec(ia, ia + 9);

	MiniSTL::make_heap(ivec.begin(), ivec.end());
	for (int i = 0; i < ivec.size(); ++i) {
		cout << ivec[i] << ' ';
	}
	cout << endl;
	
	ivec.push_back(7);
	MiniSTL::make_heap(ivec.begin(), ivec.end());
	for (int i = 0; i < ivec.size(); ++i) {
		cout << ivec[i] << ' ';
	}
	cout << endl;

	MiniSTL::pop_heap(ivec.begin(), ivec.end());
	cout << ivec.back() << endl;
	ivec.pop_back();
}

void priority_queueTest() {
	int ia[9] = { 0,1,2,3,4,8,9,3,5 };
	priortyu_queue<int> ipq(ia, ia + 9);
	cout << "szie = " << ipq.size() << endl;

	while (!ipq.empty()) {
		cout << ipq.top() << " ";
		ipq.pop();
	}
	cout << endl;
}

void slistTest() {
	int i;
	slist<int>islit;
	cout << "size = " << islit.size() << endl;

	islit.push_front(9);
	islit.push_front(1);
	islit.push_front(2);
	islit.push_front(3);
	islit.push_front(4);
	cout << "size = " << islit.size() << endl;

	slist<int>::iterator ite = islit.begin();
	slist<int>::iterator ite2 = islit.end();

	for (; ite != ite2; ite++) {
		cout << *ite << " ";
	}
	cout << endl;
}

void rbtreeTest() {
	rb_tree<int, int, identity<int>, less<int>> itree;
	std::cout << itree.size() << endl;

	itree.insert_unique(10);
	itree.insert_unique(7);
	itree.insert_unique(8);

	/*itree.insert_unique(15);
	itree.insert_unique(5);
	itree.insert_unique(6);

	itree.insert_unique(11);

	itree.insert_unique(13);
	itree.insert_unique(12);
*/
	cout << itree.size() << endl;
	rb_tree<int, int, identity<int>, less<int>>::iterator ite1 = itree.begin();
	rb_tree<int, int, identity<int>, less<int>>::iterator ite2 = itree.end();

	for (; ite1 != ite2; ++ite1) {
		cout << *ite1 << " ";
	}


}
int main() {

	//stackTest();
	//dequeTest();
	//queueTest();
	//heapTest();
	//priority_queueTest();
	//slistTest();
	rbtreeTest();
	system("pause");
}