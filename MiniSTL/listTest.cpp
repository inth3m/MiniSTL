#include"ALLOC.h"
#include"allocator.h"
#include"list.h"
#include"vectorTest.h"
#include<iostream>
#include<string>
using namespace MiniSTL;
struct student {
	string name;
	int age;
};

int main() {
	list<int> l;
	//list<student> s;
	//student stu;
	//stu.name = "chenyong";
	//stu.age = 15;
	//s.push_back(stu);
	//s.push_back(stu);
	//list<student>::iterator sit = s.begin();
	//cout << sit.node->data.name << endl;
	//cout << sit.node->data.age << endl;
	l.push_back(6);
	l.push_back(5);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);

	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);
	l.push_back(8);


	list<int>::iterator it = l.begin();
	for (; it != l.end(); it++)
		cout << *it << endl;
	
//	vectorTest();
	system("pause");
}