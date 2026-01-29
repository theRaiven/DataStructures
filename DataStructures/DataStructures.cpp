// DataStructures.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "List.h"
#include "RBTree.h"
#include "BenchmarkDSAndSTL.h"
#include <list>
#include <set>
#include <chrono>
using namespace std;

int main()
{
	/*ListBenchmark<List<int>, std::list<int>> ListBench(1'000'000);
	ListBench.run_all();*/

	RBTreeBenchmark<RBTree<int>, std::set<int>> RBTBench(1'000'000);
	RBTBench.run_all();
	/*List<int> mylist{ 5,4,3,2,1 };
	cout << "Before sort:\n";
	for (auto v : mylist)
		cout << v << " ";
	cout << "\nAfter sort:\n";
	mylist.sort();

	for(auto v : mylist)
		cout << v << " ";*/

	return 0;
}
