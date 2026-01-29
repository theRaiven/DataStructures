#pragma once
#include <chrono>
#include <random>
#include <set>
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

template<typename MyRBTree, typename StdSet>
class RBTreeBenchmark
{
private:
	size_t n_;
public:
	explicit RBTreeBenchmark(size_t n) : n_(n) {}

	void run_all()
	{
		run("insert",
			[&] { return insert<MyRBTree>(n_); },
			[&] { return insert<StdSet>(n_); });

		run("duplicate_insert",
			[&] { return duplicate_insert<MyRBTree>(n_); },
			[&] { return duplicate_insert<StdSet>(n_); });

		run("erase",
			[&] { return erase<MyRBTree>(n_); },
			[&] { return erase<StdSet>(n_); });

		run("erase_random",
			[&] { return erase_random<MyRBTree>(n_); },
			[&] { return erase_random<StdSet>(n_); });

		run("clear",
			[&] { return clear<MyRBTree>(n_); },
			[&] { return clear<StdSet>(n_); });

		run("find",
			[&] { return find<MyRBTree>(n_); },
			[&] { return find<StdSet>(n_); });

		run("lower_upper",
			[&] { return lower_upper<MyRBTree>(n_); },
			[&] { return lower_upper<StdSet>(n_); });

		run("iteration",
			[&] { return iteration<MyRBTree>(n_); },
			[&] { return iteration<StdSet>(n_); });
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

	template<typename TreeType>
	long long insert(size_t n)
	{
		TreeType tree;

		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					tree.insert(i);
				}
			});
	}

	template<typename TreeType>
	long long duplicate_insert(size_t n)
	{
		TreeType tree;
		for (size_t i = 0; i < n; ++i)
		{
			tree.insert(i);
		}
		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					tree.insert(i);
				}
			});
	}
	template<typename TreeType>
	long long erase(size_t n)
	{
		TreeType tree;
		for (size_t i = 0; i < n; ++i)
		{
			tree.insert(i);
		}

		return benchmark([&]()
			{
				int i = 0;
				while (!tree.empty())
				{
					tree.erase(i++);
				}
			});
	}

	template<typename TreeType>
	long long erase_random(size_t n)
	{
		TreeType tree;
		std::vector<int> keys(n);

		for (size_t i = 0; i < n; ++i)
		{
			keys[i] = i;
			tree.insert(i);
		}

		std::mt19937 gen(42);
		std::shuffle(keys.begin(), keys.end(), gen);

		return benchmark([&]
		{
			for (int k : keys)
			{
				tree.erase(k);
			}
		});
	}

	template<typename TreeType>
	long long find(size_t n)
	{
		TreeType tree;
		
		return benchmark([&]()
		{
			for (size_t i = 0; i < n; ++i)
			{
				tree.insert(i);
				tree.find(i);
			}
		});
	}

	template<typename TreeType>
	long long clear(size_t n)
	{
		TreeType tree;
		for (size_t i = 0; i < n; ++i)
			tree.insert(i);

		return benchmark([&]()
			{
				tree.clear();
			});
	}

	template<typename TreeType>
	long long iteration(size_t n)
	{
		TreeType tree;
		for (size_t i = 0; i < n; ++i)
			tree.insert(i);

		volatile size_t sum = 0;

		return benchmark([&]
		{
			for (auto x : tree)
				sum += x;
		});
	}

	template<typename TreeType>
	long long lower_upper(size_t n)
	{
		TreeType tree;
		for (size_t i = 0; i < n; ++i)
			tree.insert(i);
		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					tree.lower_bound(i);
					tree.upper_bound(i);
				}
			});
	}

	void print(const string name, long long my, long long stl)
	{
		cout << name << ":\n";
		cout << "  MyTree   = " << my << " ms\n";
		cout << "  std::set= " << stl << " ms\n\n";
	}
};