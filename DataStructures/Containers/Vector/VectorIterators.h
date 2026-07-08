#pragma once
#include <stdexcept>
template<typename T>
class Vector;

/// <summary>
/// Базовый класс итератора для вектора.
/// </summary>
/// <typeparam name="Pointer"> Тип указателя </typeparam>
/// <typeparam name="Reference"> Тип ссылки </typeparam>
/// <typeparam name="Derived"> Тип производного класса </typeparam>
template<typename Pointer, typename Reference, typename Derived, typename T>
class base_iterator
{
	friend class Vector<T>;
protected:
	Pointer ptr;

public:
	explicit base_iterator(Pointer ptr = nullptr) : ptr(ptr) {}

	/// <summary>
	/// Разыменование итератора.
	/// </summary>
	/// <returns>Значение, на которое указывает итератор</returns>
	Reference operator*() const
	{
		if (!ptr)
			throw std::runtime_error("Dereferencing null iterator");
		return *ptr;
	}
	/// <summary>
	/// Получение указателя на элемент, на который указывает итератор.
	/// </summary>
	/// <returns>Указатель на элемент, на который указывает итератор</returns>
	Pointer operator->() const
	{
		if (!ptr)
			throw std::runtime_error("Dereferencing null iterator");
		return ptr;
	}
	/// <summary>
	/// Сравнение итераторов.
	/// </summary>
	/// <param name="other">Итератор для сравнения</param>
	/// <returns>True, если итераторы равны, иначе False</returns>
	bool operator==(const Derived& other) const
	{
		return ptr == other.ptr;
	}
	/// <summary>
	/// Сравнение итераторов.
	/// </summary>
	/// <param name="other">Итератор для сравнения</param>
	/// <returns>True, если итераторы не равны, иначе False</returns>
	bool operator!=(const Derived& other) const
	{
		return ptr != other.ptr;
	}
	/// <summary>
	/// Сложение итератора с целым числом.
	/// </summary>
	/// <param name="i">Целое число</param>
	/// <returns>Итератор, смещенный на i позиций</returns>
	Derived& operator+(int i)
	{
		if (ptr)
		{
			for (int j = 0; j < i; j++)
			{
				++ptr;
			}
		}

		return static_cast<Derived&>(*this);
	}
	/// <summary>
	/// Префиксный инкремент итератора.
	/// </summary>
	/// <returns>Итератор, смещенный на одну позицию</returns>
	Derived& operator++()
	{
		if (ptr) ++ptr;
		return static_cast<Derived&>(*this);
	}
	/// <summary>
	/// Постфиксный инкремент итератора.
	/// </summary>
	/// <param name=""></param>
	/// <returns>Итератор, смещенный на одну позицию</returns>
	Derived operator++(int)
	{
		Derived temp = static_cast<Derived&>(*this);
		++(*this);
		return temp;
	}
	/// <summary>
	/// Префиксный декремент итератора.
	/// </summary>
	/// <returns>Итератор, смещенный на одну позицию</returns>
	Derived& operator--()
	{
		if (ptr) --ptr;
		return static_cast<Derived&>(*this);
	}
	/// <summary>
	/// Постфиксный декремент итератора.
	/// </summary>
	/// <param name=""></param>
	/// <returns>Итератор, смещенный на одну позицию</returns>
	Derived operator--(int)
	{
		Derived temp = static_cast<Derived&>(*this);
		--(*this);
		return temp;
	}
};
/// <summary>
/// Класс iterator представляет собой итератор для вектора.
/// </summary>
template <typename T>
class VectorIterator : public base_iterator<T*, T&, VectorIterator<T>, T>
{
public:
	using base_iterator<T*, T&, VectorIterator<T>, T>::base_iterator;
};
/// <summary>
/// Класс const_iterator представляет собой константный итератор для вектора.
/// </summary>
template <typename T>
class VectorConstIterator : public base_iterator<const T*, const T&, VectorConstIterator<T>, T>
{
public:
	using base_iterator<const T*, const T&, VectorConstIterator<T>, T>::base_iterator;

	VectorConstIterator(const VectorIterator<T>& it) : base_iterator<const T*, const T&, VectorConstIterator<T>, T>(it.ptr) {}
};