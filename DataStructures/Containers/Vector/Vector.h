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
	explicit Vector(size_t count) : v_data(v_allocator.allocate(count)), v_size(0), v_capacity(count)
	{
		size_t built_elements = 0;
		try
		{
			for (built_elements = 0; built_elements < count; built_elements++)
			{
				AllocTraits::construct(v_allocator, v_data + built_elements);
			}
		}
		catch (...)
		{
			destroy_elements(built_elements);
			deallocate_data();

			throw;
		}

		v_size = count;
	}
	/// <summary>
	/// Конструктор, инициализирующий вектор заданным количеством элементов и значением.
	/// </summary>
	/// <param name="count">Количество элементов в векторе</param>
	/// <param name="value">Значение для инициализации элементов</param>
	explicit Vector(size_t count, const T& value) : v_data(v_allocator.allocate(count)), v_size(0), v_capacity(count)
	{
		size_t built_elements = 0;

		try
		{
			for (built_elements = 0; built_elements < count; built_elements++)
			{
				AllocTraits::construct(v_allocator, v_data + built_elements, value);
			}
		}
		catch (...)
		{
			destroy_elements(built_elements);
			deallocate_data();

			throw;
		}

		v_size = count;
	}
	/// <summary>
	/// Конструктор, инициализирующий вектор с помощью списка инициализации.
	/// </summary>
	/// <param name="init_lis">Список инициализации</param>
	Vector(std::initializer_list<T> init_lis) : v_data(v_allocator.allocate(init_lis.size())), v_size(0), v_capacity(init_lis.size())
	{
		size_t built_elements = 0;

		try
		{
			for (const auto& item : init_lis)
			{
				AllocTraits::construct(v_allocator, v_data + built_elements, item);
				built_elements++;
			}
		}
		catch (...)
		{
			destroy_elements(built_elements);
			deallocate_data();

			throw;
		}

		v_size = init_lis.size();
	}
	/// <summary>
	/// Конструктор копирования.
	/// </summary>
	/// <param name="other">Другой вектор, из которого происходит копирование</param>
	Vector(const Vector& other) : v_data(v_allocator.allocate(other.v_capacity)), v_size(0), v_capacity(other.v_capacity)
	{
		size_t constructed = 0;

		try
		{
			for (; constructed < other.v_size; ++constructed)
			{
				AllocTraits::construct(v_allocator, v_data + constructed, other.v_data[constructed]);
			}
		}
		catch (...)
		{
			destroy_elements(constructed);
			deallocate_data();

			throw;
		}

		v_size = other.v_size;
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

		try
		{
			for (auto it = first; it != last; ++it)
			{
				AllocTraits::construct(v_allocator, &v_data[v_size], *it);
				++v_size;
			}
		}
		catch (...)
		{
			destroy_elements(v_size);
			deallocate_data();

			throw;
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
		destroy_elements(v_data, v_size);
	}
	void destroy_elements(size_t count)
	{
		destroy_elements(v_data, count);
	}
	void destroy_elements(T* vec, size_t count)
	{
		for (size_t i = count; i > 0; --i)
		{
			AllocTraits::destroy(v_allocator, &vec[i - 1]);
		}
	}
	void destroy_range(T* data, size_t first, size_t last)
	{
		while (last > first)
		{
			--last;
			AllocTraits::destroy(v_allocator, data + last);
		}
	}
	void destroy_range(size_t first, size_t last)
	{
		destroy_range(v_data, first, last);
	}


	void deallocate_data() noexcept
	{
		deallocate_data(v_data, v_capacity);
	}
	void deallocate_data(T*& data, size_t& capacity) noexcept
	{
		if (data)
		{
			v_allocator.deallocate(data, capacity);
			data = nullptr;
			capacity = 0;
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
		Vector temp(init_list);
		swap(temp);

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
	
	template <typename ConstructElement>
	void grow_to_size(size_t new_size, ConstructElement construct_element)
	{
		const size_t old_size = v_size;

		// Текущей capacity достаточно.
		if (new_size <= v_capacity)
		{
			size_t added = 0;

			try
			{
				for (size_t i = old_size; i < new_size; ++i)
				{
					construct_element(v_data + i);
					++added;
				}
			}
			catch (...)
			{
				destroy_range(old_size, old_size + added);
				throw;
			}

			v_size = new_size;
			return;
		}

		// Требуется новое хранилище.
		size_t new_capacity = std::max(
			v_capacity == 0 ? size_t{ 1 } : v_capacity * 2,
			new_size);

		T* new_data = v_allocator.allocate(new_capacity);

		size_t added = 0;
		size_t relocated = 0;

		try
		{
			// Сначала создаём добавляемые элементы.
			for (size_t i = old_size; i < new_size; ++i)
			{
				construct_element(new_data + i);
				++added;
			}

			// Затем переносим старые элементы.
			for (; relocated < old_size; ++relocated)
			{
				AllocTraits::construct(v_allocator,	new_data + relocated, std::move_if_noexcept(v_data[relocated]));
			}
		}
		catch (...)
		{
			destroy_elements(new_data, relocated);

			destroy_range(new_data,old_size,old_size + added);

			deallocate_data(new_data, new_capacity);
			throw;
		}

		destroy_elements();
		deallocate_data();

		v_data = new_data;
		v_size = new_size;
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
		if (new_size < v_size)
		{
			destroy_range(new_size, v_size);
			v_size = new_size;
			return;
		}

		if (new_size == v_size)
		{
			return;
		}

		grow_to_size(
			new_size,
			[this](T* address)
			{
				AllocTraits::construct(v_allocator, address);
			});
	}
	/// <summary>
	/// Изменяет количество элементов в векторе, заполняя новые элементы заданным значением.
	/// </summary>
	/// <param name="new_size">Новое количество элементов</param>
	/// <param name="value">Значение для новых элементов</param>
	void resize(size_t new_size, const T& value)
	{
		if (new_size < v_size)
		{
			destroy_range(new_size, v_size);
			v_size = new_size;
			return;
		}

		if (new_size == v_size)
		{
			return;
		}

		grow_to_size(
			new_size,
			[this, &value](T* address)
			{
				AllocTraits::construct(v_allocator,	address, value);
			});
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
			deallocate_data();
			return;
		}

		T* new_data = v_allocator.allocate(v_size);

		replace_storage(new_data, v_size);
	}

	// =============================
	// изменение содержимого вектора
	// =============================
private:
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
		emplace_back(value); // вместо десятка строк ручной реализации
	}
	/// <summary>
	/// Добавляет элемент в конец вектора, используя перемещение.
	/// </summary>
	/// <param name="value">Добавляемый элемент</param>
	void push_back(T&& value)
	{
		emplace_back(std::move(value));
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
		if (v_size < v_capacity)
		{
			AllocTraits::construct(
				v_allocator,
				v_data + v_size,
				std::forward<Args>(args)...);

			++v_size;
			return;
		}

		size_t new_capacity = v_capacity == 0
			? 1
			: v_capacity * 2;

		T* new_data = v_allocator.allocate(new_capacity);

		size_t relocated = 0;
		bool new_element_constructed = false;

		try
		{
			AllocTraits::construct(v_allocator, &new_data[v_size], std::forward<Args>(args)...); // буквально так, как внутри construct

			new_element_constructed = true;

			for (; relocated < v_size; ++relocated)
			{
				AllocTraits::construct(v_allocator, new_data + relocated, std::move_if_noexcept(v_data[relocated]));
			}
		}
		catch (...)
		{
			destroy_elements(new_data, relocated);

			if (new_element_constructed)
			{
				AllocTraits::destroy(v_allocator, new_data + v_size);
			}

			deallocate_data(new_data, new_capacity);
			throw;
		}

		destroy_elements();
		deallocate_data();

		v_data = new_data;
		v_capacity = new_capacity;
		++v_size;
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
		check_iterator(pos);

		size_t index = v_data == nullptr
			? 0
			: static_cast<size_t>(pos.ptr - v_data);

		size_t new_capacity;

		if (v_size == v_capacity)
		{
			new_capacity = v_capacity == 0
				? 1
				: v_capacity * 2;
		}
		else
		{
			new_capacity = v_capacity;
		}

		T* new_data = v_allocator.allocate(new_capacity);
		size_t constructed = 0;

		try
		{
			for (size_t i = 0; i < index; ++i)
			{
				AllocTraits::construct(v_allocator,	new_data + constructed,	v_data[i]);
				++constructed;
			}

			AllocTraits::construct(	v_allocator,new_data + constructed,	value);
			++constructed;

			for (size_t i = index; i < v_size; ++i)
			{
				AllocTraits::construct(	v_allocator,new_data + constructed,	v_data[i]);
				++constructed;
			}
		}
		catch (...)
		{
			destroy_elements(new_data, constructed);
			deallocate_data(new_data, new_capacity);
			throw;
		}

		destroy_elements();
		deallocate_data();

		v_data = new_data;
		v_capacity = new_capacity;
		++v_size;

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

		size_t index = v_data == nullptr
			? 0
			: static_cast<size_t>(pos.ptr - v_data);

		size_t new_capacity;

		if (v_size + count > v_capacity)
		{
			new_capacity = std::max(v_capacity * 2, v_size + count);
		}
		else
		{
			new_capacity = v_capacity;
		}

		T* new_data = v_allocator.allocate(new_capacity);
		size_t constructed = 0;

		try
		{
			for (size_t i = 0; i < index; ++i)
			{
				AllocTraits::construct(v_allocator,	new_data + constructed,	v_data[i]);
				++constructed;
			}

			for (size_t i = 0; i < count; ++i)
			{
				AllocTraits::construct(v_allocator,	new_data + constructed,	value);
				++constructed;
			}

			for (size_t i = index; i < v_size; ++i)
			{
				AllocTraits::construct(v_allocator, new_data + constructed,	v_data[i]);
				++constructed;
			}
		}
		catch (...)
		{
			destroy_elements(new_data, constructed);
			deallocate_data(new_data, new_capacity);

			throw;
		}

		destroy_elements();
		deallocate_data();

		v_data = new_data;
		v_capacity = new_capacity;
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
		
		size_t index = v_data == nullptr
			? 0
			: static_cast<size_t>(pos.ptr - v_data);

		size_t count = distance(first, last);

		if (count == 0)
		{
			return pos;
		}

		size_t new_capacity;

		if (v_size + count > v_capacity)
		{
			new_capacity = std::max(v_capacity * 2, v_size + count);
		}
		else
		{
			new_capacity = v_capacity;
		}

		T* new_data = v_allocator.allocate(new_capacity);
		size_t constructed = 0;

		try
		{
			for (size_t i = 0; i < index; ++i)
			{
				AllocTraits::construct(v_allocator, new_data + constructed, v_data[i]);
				++constructed;
			}

			for (auto it = first; it != last; ++it)
			{
				AllocTraits::construct(v_allocator, new_data + constructed, *it);
				++constructed;
			}

			for (size_t i = index; i < v_size; ++i)
			{
				AllocTraits::construct(v_allocator, new_data + constructed, v_data[i]);
				++constructed;
			}
		}
		catch (...)
		{
			destroy_elements(new_data, constructed);
			deallocate_data(new_data, new_capacity);
			throw;
		}

		destroy_elements();
		deallocate_data();

		v_data = new_data;
		v_capacity = new_capacity;

		v_size += count;

		return iterator(v_data + index);
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

		for (size_t i = index; i + 1 < v_size; ++i)
		{
			v_data[i] = std::move(v_data[i + 1]);
		}

		AllocTraits::destroy(v_allocator, v_data + v_size - 1);

		--v_size;

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

		size_t start = static_cast<size_t>(first.ptr - v_data);
		size_t finish = static_cast<size_t>(last.ptr - v_data);
		size_t count = finish - start;

		for (size_t i = finish; i < v_size; ++i)
		{
			v_data[i - count] = std::move(v_data[i]);
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
		if (count == 0)
		{
			clear();
			return;
		}

		size_t new_capacity;

		if (count > v_capacity)
		{
			new_capacity = std::max(v_capacity * 2, count);
		}
		else
		{
			new_capacity = v_capacity;
		}

		T* assing_data = v_allocator.allocate(new_capacity);

		size_t constructred = 0;
		try
		{
			for (constructred = 0; constructred < count; constructred++)
			{
				AllocTraits::construct(v_allocator, assing_data + constructred, value);
			}
		}
		catch (...)
		{
			destroy_elements(assing_data, constructred);
			deallocate_data( assing_data, new_capacity);
			throw;
		}

		destroy_elements();
		deallocate_data();

		v_data = assing_data;
		v_capacity = new_capacity;
		v_size = count;

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
		Vector temp(first, last);
		swap(temp);
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
