#pragma once
#include <stdexcept>
#include "ListNode.h"

template<typename T>
class List;

/// <summary>
/// Двунаправленный итератор для контейнера List.
/// Позволяет последовательно перемещаться по элементам списка
/// и получать доступ к хранимым данным.
/// </summary>
template<typename T>
class ListIterator
{
	friend class List<T>;
private:
	NodeList<T>* ptr;
public:

	ListIterator(NodeList<T>* ptr = nullptr) : ptr(ptr) {}
	/// <summary>
	/// Возвращает ссылку на элемент, на который указывает итератор.
	/// </summary>
	/// <returns>Ссылка на элемент, на который указывает итератор.</returns>
	T& operator*()
	{
		if (!ptr) throw std::runtime_error("Dereferencing null iterator");
		return ptr->data;
	}
	/// <summary>
	/// Возвращает константную ссылку на элемент, на который указывает итератор.
	/// </summary>
	/// <returns>Константная ссылка на элемент, на который указывает итератор.</returns>
	const T& operator*() const
	{
		if (!ptr) throw std::runtime_error("Dereferencing null iterator");
		return ptr->data;
	}
	/// <summary>
	/// Проверяет, равны ли два итератора.
	/// </summary>
	/// <param name="other">Итератор, с которым сравнивается текущий итератор.</param>
	/// <returns>True, если итераторы равны, иначе False.</returns>
	bool operator==(const ListIterator<T>& other) const
	{
		return ptr == other.ptr;
	}
	/// <summary>
	/// Перемещает итератор на следующий элемент.
	/// </summary>
	/// <returns>Ссылка на измененный итератор.</returns>
	ListIterator& operator++()
	{
		if (ptr) ptr = ptr->next;
		return *this;
	}
	/// <summary>
	/// Перемещает итератор на следующий элемент.
	/// </summary>
	/// <param name=""></param>
	/// <returns>Ссылка на измененный итератор.</returns>
	ListIterator operator++(int)
	{
		ListIterator temp = *this;
		++(*this);
		return temp;
	}
	/// <summary>
	/// Перемещает итератор на предыдущий элемент.
	/// </summary>
	/// <returns>Ссылка на измененный итератор.</returns>
	ListIterator& operator--()
	{
		if (ptr) ptr = ptr->prev;
		return *this;
	}
	/// <summary>
	/// Перемещает итератор на предыдущий элемент.
	/// </summary>
	/// <param name=""></param>
	/// <returns>Ссылка на измененный итератор.</returns>
	ListIterator operator--(int)
	{
		ListIterator temp = *this;
		--(*this);
		return temp;
	}
	/// <summary>
	/// Проверяет, не равны ли два итератора.
	/// </summary>
	/// <param name="other">Итератор, с которым сравнивается текущий итератор.</param>
	/// <returns>True, если итераторы не равны, иначе False.</returns>
	bool operator!=(const ListIterator<T>& other) const
	{
		return ptr != other.ptr;
	}

};