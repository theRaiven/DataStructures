#pragma once
#include <chrono>
#include <random>
#include <set>
using namespace std;
using Clock = chrono::high_resolution_clock;

class BenchmarkPrinter
{
public:
	virtual ~BenchmarkPrinter() = default;

	virtual std::string my_container_name() const
	{
		return "Benchmark";
	}

	virtual std::string std_container_name() const
	{
		return "Benchmark";
	}

	virtual void print_header() const
	{
		std::cout << std::left
			<< std::setw(22+2) << "Operation"
			<< std::setw(15+2) << my_container_name()
			<< std::setw(15+2) << std_container_name()
			<< std::setw(12+2) << "Ratio"
			<< '\n';

		std::cout << std::string(64, '-') << '\n';
	}

	virtual void print_row(const std::string& name, long long my, long long stl) const
	{
		double ratio = stl == 0 ? 0.0 :
			static_cast<double>(my) / stl;

		std::cout << std::left
			<< std::setw(22) << name << "| "
			<< std::setw(15) << (std::to_string(my) + " micro s") << "| "
			<< std::setw(15) << (std::to_string(stl) + " micro s") << "| "
			<< std::fixed << std::setprecision(2)
			<< ratio << "x\n";
	}

	virtual void print_footer() const
	{
		std::cout << std::string(64, '-') << "\n\n";
	}
};

template<typename MyList, typename StdList>
class ListBenchmark : public BenchmarkPrinter
{
private:
	size_t n_;
public:
	explicit ListBenchmark(size_t n) : n_(n) {}

	virtual std::string my_container_name() const override
	{
		return "MyList";
	}

	virtual std::string std_container_name() const override
	{
		return "std::list";
	}

	void run_all()
	{
		print_header();

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

		print_footer();
	}

private:
	template<typename F1, typename F2>
	void run(const string& name, F1 my, F2 stl)
	{
		long long my_time = my();
		long long stl_time = stl();
		print_row(name, my_time, stl_time);
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

	void print(const string& name, long long my, long long stl)
	{
		cout << name << ":\n";
		cout << "  MyList   = " << my << "  micro s\n";
		cout << "  std::list= " << stl << "  micro s\n\n";
	}
};

template<typename MyRBTree, typename StdSet>
class RBTreeBenchmark : public BenchmarkPrinter
{
private:
	size_t n_;
public:
	explicit RBTreeBenchmark(size_t n) : n_(n) {}

	virtual std::string my_container_name() const override
	{
		return "MyRBTree";
	}

	virtual std::string std_container_name() const override
	{
		return "std::set";
	}

	void run_all()
	{
		print_header();

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

		print_footer();
	}

private:
	template<typename F1, typename F2>
	void run(const string& name, F1 my, F2 stl)
	{
		long long my_time = my();
		long long stl_time = stl();
		print_row(name, my_time, stl_time);
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

	void print(const string& name, long long my, long long stl)
	{
		cout << name << ":\n";
		cout << "  MyTree   = " << my << "  micro s\n";
		cout << "  std::set= " << stl << "  micro s\n\n";
	}
};

template<typename MyHashTable, typename StdMap>
class MapBenchmark : public BenchmarkPrinter
{
private:
	size_t n_;
public:
	explicit MapBenchmark(size_t n) : n_(n) {}

	virtual std::string my_container_name() const override
	{
		return "MyHashTable";
	}

	virtual std::string std_container_name() const override
	{
		return "std::unordered_map / std::map";
	}

	void run_all()
	{
		print_header();

		run("emplace",
			[&] { return emplace<MyHashTable>(n_); },
			[&] { return emplace<StdMap>(n_); });

		run("duplicate_emplace",
			[&] { return duplicate_emplace<MyHashTable>(n_); },
			[&] { return duplicate_emplace<StdMap>(n_); });

		run("erase",
			[&] { return erase<MyHashTable>(n_); },
			[&] { return erase<StdMap>(n_); });

		run("erase_random",
			[&] { return erase_random<MyHashTable>(n_); },
			[&] { return erase_random<StdMap>(n_); });

		run("clear",
			[&] { return clear<MyHashTable>(n_); },
			[&] { return clear<StdMap>(n_); });

		run("iteration",
			[&] { return iteration<MyHashTable>(n_); },
			[&] { return iteration<StdMap>(n_); });

		print_footer();
	}

private:
	template<typename F1, typename F2>
	void run(const string& name, F1 my, F2 stl)
	{
		long long my_time = my();
		long long stl_time = stl();
		print_row(name, my_time, stl_time);
	}

	template<typename Func>
	long long benchmark(Func f)
	{
		auto start = Clock::now();
		f();
		auto end = Clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	template<typename MapType>
	long long emplace(size_t n)
	{
		MapType map;

		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					map.emplace(i, i);
				}
			});
	}

	template<typename MapType>
	long long duplicate_emplace(size_t n)
	{
		MapType map;
		for (size_t i = 0; i < n; ++i)
		{
			map.emplace(i, i);
		}
		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					map.emplace(i, i);
				}
			});
	}

	template<typename MapType>
	long long erase(size_t n)
	{
		MapType map;
		for (size_t i = 0; i < n; ++i)
		{
			map.emplace(i, i);
		}

		return benchmark([&]()
			{
				int i = 0;
				for (size_t i = 0; i < n; ++i)
					map.erase(i);
			});
	}

	template<typename MapType>
	long long erase_random(size_t n)
	{
		MapType map;
		std::vector<int> keys(n);

		for (size_t i = 0; i < n; ++i)
		{
			keys[i] = i;
			map.emplace(i, i);
		}

		std::mt19937 gen(42);
		std::shuffle(keys.begin(), keys.end(), gen);

		return benchmark([&]
			{
				for (int k : keys)
				{
					map.erase(k);
				}
			});
	}

	template<typename MapType>
	long long clear(size_t n)
	{
		MapType map;
		for (size_t i = 0; i < n; ++i)
			map.emplace(i, i);

		return benchmark([&]()
			{
				map.clear();
			});
	}

	template<typename MapType>
	long long iteration(size_t n)
	{
		MapType map;
		for (size_t i = 0; i < n; ++i)
			map.emplace(i, i);

		volatile size_t sum = 0;

		return benchmark([&]
			{
				for (auto& [k, v] : map)
					sum += k;
			});
	}

	template<typename MapType>
	long long lower_upper(size_t n)
	{
		MapType map;
		for (size_t i = 0; i < n; ++i)
			map.emplace(i,i);
		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					map.lower_bound(i);
					map.upper_bound(i);
				}
			});
	}

	void print(const string& name, long long my, long long stl)
	{
		cout << name << ":\n";
		cout << "  MyHashTable                   = " << my << "  micro s\n";
		cout << "  std::unordered_map / std::map = " << stl << "  micro s\n\n";
	}
};

template<typename MyVector, typename StdVector>
class VectorBenchmark : public BenchmarkPrinter
{
private:
	size_t n_;
public:
	explicit VectorBenchmark(size_t n) : n_(n) {}

	virtual std::string my_container_name() const override
	{
		return "MyVector";
	}

	virtual std::string std_container_name() const override
	{
		return "std::vector";
	}

	void run_all()
	{
		print_header();

		run("push_back",
			[&] { return push_back<MyVector>(n_); },
			[&] { return push_back<StdVector>(n_); });

		run("pop_back",
			[&] { return pop_back<MyVector>(n_); },
			[&] { return pop_back<StdVector>(n_); });


		run("insert_middle",
			[&] { return insert_middle<MyVector>(n_); },
			[&] { return insert_middle<StdVector>(n_); });
	
		run("insert_init_list",
			[&] { return insert_init_list<MyVector>(n_); },
			[&] { return insert_init_list<StdVector>(n_); });

		run("remove_init_list",
			[&] { return erase_range_list<MyVector>(n_); },
			[&] { return erase_range_list<StdVector>(n_); });

		run("erase_middle",
			[&] { return erase_middle<MyVector>(n_); },
			[&] { return erase_middle<StdVector>(n_); });

		run("clear",
			[&] { return clear<MyVector>(n_); },
			[&] { return clear<StdVector>(n_); });
		
		run("reserve",
			[&] { return reserve<MyVector>(n_); },
			[&] { return reserve<StdVector>(n_); });

		run("resize",
			[&] { return resize<MyVector>(n_); },
			[&] { return resize<StdVector>(n_); });

		run("shrink_to_fit",
			[&] { return shrink_to_fit<MyVector>(n_); },
			[&] { return shrink_to_fit<StdVector>(n_); });

		run("assign",
			[&] { return assign<MyVector>(n_); },
			[&] { return assign<StdVector>(n_); });

		run("emplace_back",
			[&] { return emplace_back<MyVector>(n_); },
			[&] { return emplace_back<StdVector>(n_); });

		run("copy_constructor",
			[&] { return copy_constructor<MyVector>(n_); },
			[&] { return copy_constructor<StdVector>(n_); });

		run("move_constructor",
			[&] { return move_constructor<MyVector>(n_); },
			[&] { return move_constructor<StdVector>(n_); });

		run("copy_assignment",
			[&] { return copy_assignment<MyVector>(n_); },
			[&] { return copy_assignment<StdVector>(n_); });

		run("move_assignment",
			[&] { return move_assignment<MyVector>(n_); },
			[&] { return move_assignment<StdVector>(n_); });

		run("swap",
			[&] { return swap<MyVector>(n_); },
			[&] { return swap<StdVector>(n_); });

		print_footer();
	}

private:
	template<typename F1, typename F2>
	void run(const string& name, F1 my, F2 stl)
	{
		long long my_time = my();
		long long stl_time = stl();
		print_row(name, my_time, stl_time);
	}

	template<typename Func>
	long long benchmark(Func f)
	{
		auto start = Clock::now();
		f();
		auto end = Clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	template<typename VectorType>
	long long push_back(size_t n)
	{
		VectorType Vector;

		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					Vector.push_back(i);
				}
			});
	}

	template<typename VectorType>
	long long pop_back(size_t n)
	{
		VectorType Vector;
		for (size_t i = 0; i < n; ++i)
		{
			Vector.push_back(i);
		}

		return benchmark([&]()
			{
				while (!Vector.empty())
				{
					Vector.pop_back();
				}
			});
	}

	template<typename VectorType>
	long long insert_middle(size_t n)
	{
		VectorType Vector;
		for (size_t i = 0; i < n; ++i)
		{
			Vector.push_back(i);
		}

		auto it = Vector.begin();

		for (size_t i = 0; i < n / 2; ++i)
		{
			++it;
		}

		return benchmark([&]
		{
			for (size_t i = 0; i < 1000; ++i)
			{
				Vector.insert(Vector.begin() + Vector.size() / 2, -1);
				Vector.erase(Vector.begin() + Vector.size() / 2);
			}
		});
	}

	template<typename VectorType>
	long long insert_init_list(size_t n)
	{
		VectorType Vector;
		for (size_t i = 0; i < n; ++i)
		{
			Vector.push_back(i);
		}
		auto it = Vector.begin();
		for (size_t i = 0; i < n / 2; ++i)
		{
			++it;
		}
		return benchmark([&]()
			{
				Vector.insert(it, { -1, -2, -3 });
			});
	}

	template<typename VectorType>
	long long erase_range_list(size_t n)
	{
		VectorType Vector;
		for (size_t i = 0; i < n; ++i)
		{
			Vector.push_back(i);
		}
		auto it = Vector.begin();
		for (size_t i = 0; i < n / 2; ++i)
		{
			++it;
		}
		return benchmark([&]()
			{
				Vector.erase(it, it + 3);
			});
	}

	template<typename VectorType>
	long long erase_middle(size_t n)
	{
		VectorType Vector;
		for (size_t i = 0; i < n; ++i)
		{
			Vector.push_back(i);
		}

		auto it = Vector.begin();

		for (size_t i = 0; i < n / 2; ++i)
		{
			++it;
		}

		return benchmark([&]()
			{
				Vector.erase(it);
			});
	}

	template<typename VectorType>
	long long clear(size_t n)
	{
		VectorType Vector;
		for (size_t i = 0; i < n; ++i)
			Vector.push_back(i);

		return benchmark([&]()
			{
				Vector.clear();
			});
	}

	template<typename VectorType>
	long long reserve(size_t n)
	{
		VectorType Vector;

		return benchmark([&]()
			{
				Vector.reserve(n);
			});
	}

	template<typename VectorType>
	long long resize(size_t n)
	{
		VectorType Vector;

		return benchmark([&]()
			{
				Vector.resize(n);
			});
	}

	template<typename VectorType>
	long long shrink_to_fit(size_t n)
	{
		VectorType Vector;

		Vector.reserve(n * 2);
		Vector.resize(n);

		return benchmark([&]()
			{
				Vector.shrink_to_fit();
			});
	}

	template<typename VectorType>
	long long assign(size_t n)
	{
		VectorType Vector;

		return benchmark([&]()
			{
				Vector.assign(n, 42);
			});
	}

	template<typename VectorType>
	long long emplace_back(size_t n)
	{
		VectorType Vector;

		return benchmark([&]()
			{
				for (size_t i = 0; i < n; ++i)
				{
					Vector.emplace_back(i);
				}
			});
	}

	template<typename VectorType>
	long long copy_constructor(size_t n)
	{
		VectorType Vector;

		for (size_t i = 0; i < n; ++i)
		{
			Vector.push_back(i);
		}

		return benchmark([&]()
			{
				VectorType copy(Vector);
			});
	}

	template<typename VectorType>
	long long move_constructor(size_t n)
	{
		return benchmark([&]()
			{
				VectorType Vector;

				for (size_t i = 0; i < n; ++i)
				{
					Vector.push_back(i);
				}

				VectorType moved(std::move(Vector));
			});
	} 

	template<typename VectorType>
	long long copy_assignment(size_t n)
	{
		VectorType source;

		for (size_t i = 0; i < n; ++i)
		{
			source.push_back(i);
		}

		return benchmark([&]()
			{
				VectorType dest;
				dest = source;
			});
	}

	template<typename VectorType>
	long long move_assignment(size_t n)
	{
		return benchmark([&]()
			{
				VectorType source;

				for (size_t i = 0; i < n; ++i)
				{
					source.push_back(i);
				}

				VectorType dest;
				dest = std::move(source);
			});
	}

	template<typename VectorType>
	long long swap(size_t n)
	{
		VectorType first;
		VectorType second;

		for (size_t i = 0; i < n; ++i)
		{
			first.push_back(i);
			second.push_back(i + n);
		}

		return benchmark([&]()
			{
				first.swap(second);
			});
	}

	void print(const string& name, long long my, long long stl)
	{
		cout << name << ":\n";
		cout << "  MyVector = " << my << " micro s\n";
		cout << "  std::vector= " << stl << " micro s\n\n";
	}
};