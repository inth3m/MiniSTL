
#include"ALLOC.h"
#include"allocator.h"
#include"vector.h"
#include"list.h"
#include"deque.h"
#include<iostream>
#include<string>
class student {
public:
	string name;
	int age;
};

void vectorTest() {

	int i;
	vector<int> iv(2, 9);
	cout << "size = " << iv.size() << endl;
	cout << "capacity = " << iv.capactity() <<endl;

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

	deque<int> ideq(20, 9);
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
	cout << "size() = " << ideq.size()<<endl;
}

int main() {
	
	dequeTest();
	system("pause");
}