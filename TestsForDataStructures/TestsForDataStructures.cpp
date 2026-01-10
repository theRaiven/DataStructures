#include "pch.h"
#include "CppUnitTest.h"
#include "../DataStructures/List.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestsForDataStructures
{
	TEST_CLASS(TestsForDataStructures)
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
}
