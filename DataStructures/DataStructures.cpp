// DataStructures.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "List.h"
#include <list>
#include <chrono>
using namespace std;
using Clock = chrono::high_resolution_clock;

template<typename MyList, typename StdList>
class ListBenchmark
{
private:
	size_t n_;
public:
	explicit ListBenchmark(size_t n) : n_(n) {}

	void run_all()
	{
		run("push_back",
			[&] { return push_back<MyList>(n_); },
			[&] { return push_back<StdList>(n_); });

		run("push_front",
			[&] { return push_front<MyList>(n_); },
			[&] { return push_front<StdList>(n_); });

		run("pop_back",
			[&] { return pop_back<MyList>(n_); },
			[&] { return pop_back<StdList>(n_); });

		run("pop_front",
			[&] { return pop_front<MyList>(n_); },
			[&] { return pop_front<StdList>(n_); });

		run("insert_middle",
			[&] { return insert_middle<MyList>(n_); },
			[&] { return insert_middle<StdList>(n_); });

		run("erase_middle",
			[&] { return erase_middle<MyList>(n_); },
			[&] { return erase_middle<StdList>(n_); });

		run("clear",
			[&] { return clear<MyList>(n_); },
			[&] { return clear<StdList>(n_); });

		run("sort",
			[&] { return sort<MyList>(100'000); },
			[&] { return sort<StdList>(100'000); });
	}

private:
	template<typename F1, typename F2>
	void run(const string& name, F1 my, F2 stl)
	{
		long long my_time = my();
		long long stl_time = stl();
		print(name, my_time, stl_time);
	}

	template<typename Func>
	long long benchmark(Func f)
	{
		auto start = Clock::now();
		f();
		auto end = Clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	template<typename ListType>
	long long push_back(size_t n)
	{
		ListType List;

		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					List.push_back(i);
				}
			});
	}

	template<typename ListType>
	long long push_front(size_t n)
	{
		ListType List;

		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					List.push_front(i);
				}
			});
	}

	template<typename ListType>
	long long pop_back(size_t n)
	{
		ListType List;
		for (size_t i = 0; i < n; ++i)
		{
			List.push_back(i);
		}

		return benchmark([&]()
			{
				while (!List.empty())
				{
					List.pop_back();
				}
			});
	}

	template<typename ListType>
	long long pop_front(size_t n)
	{
		ListType List;
		for (size_t i = 0; i < n; ++i)
		{
			List.push_back(i);
		}

		return benchmark([&]()
			{
				while (!List.empty())
				{
					List.pop_front();
				}
			});
	}

	template<typename ListType>
	long long insert_middle(size_t n)
	{
		ListType List;
		for (size_t i = 0; i < n; ++i)
		{
			List.push_back(i);
		}

		auto it = List.begin();

		for (size_t i = 0; i < n / 2; ++i)
		{
			++it;
		}

		return benchmark([&]()
			{
				List.insert(it, -1);
			});
	}

	template<typename ListType>
	long long erase_middle(size_t n)
	{
		ListType List;
		for (size_t i = 0; i < n; ++i)
		{
			List.push_back(i);
		}

		auto it = List.begin();

		for (size_t i = 0; i < n / 2; ++i)
		{
			++it;
		}

		return benchmark([&]()
			{
				List.erase(it);
			});
	}

	template<typename ListType>
	long long clear(size_t n)
	{
		ListType List;
		for (size_t i = 0; i < n; ++i)
			List.push_back(i);

		return benchmark([&]()
			{
				List.clear();
			});
	}

	template<typename ListType>
	long long sort(size_t n)
	{
		ListType list;
		for (size_t i = 0; i < n; ++i)
			list.push_back(rand());

		return benchmark([&]()
			{
				list.sort();
			});
	}

	void print(const string name, long long my, long long stl)
	{
		cout << name << ":\n";
		cout << "  MyList   = " << my << " ms\n";
		cout << "  std::list= " << stl << " ms\n\n";
	}
};

int main()
{
	/*ListBenchmark<List<int>, std::list<int>> bench(1'000'000);
	bench.run_all();*/
	List<int> mylist{ 5,4,3,2,1 };
	cout << "Before sort:\n";
	for (auto v : mylist)
		cout << v << " ";
	cout << "\nAfter sort:\n";
	mylist.sort();

	for(auto v : mylist)
		cout << v << " ";

	return 0;
}
