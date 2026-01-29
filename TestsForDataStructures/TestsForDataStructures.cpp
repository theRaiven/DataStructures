#include "pch.h"
#include "CppUnitTest.h"
#include "../DataStructures/List.h"
#include "../DataStructures/RBTree.h"
#include <random>
#include <set>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestsForDataStructures
{
	TEST_CLASS(TestsForList)
	{
	public:
		TEST_METHOD(Sort)
		{
		    List<int> myList = { 4, 2, 5, 1, 3 };
			myList.sort();
			int expected[] = { 1, 2, 3, 4, 5 };
			int index = 0;
			for (const auto& item : myList)
			{
				Assert::AreEqual(expected[index], item);
				index++;
			}
		}
		TEST_METHOD(Sort_EmptyList)
		{
			List<int> myList;
			myList.sort();
			Assert::AreEqual(static_cast<size_t>(0), myList.size());
		}
		TEST_METHOD(Sort_SingleElement)
		{
			List<int> myList = { 42 };
			myList.sort();
			Assert::AreEqual(static_cast<size_t>(1), myList.size());
			for (const auto& item : myList)
			{
				Assert::AreEqual(42, item);
			}
		}
		TEST_METHOD(Sort_AlreadySorted)
		{
			List<int> myList = { 1, 2, 3, 4, 5 };
			myList.sort();
			int expected[] = { 1, 2, 3, 4, 5 };
			int index = 0;
			for (const auto& item : myList)
			{
				Assert::AreEqual(expected[index], item);
				index++;
			}
		}
		TEST_METHOD(Sort_WithDuplicates)
		{
			List<int> myList = { 3, 1, 2, 3, 2 };
			myList.sort();
			int expected[] = { 1, 2, 2, 3, 3 };
			int index = 0;
			for (auto& v : myList)
				Assert::AreEqual(expected[index++], v);
		}
		TEST_METHOD(Swap_ToList)
		{
			List<int> list1;
			list1.push_back(2);
			list1.push_back(3);
			list1.push_front(1);
			List<int> list2 = { 4, 5 };
			list1.swap(list2);
			int expected1[] = { 4, 5 };
			int expected2[] = { 1, 2, 3 };
			int index = 0;
			for (const auto& item : list1)
			{
				Assert::AreEqual(expected1[index], item);
				index++;
			}
			index = 0;
			for (const auto& item : list2)
			{
				Assert::AreEqual(expected2[index], item);
				index++;
			}
			Assert::AreEqual(static_cast<size_t>(2), list1.size());
			Assert::AreEqual(static_cast<size_t>(3), list2.size());
		}
		TEST_METHOD(PopBack)
		{
			List<int> list = { 1, 2, 3 };
			list.pop_back();

			int expected[] = { 1, 2 };
			int i = 0;
			for (auto x : list)
				Assert::AreEqual(expected[i++], x);
		}
		TEST_METHOD(PopBack_Single)
		{
			List<int> list = { 10 };
			list.pop_back();

			Assert::AreEqual(static_cast<size_t>(0), list.size());
			Assert::AreEqual(true, list.empty());
		}
		TEST_METHOD(PopFront)
		{
			List<int> list = { 1, 2, 3 };
			list.pop_front();

			int expected[] = { 2, 3 };
			int i = 0;
			for (auto x : list)
				Assert::AreEqual(expected[i++], x);
		}
		TEST_METHOD(PopFront_Single)
		{
			List<int> list = { 10 };
			list.pop_front();

			Assert::AreEqual(static_cast<size_t>(0), list.size());
			Assert::AreEqual(true, list.empty());
		}
		TEST_METHOD(FrontBack)
		{
			List<int> list = { 1, 2, 3 };

			Assert::AreEqual(1, list.front());
			Assert::AreEqual(3, list.back());
		}
		TEST_METHOD(FrontBack_Empty)
		{
			List<int> list;

			Assert::ExpectException<std::out_of_range>([&list]() { list.front(); });
		}
		TEST_METHOD(Erase_Middle)
		{
			List<int> list = { 1, 2, 3 };
			auto it = list.begin();
			++it;

			list.erase(it);

			int expected[] = { 1, 3 };
			int i = 0;
			for (auto x : list)
				Assert::AreEqual(expected[i++], x);
		}
		TEST_METHOD(Remove_Value)
		{
			List<int> list = { 1, 2, 2, 3 };
			list.remove(2);

			int expected[] = { 1, 3 };
			int i = 0;
			for (auto x : list)
				Assert::AreEqual(expected[i++], x);
		}
		TEST_METHOD(Remove_NotFound)
		{
			List<int> list = { 1, 2, 3 };
			list.remove(5);

			Assert::AreEqual(static_cast<size_t>(3), list.size());
		}
		TEST_METHOD(Unique)
		{
			List<int> list = { 1, 1, 2, 2, 2, 3 };
			list.unique();

			int expected[] = { 1, 2, 3 };
			int i = 0;
			for (auto x : list)
				Assert::AreEqual(expected[i++], x);
		}
		TEST_METHOD(Resize_Add)
		{
			List<int> list = { 1, 2 };
			list.resize(4, 9);

			int expected[] = { 1, 2, 9, 9 };
			int i = 0;
			for (auto x : list)
				Assert::AreEqual(expected[i++], x);
		}
		TEST_METHOD(Resize_Decrease)
		{
			List<int> list = { 1, 2, 3 };
			list.resize(2);

			int expected[] = { 1, 2 };
			int i = 0;
			for (auto x : list)
				Assert::AreEqual(expected[i++], x);
		}
		TEST_METHOD(Splice)
		{
			List<int> a = { 1, 4 };
			List<int> b = { 2, 3 };

			auto it = a.begin();
			++it;

			a.splice(it, b);

			int expected[] = { 1, 2, 3, 4 };
			int i = 0;
			for (auto x : a)
				Assert::AreEqual(expected[i++], x);

			Assert::AreEqual(static_cast<size_t>(0), b.size());
		}

	};
	TEST_CLASS(TestsForRBTree)
	{
	public:
		TEST_METHOD(ValidateEmptyRBTree)
		{
			RBTree<int> tree;

			Assert::AreEqual(static_cast<size_t>(0), tree.size());
			Assert::AreEqual(true, tree.empty());
			Assert::IsTrue(tree.begin() == tree.end());
			Assert::IsTrue(tree.find(10) == tree.end());
			Assert::IsTrue(tree.lower_bound(10) == tree.end());
			Assert::IsTrue(tree.upper_bound(10) == tree.end());
			Assert::AreEqual(tree.validate(), true);

		}
		TEST_METHOD(ValidateRBTreeWithOneElem)
		{
			RBTree<int> tree;
			tree.insert(5);

			Assert::AreEqual(static_cast<size_t>(1), tree.size());
			Assert::AreEqual(false, tree.empty());
			Assert::AreEqual(*tree.begin(), 5);
			Assert::IsTrue(++tree.begin() == tree.end());
			Assert::IsTrue(--tree.end() == tree.begin());
			Assert::IsTrue(tree.find(5) != tree.end());
			Assert::IsTrue(tree.find(4) == tree.end());
			Assert::AreEqual(tree.validate(), true);

		}
		TEST_METHOD(ValidateInsertionIsSequential)
		{
			RBTree<int> tree;
			for (int i = 1; i <= 1000; ++i)
			{
				tree.insert(i);
			}
			Assert::AreEqual(static_cast<size_t>(1000), tree.size());
			Assert::AreEqual(tree.validate(), true);

			int i=1;
			for (auto t : tree)
			{
				Assert::AreEqual(t, i++);
			}
		
		}
		TEST_METHOD(ValidateReverseInsertionIsSequential)
		{
			RBTree<int> tree;
			for (int i = 1000; i >= 1; --i)
			{
				tree.insert(i);
			}
			Assert::AreEqual(static_cast<size_t>(1000), tree.size());
			Assert::AreEqual(tree.validate(), true);

			int i = 1;
			for (auto t : tree)
			{
				Assert::AreEqual(t, i++);
			}
		}
		TEST_METHOD(ValidateRandomInsertionIsSequential)
		{
			RBTree<int> tree;
			std::mt19937 gen(42);
			for (int i = 0; i < 10'000; ++i)
			{
				tree.insert(gen());
			}
			Assert::AreEqual(static_cast<size_t>(10000), tree.size());
			Assert::AreEqual(tree.validate(), true);
		}
		TEST_METHOD(In_orderTraversal)
		{
			RBTree<int> tree;
			for (int i = 1000; i >= 1; --i)
			{
				tree.insert(i);
			}
			Assert::AreEqual(static_cast<size_t>(1000), tree.size());
			Assert::AreEqual(tree.validate(), true);
			std::vector<int> v;
			for (auto x : tree)
			{
				v.push_back(x);
			}
			Assert::AreEqual(true, std::is_sorted(v.begin(), v.end()));
			Assert::AreEqual(v.size(), tree.size());
		}
		TEST_METHOD(CorrectIteratorTravel)
		{
			RBTree<int> tree;
			for (int i = 1000; i >= 1; --i)
			{
				tree.insert(i);
			}
			auto it = tree.find(10);
			auto next = it;
			++next;
			tree.erase(it);
			Assert::AreEqual(*tree.find(*next), *next);
			
			it = tree.begin();
			auto prev = it++;
			Assert::AreEqual(*prev, 1);
			it = tree.end();
			--it;
			Assert::AreEqual(*it, 1000);
		}
		TEST_METHOD(EraseSheet)
		{
			RBTree<int> tree;

			tree.insert(10);
			tree.insert(9);
			tree.insert(8);

			Assert::AreEqual(tree.validate(), true);

			tree.erase(8);
			
			Assert::AreEqual(tree.validate(), true);
		}
		TEST_METHOD(EraseNodeWithOneChild)
		{
			RBTree<int> tree;

			for (int i = 10; i > 0; i--)
			{
				tree.insert(i);
			}
			Assert::AreEqual(tree.validate(), true);

			tree.erase(2);

			Assert::AreEqual(tree.validate(), true);
		}
		TEST_METHOD(EraseNodeWithTwoChild)
		{
			RBTree<int> tree;

			for (int i = 10; i > 0; i--)
			{
				tree.insert(i);
			}
			Assert::AreEqual(tree.validate(), true);

			tree.erase(5);

			Assert::AreEqual(tree.validate(), true);
		}
		TEST_METHOD(EraseFirstNode)
		{
			RBTree<int> tree;

			for (int i = 10; i > 0; i--)
			{
				tree.insert(i);
			}
			tree.erase(*tree.begin());

			Assert::AreEqual(tree.validate(), true);
		}
		TEST_METHOD(EraseList)
		{
			RBTree<int> tree;

			for (int i = 10; i > 0; i--)
			{
				tree.insert(i);
			}
			std::vector<int> keys{ 1,2,3,4,5 };

			for (int k : keys)
			{
				tree.erase(k);
				Assert::AreEqual(tree.validate(), true);
			}
		}
		TEST_METHOD(EraseAll)
		{
			RBTree<int> tree;
			for (int i = 1; i <= 1000; ++i) tree.insert(i);
			for (int i = 1; i <= 1000; ++i)
			{
				tree.erase(i);
				Assert::IsTrue(tree.validate());
			}
			Assert::IsTrue(tree.empty());
		}
		TEST_METHOD(RandomInsertRandomErase)
		{
			RBTree<int> tree;
			std::mt19937 gen(42);
			std::vector<int> keys;

			for (int i = 0; i < 5000; ++i)
			{
				int v = gen();
				keys.push_back(v);
				tree.insert(v);
			}
			std::shuffle(keys.begin(), keys.end(), gen);

			for (int v : keys)
			{
				tree.erase(v);
				Assert::IsTrue(tree.validate());
			}
		}
		TEST_METHOD(LowerUpperEqual)
		{
			RBTree<int> tree;

			for (int i = 1000; i > 0; i--)
			{
				tree.insert(i);
			}
			Assert::AreEqual(*tree.upper_bound(10), 11);
			Assert::AreEqual(tree.lower_bound(99).operator*(), 99);
			Assert::IsTrue(tree.upper_bound(1000) == tree.end());
		
			auto range = tree.equal_range(10);

			Assert::IsTrue(tree.lower_bound(10) == range.first);
			Assert::IsTrue(tree.upper_bound(10) == range.second);
		}
		TEST_METHOD(CopyTree)
		{
			RBTree<int> tree;

			for (int i = 1000; i > 0; i--)
			{
				tree.insert(i);
			}
		
			auto copy_tree = tree;

			Assert::AreEqual(copy_tree.size(), tree.size());
			auto it_copy = copy_tree.begin();
			for (auto it_original : tree)
			{
				Assert::IsTrue(*it_copy == it_original);
				++it_copy;
			}
		}
		TEST_METHOD(MoveTree)
		{
			RBTree<int> tree;

			for (int i = 1000; i > 0; i--)
			{
				tree.insert(i);
			}
			RBTree<int> a = std::move(tree);
			
			Assert::AreEqual(tree.empty(), true);
			Assert::AreEqual(a.validate(), true);
		}
		TEST_METHOD(SelfCopy)
		{
			RBTree<int> tree;

			for (int i = 1000; i > 0; i--)
			{
				tree.insert(i);
			}
			 tree = tree;

			Assert::AreEqual(tree.validate(), true);
		}
		TEST_METHOD(SetComparison)
		{
			RBTree<int> my;
			std::set<int> st;

			for (int i = 1000; i > 0; i--)
			{
				my.insert(i);
				st.insert(i);
			}

			Assert::AreEqual(st.size(), my.size());
			auto it_st = st.begin();
			for (auto it_my : my)
			{
				Assert::IsTrue(*it_st == it_my);
				++it_st;
			}
		}
	};
}
