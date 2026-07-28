// DataStructures.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "Containers/List/List.h"
#include "Containers/Vector/Vector.h"
#include "Containers/RBTree/RBTree.h"
#include "Benchmark/BenchmarkDSAndSTL.h"
#include "Containers/HashMap/HashMapChaining/HashMapChaining.h"
#include "Containers/HashMap/HashMapOpenAddressing/HashMapOpenAddressing.h"
#include <unordered_map>
#include <list>
#include <vector>
#include <set>
#include <chrono>
using namespace std;
using namespace ds;

int main()
{
	constexpr std::size_t element_count = 100'000;
	VectorBenchmark<ds::Vector<int>, std::vector<int>> vectorBenchmark(100000);
	vectorBenchmark.run_all();

	ListBenchmark<ds::List<int>, std::list<int>> listBenchmark(100000);
	listBenchmark.run_all();

	RBTreeBenchmark<ds::RBTree<int>, std::set<int>> rbTreeBenchmark(100000);
	rbTreeBenchmark.run_all();

	MapBenchmark<ds::HashMapChaining<int, int>, std::unordered_map<int, int>> mapBenchmark(100000);
	mapBenchmark.run_all();
	
	OpenAddressingMapBenchmark<HashMapOpenAddressing<int, int>,	std::unordered_map<int, int>> mapOpenBenchmark(element_count);
	mapOpenBenchmark.run_all();

	return 0;
}
