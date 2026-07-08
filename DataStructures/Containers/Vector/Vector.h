#pragma once
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <type_traits>
#include <memory>
#include "VectorIterators.h"

/// <summary>
/// Класс Vector представляет собой динамический массив элементов типа T.
/// </summary>
/// <typeparam name="T">Тип элементов вектора</typeparam>
template <typename T>
class Vector
{
private:
	using AllocTraits = std::allocator_traits<std::allocator<T>>;
	std::allocator<T> v_allocator;

	T* v_data;
	size_t v_size;
	size_t v_capacity;

	using iterator = VectorIterator<T>;
	using const_iterator = VectorConstIterator<T>;

public: 
	// =========================
	// конструкторы и деструктор
	// =========================

	/// <summary>
	/// Конструктор по умолчанию. Инициализирует пустой вектор: устанавливает data в nullptr, vector_size и vector_capacity в 0.
	/// </summary>
	constexpr Vector() noexcept : v_data(nullptr), v_size(0), v_capacity(0) {}
	/// <summary>
	/// Конструктор, инициализирующий вектор заданным количеством элементов.
	/// </summary>
	/// <param name="count">Количество элементов в векторе</param>
	explicit Vector(size_t count) : v_data(v_allocator.allocate(count)), v_size(count), v_capacity(count)
	{
		for(size_t i = 0; i < count; i++)
		{
			AllocTraits::construct(v_allocator ,&v_data[i], T());
		}
	}
	/// <summary>
	/// Конструктор, инициализирующий вектор заданным количеством элементов и значением.
	/// </summary>
	/// <param name="count">Количество элементов в векторе</param>
	/// <param name="value">Значение для инициализации элементов</param>
	explicit Vector(size_t count, const T& value) : v_data(v_allocator.allocate(count)), v_size(count), v_capacity(count)
	{
		for (size_t i = 0; i < count; i++)
		{
			AllocTraits::construct(v_allocator, &v_data[i], value);
		}
	}
	/// <summary>
	/// Конструктор, инициализирующий вектор с помощью списка инициализации.
	/// </summary>
	/// <param name="init_lis">Список инициализации</param>
	Vector(std::initializer_list<T> init_lis) : v_data(v_allocator.allocate(init_lis.size())), v_size(init_lis.size()), v_capacity(init_lis.size())
	{
		size_t i = 0;
		for (auto& item : init_lis)
		{
			AllocTraits::construct(v_allocator, &v_data[i++], item);
		}
	}
	/// <summary>
	/// Конструктор копирования.
	/// </summary>
	/// <param name="other">Другой вектор, из которого происходит копирование</param>
	Vector(const Vector& other) : v_data(v_allocator.allocate(other.v_capacity)), v_size(other.v_size), v_capacity(other.v_capacity)
	{
		size_t costructed = 0;
		try
		{
			for (; costructed < other.v_size; costructed++)
			{
				AllocTraits::construct(v_allocator, &v_data[costructed], other.v_data[costructed]);
			}
		}
		catch (...)
		{
			while (costructed > 0)
			{
				--costructed;
				AllocTraits::destroy(v_allocator, &v_data[costructed]);
			}
			deallocate_data();
			throw;
		}
	}
	/// <summary>
	/// Конструктор перемещения.
	/// </summary>
	/// <param name="other">Другой вектор, из которого происходит перемещение</param>
	constexpr Vector(Vector&& other) noexcept
	{
		v_allocator = std::move(other.v_allocator);

		v_data = std::exchange(other.v_data, nullptr);
		v_size = std::exchange(other.v_size, 0);
		v_capacity = std::exchange(other.v_capacity, 0);
	}
	/// <summary>
	/// Конструктор, инициализирующий вектор диапазоном итераторов.
	/// </summary>
	/// <typeparam name="InputIt">Тип итератора</typeparam>
	/// <param name="first">Итератор на первый элемент диапазона</param>
	/// <param name="last">Итератор на последний элемент диапазона</param>
	template <typename InputIt,
		std::enable_if_t<!std::is_integral_v<InputIt>, int> = 0>
	Vector(InputIt first, InputIt last)
	{
		size_t count = distance(first, last);

		v_data = v_allocator.allocate(count);
		v_size = 0;
		v_capacity = count;

		for (auto it = first; it != last; ++it)
		{
			AllocTraits::construct(v_allocator, &v_data[v_size++], *it);
		}
	}

private:
	template <typename InputIt,
		std::enable_if_t<!std::is_integral_v<InputIt>, int> = 0>
	int distance(InputIt first, InputIt last)
	{
		int count = 0;
		for (auto it = first; it != last; ++it)
		{
			++count;
		}
		return count;
	}

	void destroy_elements()
	{
		destroy_elements(v_size, v_data);
	}
	void destroy_elements(size_t count)
	{
		destroy_elements(count, v_data);
	}
	void destroy_elements(size_t count, T* vec)
	{
		for (size_t i = count; i > 0; --i)
		{
			AllocTraits::destroy(v_allocator, &vec[i - 1]);
		}
	}
	void destroy_range(size_t first, size_t last)
	{
		for (size_t i = first; i < last; ++i)
		{
			AllocTraits::destroy(v_allocator, &v_data[i]);
		}
	}
	
	void deallocate_data()
	{
		deallocate_data(v_data, v_capacity);
	}
	void deallocate_data(T* data, size_t capacity)
	{
		if (data)
		{
			v_allocator.deallocate(data, capacity);
			data = nullptr;
		}
	}
public:
	/// <summary>
	/// Освобождает динамически выделенную память, занимаемую элементами вектора.
	/// </summary>
	~Vector() noexcept
	{
		destroy_elements();
		deallocate_data();

		v_size = 0;
		v_capacity = 0;
	}

	// ==================================
	// операторы присваивания и сравнения
	// ==================================

	/// <summary>
	/// Оператор присваивания.
	/// </summary>
	/// <param name="other">Другой вектор, из которого происходит присваивание</param>
	/// <returns> Ссылка на текущий объект Vector.</returns>
	Vector& operator=(const Vector& other)
	{
		Vector temp(other);
		swap(temp);
		return *this;
	}
	/// <summary>
	/// Оператор присваивания перемещения.
	/// </summary>
	/// <param name="other">Другой вектор, из которого происходит присваивание</param>
	/// <returns> Ссылка на текущий объект Vector.</returns>
	Vector& operator=(Vector&& other) noexcept
	{
		if (this != &other)
		{
			destroy_elements();
			deallocate_data();

			v_allocator = std::move(other.v_allocator);
			v_data = std::exchange(other.v_data, nullptr);
			v_size = std::exchange(other.v_size, 0);
			v_capacity = std::exchange(other.v_capacity, 0);
		}
		return *this;
	}
	/// <summary>
	/// Оператор присваивания списка инициализации.
	/// </summary>
	/// <param name="init_list">Список инициализации</param>
	/// <returns> Ссылка на текущий объект Vector.</returns>
	Vector& operator=(std::initializer_list<T> init_list)
	{
		destroy_elements();
		deallocate_data();

		v_data = v_allocator.allocate(init_list.size());
		v_size = 0;
		v_capacity = init_list.size();

		for (const auto& item : init_list)
		{
			AllocTraits::construct(v_allocator, &v_data[v_size++], item);
		}
		return *this;
	}
	/// <summary>
	/// Сравнивает текущий вектор с другим по размеру и содержимому.
	/// </summary>
	/// <param name="other">Другой вектор, с которым происходит сравнение</param>
	/// <returns>True, если векторы равны, иначе False</returns>
	bool operator==(const Vector& other) const
	{
		if (v_size != other.v_size)
		{
			return false;
		}

		for (size_t i = 0; i < v_size; i++)
		{
			if (v_data[i] != other.v_data[i]) return false;
		}
		return true;
	}
	/// <summary>
	/// Сравнивает текущий вектор с другим на неравенство.
	/// </summary>
	/// <param name="other">Другой вектор, с которым происходит сравнение</param>
	/// <returns>True, если векторы не равны, иначе False</returns>
	bool operator!=(const Vector& other) const
	{
		return !(*this == other);
	}

	/// <summary>
	/// Оператор доступа по индексу.
	/// </summary>
	/// <param name="index">Индекс элемента</param>
	/// <returns>Ссылка на элемент вектора</returns>
	T& operator[](size_t index)
	{
		return v_data[index];
	}
	/// <summary>
	/// Оператор доступа по индексу (константная версия).
	/// </summary>
	/// <param name="index">Индекс элемента</param>
	/// <returns>Ссылка на элемент вектора</returns>
	const T& operator[](size_t index) const
	{
		return v_data[index];
	}

	// =========
	// итераторы 
	// =========

	/// <summary>
	/// Возвращает итератор на первый элемент вектора.
	/// </summary>
	/// <returns>Итератор на первый элемент вектора</returns>
	iterator begin() noexcept
	{
		return iterator(v_data);
	}
	/// <summary>
	/// Возвращает итератор на последний элемент вектора.
	/// </summary>
	/// <returns>Итератор на последний элемент вектора</returns>
	iterator end() noexcept
	{
		return iterator(v_data + v_size);
	}
	/// <summary>
	/// Возвращает константный итератор на первый элемент вектора.
	/// </summary>
	/// <returns>Константный итератор на первый элемент вектора</returns>
	const_iterator begin() const noexcept
	{
		return const_iterator(v_data);
	}
	/// <summary>
	/// Возвращает константный итератор на последний элемент вектора.
	/// </summary>
	/// <returns>Константный итератор на последний элемент вектора</returns>
	const_iterator end() const noexcept
	{
		return const_iterator(v_data + v_size);
	}

	// ==================
	// доступ к элементам
	// ==================

	/// <summary>
	/// Возвращает ссылку на элемент вектора по индексу.
	/// </summary>
	/// <param name="index">Индекс элемента</param>
	/// <returns>Ссылка на элемент вектора</returns>
	T& at(size_t index)
	{
		if (index >= v_size)
		{
			throw std::out_of_range("Index out of range");
		}
		return v_data[index];
	}
	/// <summary>
	/// Возвращает константную ссылку на элемент вектора по индексу.
	/// </summary>
	/// <param name="index">Индекс элемента</param>
	/// <returns>Константная ссылка на элемент вектора</returns>
	const T& at(size_t index) const
	{
		if (index >= v_size)
		{
			throw std::out_of_range("Index out of range");
		}
		return v_data[index];
	}
	/// <summary>
	/// Возвращает ссылку на первый элемент вектора.
	/// </summary>
	/// <returns>Ссылка на первый элемент вектора</returns>
	T& front()
	{
		if (v_size == 0)
		{
			throw std::out_of_range("Vector is empty");
		}
		return v_data[0];
	}
	/// <summary>
	/// Возвращает константную ссылку на первый элемент вектора.
	/// </summary>
	/// <returns>Константная ссылка на первый элемент вектора</returns>
	const T& front() const
	{
		if (v_size == 0)
		{
			throw std::out_of_range("Vector is empty");
		}
		return v_data[0];
	}
	/// <summary>
	/// Возвращает ссылку на последний элемент вектора.
	/// </summary>
	/// <returns>Ссылка на последний элемент вектора</returns>
	T& back()
	{
		if (v_size == 0)
		{
			throw std::out_of_range("Vector is empty");
		}
		return v_data[v_size - 1];
	}
	/// <summary>
	/// Возвращает константную ссылку на последний элемент вектора.
	/// </summary>
	/// <returns>Константная ссылка на последний элемент вектора</returns>
	const T& back() const
	{
		if (v_size == 0)
		{
			throw std::out_of_range("Vector is empty");
		}
		return v_data[v_size - 1];
	}
	/// <summary>
	/// Возвращает указатель на данные вектора.
	/// </summary>
	/// <returns>Указатель на данные вектора</returns>
	constexpr T* data() noexcept
	{
		return v_data;
	}
	/// <summary>
	/// Возвращает константный указатель на данные вектора.
	/// </summary>
	/// <returns>Константный указатель на данные вектора</returns>
	constexpr const T* data() const noexcept
	{
		return v_data;
	}

	// ====================
	// информация о векторе 
	// ====================
	
	/// <summary>
	/// Проверяет, является ли вектор пустым.
	/// </summary>
	/// <returns>true, если вектор пуст, иначе false</returns>
	constexpr bool empty() const noexcept
	{
		return v_size == 0;
	}
	/// <summary>
	/// Возвращает количество элементов в векторе.
	/// </summary>
	/// <returns>Количество элементов в векторе</returns>
	constexpr size_t size() const noexcept
	{
		return v_size;
	}
	/// <summary>
	/// Возвращает вместимость вектора.
	/// </summary>
	/// <returns>Вместимость вектора</returns>
	constexpr size_t capacity() const noexcept
	{
		return v_capacity;
	}

	// ==================
	// управление памятью
	// ==================
	
private:
	void replace_storage(T* new_data, size_t new_capacity)
	{
		size_t constructed = 0;
		try
		{
			for (constructed = 0; constructed < v_size; ++constructed)
			{
				AllocTraits::construct(v_allocator, &new_data[constructed], std::move_if_noexcept(v_data[constructed]));
			}
		}
		catch (...)
		{
			while (constructed > 0)
			{
				--constructed;
				AllocTraits::destroy(v_allocator, &new_data[constructed]);
			}
			deallocate_data(new_data, new_capacity);
			throw;
		}
		destroy_elements();
		deallocate_data();

		v_data = new_data;
		v_capacity = new_capacity;
	}

public: 

	/// <summary>
	/// Резервирует место для элементов в векторе.
	/// </summary>
	/// <param name="new_capacity">Новая вместимость вектора</param>
	void reserve(size_t new_capacity)
	{
		if (new_capacity > v_capacity)
		{
			T* new_data = v_allocator.allocate(new_capacity);
			
			replace_storage(new_data, new_capacity);
		}
	}
	/// <summary>
	/// Изменяет количество элементов в векторе.
	/// </summary>
	/// <param name="new_size">Новое количество элементов</param>
	void resize(size_t new_size)
	{
		if (new_size > v_capacity)
		{
			reserve(new_size);
		}
		if (new_size > v_size)
		{
			for (size_t i = v_size; i < new_size; ++i)
			{
				AllocTraits::construct(v_allocator, &v_data[i], T());
			}
			v_size = new_size;
		}
		else
		{
			for (size_t i = new_size; i < v_size; ++i)
			{
				AllocTraits::destroy(v_allocator, &v_data[i]);
			}
			v_size = new_size;
		}
	}
	/// <summary>
	/// Изменяет количество элементов в векторе, заполняя новые элементы заданным значением.
	/// </summary>
	/// <param name="new_size">Новое количество элементов</param>
	/// <param name="value">Значение для новых элементов</param>
	void resize(size_t new_size, const T& value)
	{
		if (new_size > v_capacity)
		{
			reserve(new_size);
		}
		if (new_size > v_size)
		{
			for (size_t i = v_size; i < new_size; i++)
			{
				AllocTraits::construct(v_allocator, &v_data[i], value);
			}
			v_size = new_size;
		}
		else
		{
			for (size_t i = new_size; i < v_size; i++)
			{
				AllocTraits::destroy(v_allocator, &v_data[i]);
			}
			v_size = new_size;
		}
	}
	/// <summary>
	/// Очищает вектор
	/// </summary>
	void clear()
	{
		destroy_elements();
		v_size = 0;
	}
	/// <summary>
	/// Изменяет вместимость вектора на его текущий размер
	/// </summary>
	void shrink_to_fit()
	{
		if (v_size == v_capacity)
		{
			return;
		}

		if (v_size == 0)
		{
			v_allocator.deallocate(v_data, v_capacity);
			v_data = nullptr;
			v_capacity = 0;
			return;
		}

		T* new_data = v_allocator.allocate(v_size);

		replace_storage(new_data, v_size);
	}

	// =============================
	// изменение содержимого вектора
	// =============================
private:
	void ensure_capacity()
	{
		if (v_size == v_capacity)
		{
			if (v_capacity == 0)
			{
				reserve(1);
			}
			else
			{
				reserve(v_capacity * 2);
			}
		}
	}

	void check_iterator(iterator pos)
	{
		if (!v_data)
		{
			if (pos.ptr != nullptr)
			{
				throw std::out_of_range("Invalid iterator");
			}
			return;
		}
		if (pos.ptr < v_data || pos.ptr > v_data + v_size)
		{
			throw std::out_of_range("Invalid iterator");
		}
	}
public:
	/// <summary>
	/// Добавляет элемент в конец вектора.
	/// </summary>
	/// <param name="value">Добавляемый элемент</param>
	void push_back(const T& value)
	{
		ensure_capacity();

		AllocTraits::construct(v_allocator, &v_data[v_size], value); // move не нужен, так как value - это const ссылка, и все равно будет lvalue
		v_size++;
	}
	/// <summary>
	/// Добавляет элемент в конец вектора, используя перемещение.
	/// </summary>
	/// <param name="value">Добавляемый элемент</param>
	void push_back(T&& value)
	{
		ensure_capacity();
		AllocTraits::construct(v_allocator, &v_data[v_size], std::move(value)); // value - lvalue, а мы хотим переместить
		v_size++;
	}
	/// <summary>
	/// Конструирует элемент типа T непосредственно в конце вектора,
	/// используя переданные аргументы конструктора, и увеличивает размер вектора.
	/// </summary>
	/// <typeparam name="Args">
	/// Типы аргументов, передаваемых в конструктор объекта T.
	/// </typeparam>
	/// <param name="args">
	/// Аргументы, которые будут перенаправлены в конструктор объекта типа T.
	/// </param>
	template <typename... Args>
	void emplace_back(Args&&... args)
	{
		ensure_capacity();
		AllocTraits::construct(v_allocator, &v_data[v_size], std::forward<Args>(args)...); // буквально так, как внутри construct
		v_size++;
	}
	/// <summary>
	/// Удаляет последний элемент из вектора.
	/// </summary>
	void pop_back()
	{
		if (v_size == 0)
		{
			throw std::out_of_range("Vector is empty");
		}
		AllocTraits::destroy(v_allocator, &v_data[v_size - 1]);
		--v_size;
	}
	/// <summary>
	/// Вставляет элемент в вектор.
	/// </summary>
	/// <param name="pos">Итератор, указывающий на позицию для вставки</param>
	/// <param name="value">Вставляемый элемент</param>
	/// <returns>Итератор, указывающий на вставленный элемент</returns>
	iterator insert(iterator pos, const T& value)
	{
		// NOTE:
		// Текущая реализация корректна для большинства типов,
		// однако не обеспечивает strong exception guarantee,
		// если перемещение или копирование T генерирует исключение.
		
		// TODO:
		// 1. Обеспечить strong exception guarantee.
		// 2. Реализовать rollback при исключении во время сдвига элементов.
		// 3. Устранить дублирование логики между всеми перегрузками insert().

		check_iterator(pos);

		size_t index = pos.ptr - v_data;

		if (index == v_size)
		{
			push_back(value);
			return iterator(v_data + v_size - 1);
		}

		ensure_capacity();

		// задача из TODO 1:

		for (size_t i = v_size; i > index; i--)
		{
			AllocTraits::construct(v_allocator, &v_data[i], std::move(v_data[i - 1]));
			AllocTraits::destroy(v_allocator, &v_data[i - 1]);
		}
		AllocTraits::construct(v_allocator, &v_data[index], value);
		v_size++;

		return iterator(v_data + index);
	}
	/// <summary>
	/// Вставляет count копий элемента value перед позицией pos.
	/// </summary>
	/// <param name="pos">Позиция для вставки</param>
	/// <param name="count">Количество вставляемых элементов</param>
	/// <param name="value">Значение вставляемых элементов</param>
	/// <returns>Итератор на первый вставленный элемент</returns>
	iterator insert(iterator pos, size_t count, const T& value)
	{
		check_iterator(pos);

		if (count == 0)
		{
			return pos;
		}

		size_t index = pos.ptr - v_data;

		if (v_size + count > v_capacity)
		{
			reserve(std::max(v_capacity * 2, v_size + count));
		}

		for (size_t i = v_size; i > index; --i)
		{
			AllocTraits::construct(v_allocator, &v_data[i + count - 1], std::move(v_data[i - 1]));
			AllocTraits::destroy(v_allocator, &v_data[i - 1]);
		}

		for (size_t i = 0; i < count; ++i)
		{
			AllocTraits::construct(v_allocator, &v_data[index + i], value); // 
		}

		v_size += count;

		return iterator(v_data + index);
	}
	/// <summary>
	/// Вставляет элементы из инициализирующего списка в вектор.
	/// </summary>
	/// <param name="pos">Итератор, указывающий на позицию для вставки</param>
	/// <param name="init_lis">Инициализирующий список элементов</param>
	/// <returns>Итератор, указывающий на первый вставленный элемент</returns>
	iterator insert(iterator pos, std::initializer_list<T> init_lis)
	{
		return insert(pos, init_lis.begin(), init_lis.end());
	}
	/// <summary>
	/// Вставляет элементы из диапазона [first, last) в вектор.
	/// </summary>
	/// <param name="pos">Итератор, указывающий на позицию для вставки</param>
	/// <param name="first">Итератор на первый элемент диапазона</param>
	/// <param name="last">Итератор на последний элемент диапазона</param>
	/// <returns>Итератор, указывающий на первый вставленный элемент</returns>
	template <typename InputIt,
		std::enable_if_t<!std::is_integral_v<InputIt>, int> = 0>
	iterator insert(iterator pos, InputIt first, InputIt last)
	{
		check_iterator(pos);

		size_t index = pos.ptr - v_data;
		size_t count = std::distance(first, last);

		if (count == 0)
		{
			return pos;
		}

		if (v_size + count > v_capacity)
		{
			reserve(std::max(v_capacity * 2, v_size + count));
		}

		for (size_t i = v_size; i > index; --i)
		{
			AllocTraits::construct(v_allocator, &v_data[i + count - 1], std::move(v_data[i - 1]));
			AllocTraits::destroy(v_allocator, &v_data[i - 1]);
		}

		for (auto it = first; it != last; ++it)
		{
			AllocTraits::construct(v_allocator, &v_data[index++], *it);
		}

		v_size += count;

		return iterator(v_data + index - count);
	}
	/// <summary>
	/// Удаляет элемент из вектора.
	/// </summary>
	/// <param name="pos">Итератор, указывающий на элемент для удаления</param>
	/// <returns>Итератор, указывающий на следующий элемент</returns>
	iterator erase(iterator pos)
	{
		check_iterator(pos);

		if (pos == end())
			throw std::out_of_range("Cannot erase end()");

		size_t index = pos.ptr - v_data;

		for (size_t i = index; i < v_size - 1; i++)
		{
			AllocTraits::destroy(v_allocator, &v_data[i]);
			AllocTraits::construct(v_allocator, &v_data[i], std::move(v_data[i + 1]));
		}
		AllocTraits::destroy(v_allocator, &v_data[v_size - 1]);
		v_size--;

		return iterator(v_data + index);
	}
	/// <summary>
	/// удаляет элементы из вектора в указанном диапазоне.
	/// </summary>
	/// <param name="first">начала диапозона</param>
	/// <param name="last">конец диапозона</param>
	/// <returns>итератор на следующий элемент</returns>
	iterator erase(iterator first, iterator last)
	{
		check_iterator(first);
		check_iterator(last);

		if (first.ptr > last.ptr)
		{
			throw std::out_of_range("Invalid iterator range");
		}

		if (first == last)
		{
			return first;
		}

		size_t start = first.ptr - v_data;
		size_t finish = last.ptr - v_data;

		size_t count = finish - start;

		for (size_t i = finish; i < v_size; i++)
		{
			AllocTraits::destroy(v_allocator, &v_data[i - count]);
			AllocTraits::construct(v_allocator, &v_data[i - count], std::move(v_data[i]));
		}

		destroy_range(v_size - count, v_size);

		v_size -= count;

		return iterator(v_data + start);
	}
	/// <summary>
	/// Обменивает содержимое вектора с другим вектором.
	/// </summary>
	/// <param name="other">Вектор, с которым нужно обменяться содержимым</param>
	void swap(Vector& other) noexcept
	{
		std::swap(v_allocator, other.v_allocator);
		std::swap(v_data, other.v_data);
		std::swap(v_size, other.v_size);
		std::swap(v_capacity, other.v_capacity);
	}

	// ==========================
	// Изменение размера
	// ==========================

	/// <summary>
	/// Назначает новые значения элементам вектора.
	/// </summary>
	/// <param name="count">Количество элементов для назначения</param>
	/// <param name="value">Значение для назначения</param>
	void assign(size_t count, const T& value)
	{
		if (count > v_capacity)
		{
			reserve(count);
		}

		destroy_elements();
		v_size = 0;

		for (size_t i = 0; i < count; i++)
		{
			AllocTraits::construct(v_allocator, &v_data[i], value);
			++v_size;
		}
	}
	/// <summary>
	/// Назначает новые значения элементам вектора.
	/// </summary>
	/// <param name="first">Итератор на первый элемент диапазона</param>
	/// <param name="last">Итератор на последний элемент диапазона</param>
	template <typename InputIt,
		std::enable_if_t<!std::is_integral_v<InputIt>, int> = 0>
	void assign(InputIt first, InputIt last)
	{
		size_t count = distance(first, last);

		if (count > v_capacity)
		{
			reserve(count);
		}
		
		destroy_elements();
		v_size = 0;
		
		for (auto it = first; it != last; ++it)
		{
			AllocTraits::construct(v_allocator, &v_data[v_size++], *it);
		}
	}
	/// <summary>
	/// Назначает новые значения элементам вектора.
	/// </summary>
	/// <param name="init_lis">Список инициализации</param>
	void assign(std::initializer_list<T> init_lis)
	{
		assign(init_lis.begin(), init_lis.end());
	}
};
