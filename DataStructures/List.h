#pragma once
#include <iostream>
#include <initializer_list>
/// <summary>
/// Структура узла Node для двусвязного списка, содержащая данные и указатели на следующий и предыдущий узлы.
/// </summary>
/// <typeparam name="T">Тип данных узла</typeparam>
template <typename T>
struct Node
{
	T data;
	Node* next;
	Node* prev;
	Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
	~Node() = default;
};

/// <summary>
/// Класс двусвязного списка List, реализующий основные операции вставки, удаления, доступа и сортировки элементов.
/// </summary>
/// <typeparam name="T">Тип данных узлов списка</typeparam>
template <typename T>
class List
{
private:
	Node<T>* head;
	Node<T>* tail;
	size_t list_size;
public: 
	/// <summary>
	/// Двунаправленный итератор для контейнера List.
	/// Позволяет последовательно перемещаться по элементам списка
	/// и получать доступ к хранимым данным.
	/// </summary>
	class iterator
	{
		friend class List<T>;
	private:
		Node<T>* ptr;
	public:
		
		iterator(Node<T>* ptr = nullptr) : ptr(ptr) {}
		T& operator*()
		{
			if (!ptr) throw std::runtime_error("Dereferencing null iterator");
			return ptr->data;
		}
		const T& operator*() const 
		{
			if (!ptr) throw std::runtime_error("Dereferencing null iterator"); 
			return ptr->data;
		}
		bool operator==(const iterator& other) const
		{
			return ptr == other.ptr;
		}

		iterator& operator++()
		{
			if (ptr) ptr = ptr->next;
			return *this;
		}
		iterator& operator++(int)
		{
			iterator temp = *this;
			++(*this);
			return temp;
		}
		iterator& operator--()
		{
			if (ptr) ptr = ptr->prev;
			return *this;
		}
		iterator& operator--(int count)
		{
			iterator temp = *this;
			--(*this);
			return temp;
		}
		bool operator!=(const iterator& other) const
		{
			return ptr != other.ptr;
		}

	};
private: 
	/// <summary>
	/// Объединяет два отсортированных списка в один отсортированный результат.
	/// </summary>
	/// <param name="left">Входной отсортированный список (левый). Используется для чтения и опустошаются.</param>
	/// <param name="right">Входной отсортированный список (правый). Используется для чтения и опустошаются.</param>
	void merge_sort(List<T>& left, List<T>& right)
	{
		while (!left.empty() && !right.empty())
		{
			if (left.front() < right.front())
				splice(end(), left, left.begin());
			else
				splice(end(), right, right.begin());
		}

		splice(end(), left);
		splice(end(), right);
	}
public:
	/// <summary>
	/// Конструктор по умолчанию. Инициализирует пустой список: устанавливает head и tail в nullptr и list_size в 0.
	/// </summary>
	List() : head(nullptr), tail(nullptr), list_size(0) {}
	/// <summary>
	/// Конструктор класса List, инициализирует список элементами из std::initializer_list.
	/// </summary>
	/// <param name="init_list">Инициализаторный список элементов (std::initializer_list<T>), которые будут добавлены в создаваемый список.</param>
	List(std::initializer_list<T> init_list) : List()
	{
		for (const auto& item : init_list)
		{
			push_back(item);
		}
	}
	/// <summary>
	/// Создаёт и инициализирует объект List, содержащий count элементов, каждый из которых создаётся значением по умолчанию для типа T, без неявных преобразований.
	/// </summary>
	/// <param name="count">Количество элементов, которые будут добавлены в список. Для каждого элемента вызывается конструктор по умолчанию T().</param>
	explicit List(size_t count) : List()
	{
		for (size_t i = 0; i < count; ++i)
		{
			push_back(T());
		}
	}
	/// <summary>
	/// Явный конструктор List, инициализирующий список заданным количеством копий значения.
	/// </summary>
	/// <param name="count">Количество элементов, которые будут добавлены в список.</param>
	/// <param name="value">Значение, копии которого будут добавлены в список (передаётся по const ссылке).</param>
	explicit List(size_t count, const T& value) : List()
	{
		for (size_t i = 0; i < count; ++i)
		{
			push_back(value);
		}
	}
	/// <summary>
	/// Инициализирует список, скопировав элементы из заданного диапазона итераторов.
	/// </summary>
	/// <typeparam name="InputIt">Тип итератора, который должен поддерживать операции инкремента и разыменования (минимум требования InputIterator).</typeparam>
	/// <param name="first">Итератор на первый элемент диапазона (включительно).</param>
	/// <param name="last">Итератор на элемент после последнего в диапазоне (исключительно).</param>
	template<class InputIt>
	List(InputIt first, InputIt last) : List()
	{
		for (auto it = first; it != last; ++it)
		{
			push_back(*it);
		}
	}
	/// <summary>
	/// Конструктор копирования, инициализирующий новый список как поэлементную копию переданного списка.
	/// </summary>
	/// <param name="other">Константная ссылка на список-источник, элементы которого будут скопированы.</param>
	List(const List& other) : List()
	{
		for (Node<T>* current = other.head; current != nullptr; current = current->next)
		{
			push_back(current->data);
		}
	}
	/// <summary>
	/// Конструктор перемещения класса List. Гарантирует отсутствие исключений,позволяет использовать move вместо копирования.
	/// </summary>
	/// <param name="other">Rvalue-ссылка на другой объект List, ресурсы и состояние которого передаются текущему объекту. После перемещения other остаётся пустым (head и tail устанавливаются в nullptr, list_size = 0).</param>
	List(List&& other) noexcept : head(other.head), tail(other.tail), list_size(other.list_size)
	{
		other.head = nullptr;
		other.tail = nullptr;
		other.list_size = 0;
	}

	/// <summary>
	/// Деструктор класса List. Вызывает clear() для очистки содержимого и освобождения связанных ресурсов при уничтожении объекта.
	/// </summary>
	~List()
	{
		clear();
	}

	/// <summary>
	/// Добавляет элемент в конец двусвязного списка: создаёт новый узел, обновляет указатели head/tail и увеличивает размер списка.
	/// </summary>
	/// <param name="value">Константная ссылка на значение элемента, которое будет скопировано в новый узел и добавлено в конец списка.</param>
	void push_back(const T& value)
	{
		Node<T>* new_node = new Node<T>(value);
		if (tail)
		{
			tail->next = new_node;
			new_node->prev = tail;
			tail = new_node;
		}
		else
		{
			head = new_node;
			tail = new_node;
		}
		list_size++;
	}
	/// <summary>
	/// Добавляет элемент в начало двусвязного списка: создаёт новый узел, обновляет указатели head/tail и увеличивает размер списка.
	/// </summary>
	/// <param name="value">Константная ссылка на значение элемента, которое будет скопировано в новый узел и добавлено в начало списка.</param>
	void push_front(const T& value)
	{
		Node<T>* new_node = new Node<T>(value);
		if (head)
		{
			new_node->next = head;
			head->prev = new_node;
			head = new_node;
		}
		else
		{
			head = new_node;
			tail = new_node;
		}
		list_size++;
	}
	/// <summary>
	/// Вставляет элемент в список перед позицией pos. Если pos == end(), элемент добавляется в конец; если pos указывает на начало, выполняется вставка в начало.
	/// </summary>
	/// <param name="pos">Итератор, перед которым будет вставлен новый элемент.</param>
	/// <param name="value">Константная ссылка на значение элемента, которое будет вставлено в список.</param>
	/// <returns>Итератор, указывающий на только что вставленный элемент.</returns>
	iterator insert(iterator pos, const T& value)
	{
		
		if (pos == end())
		{
			push_back(value);
			return iterator(tail);
		}
		if (pos.ptr == head)
		{
			push_front(value);
			return iterator(head);
		}
		Node<T>* new_node = new Node<T>(value);
		Node<T>* current = pos.ptr;
		
		new_node->next = current;
		new_node->prev = current->prev;
		current->prev->next = new_node;
		current->prev = new_node;
		list_size++;
		return iterator(new_node);
	}
	/// <summary>
	/// Переносит все узлы из другого списка в текущий список перед позицией pos. 
	/// </summary>
	/// <param name="pos">Итератор в текущем списке, указывающий позицию, перед которой будут вставлены элементы списка other. Если pos == end(), элементы добавляются в конец списка.</param>
	/// <param name="other">Ссылка на список, элементы которого перемещаются в текущий список. После операции other опустошается. </param>
	void splice(iterator pos, List& other)
	{
		if (this == &other || other.empty())
		{
			return;
		}
		if (empty())
		{
			head = other.head;
			tail = other.tail;
		}
		else if (pos == end())
		{

			tail->next = other.head;
			other.head->prev = tail;
			tail = other.tail;
		}
		else if (pos.ptr == head)
		{
			other.tail->next = head;
			head->prev = other.tail;
			head = other.head;
		}
		else
		{
			Node<T>* current = pos.ptr;
			Node<T>* before = current->prev;
			before->next = other.head;
			other.head->prev = before;

			other.tail->next = current;
			current->prev = other.tail;
		}
		list_size += other.list_size;

		other.head = nullptr;
		other.tail = nullptr;
		other.list_size = 0;
	}
	/// <summary>
	/// Переносит узел из другого списка в текущий список перед позицией pos. 
	/// </summary>
	/// <param name="pos">Итератор в текущем списке, указывающий позицию, перед которой будут вставлены элементы списка other. Если pos == end(), элементы добавляются в конец списка.</param>
	/// <param name="other">Ссылка на список, элементы которого перемещаются в текущий список. После операции other опустошается. </param>
	/// <param name="it">Переносимый элемент списка</param>
	void splice(iterator pos, List& other, iterator it)
	{
		if (this == &other && (pos == it || pos == ++iterator(it)))
			return;
		Node<T>* node = it.ptr;

		if (node->prev)
		{
			node->prev->next = node->next;
		}
		else
		{
			other.head = node->next;
		}
		if (node->next)
		{
			node->next->prev = node->prev;
		}
		else
		{
			other.tail = node->prev;
		}
		other.list_size--;
		if (empty())
		{
			node->prev = nullptr;
			node->next = nullptr;
			head = tail = node;
		}
		else if (pos == begin())
		{
			node->next = head;
			node->prev = nullptr;
			head->prev = node;
			head = node;
		}
		else if (pos == end())
		{
			node->prev = tail;
			node->next = nullptr;
			tail->next = node;
			tail = node;
		}
		else
		{
			Node<T>* current = pos.ptr;
			Node<T>* before = current->prev;

			before->next = node;
			node->prev = before;

			node->next = current;
			current->prev = node;
		}
		list_size++;
	}
	/// <summary>
	/// Удаляет последний элемент двусвязного списка.
	/// </summary>
	void pop_back()
	{
		if (!tail)
		{
			throw std::out_of_range("List is empty");
		}
		Node<T>* temp = tail;
		tail = tail->prev;
		if (tail)
		{
			tail->next = nullptr;
		}
		else
		{
			head = nullptr;
		}
		delete temp;
		--list_size;
	}
	/// <summary>
	/// Удаляет первого элемент двусвязного списка.
	/// </summary>
	void pop_front()
	{
		if (!head)
		{
			throw std::out_of_range("List is empty");
		}
		Node<T>* temp = head;
		head = head->next;
		if (head)
		{
			head->prev = nullptr;
		}
		else
		{
			tail = nullptr;
		}
		delete temp;
		--list_size;
	}
	/// <summary>
	/// Удаляет элемент списка, на который указывает итератор pos.
	/// </summary>
	/// <param name="pos">Итератор, указывающий на удаляемый элемент.</param>
	/// <returns>Итератор на элемент, следующий за удалённым. Если удалён последний элемент, возвращается end(). Если удалён был первый элемент, возвращается итератор на новый head.</returns>
	iterator erase(iterator pos)
	{
		if (pos == end())
			throw std::out_of_range("Cannot erase end() iterator");

		Node<T>* current = pos.ptr;
		Node<T>* next_node = current->next;
		if (current == head)
		{
			pop_front();
			return iterator(head);
		}
		if (current == tail)
		{
			pop_back();
			return end();
		}
		current->prev->next = current->next;
		current->next->prev = current->prev;
		delete current;
		--list_size;

		return iterator(next_node);
	}
	/// <summary>
	/// Удаляет все элементы, равные заданному значению.
	/// </summary>
	/// <param name="value">Значение; элементы, равные этому значению, будут удалены.</param>
	void remove(const T& value)
	{
		for (auto it = begin(); it != end(); )
		{
			if (*it == value)
			{
				it = erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	/// <summary>
	/// Удаляет подряд идущие одинаковые элементы из контейнера, оставляя по одному элементу в каждой последовательной группе равных значений
	/// </summary>
	void unique()
	{
		if (list_size < 2) return;

		auto it = begin();
		auto next = it;
		++next;

		while (next != end())
		{
			if (*it == *next)
				next = erase(next);
			else
			{
				it = next;
				++next;
			}
		}
	}

	/// <summary>
	/// Возвращает итератор, указывающий на начало контейнера.
	/// </summary>
	/// <returns>Итератор, инициализированный значением head, указывающий на первый элемент списка.</returns>
	iterator begin()
	{
		return iterator(head);
	}
	/// <summary>
	/// Возвращает итератор, указывающий на позицию после последнего элемента.
	/// </summary>
	/// <returns>Итератор, созданный с nullptr, обозначающий конец последовательности.</returns>
	iterator end()
	{
		return iterator(nullptr);
	}
	/// <summary>
	/// Возвращает итератор, указывающий на начало контейнера.
	/// </summary>
	/// <returns>Итератор, инициализированный значением head, указывающий на первый элемент списка.</returns>
	iterator begin() const
	{
		return iterator(head);
	}
	/// <summary>
	/// Возвращает итератор, указывающий на позицию после последнего элемента.
	/// </summary>
	/// <returns>Итератор, созданный с nullptr, обозначающий конец последовательности.</returns>
	iterator end() const
	{
		return iterator(nullptr);
	}

	/// <summary>
	/// Возвращает ссылку на первый элемент списка
	/// </summary>
	/// <returns>Ссылка на первый элемент (T&).</returns>
	T& front()
	{
		if (!head) throw std::out_of_range("List is empty");
		return head->data;
	}
	/// <summary>
	/// Возвращает ссылку на последний элемент списка
	/// </summary>
	/// <returns>Ссылка на последний элемент (T&).</returns>
	T& back()
	{
		if (!tail) throw std::out_of_range("List is empty");
		return tail->data;
	}

	/// <summary>
	/// Проверка на пустоту списка.
	/// </summary>
	/// <returns>true - пуст; false - не пуст</returns>
	bool empty() const
	{
		return list_size == 0;
	}
	/// <summary>
	/// Возвращает количество элементов в списке.
	/// </summary>
	/// <returns>Количество элементов в списке в виде значения типа size_t (возвращает list_size).</returns>
	size_t size() const
	{
		return list_size;
	}
	/// <summary>
	/// Изменяет размер контейнера до указанного значения. Если новый размер больше текущего, добавляет элементы T() в конец; если меньше — удаляет элементы с конца.
	/// </summary>
	/// <param name="new_size">Желаемый размер контейнера (количество элементов).</param>
	void resize(size_t new_size)
	{
		while (list_size < new_size)
		{
			push_back(T());
		}
		while(list_size > new_size)
		{
			pop_back();
		}
	}
	/// <summary>
	/// Изменяет размер контейнера до указанного значения. Если новый размер больше текущего, добавляет элементы value в конец; если меньше — удаляет элементы с конца.
	/// </summary>
	/// <param name="new_size">Желаемый размер контейнера (количество элементов).</param>
	/// <param name="value">Желаемый значение каждого нового элемента.</param>
	void resize(size_t new_size, const T& value)
	{
		while (list_size < new_size)
		{
			push_back(value);
		}
		while(list_size > new_size)
		{
			pop_back();
		}
	}

	/// <summary>
	/// Обменивает содержимое данного списка с другим списком. Операция noexcept — не генерирует исключений.
	/// </summary>
	/// <param name="other">Ссылка на другой список, с которым будет выполнен обмен (обмениваются head, tail и list_size).</param>
	void swap(List& other) noexcept
	{
		std::swap(head, other.head);
		std::swap(tail, other.tail);
		std::swap(list_size, other.list_size);
	}
	/// <summary>
	/// Разворачивает двусвязный список на месте: для каждого узла меняет местами указатели next и prev и затем меняет head и tail.
	/// </summary>
	void reverse()
	{
		Node<T>* current = head;
		Node<T>* temp = nullptr;
		while (current)
		{
			temp = current->prev;
			current->prev = current->next;
			current->next = temp;
			current = current->prev;
		}
		std::swap(head, tail);
	}

	/// <summary>
	/// Выполняет сортировку элементов списка методом «разделяй и властвуй» с последующим слиянием (merge sort).
	/// </summary>
	void sort()
	{
		if (list_size < 2) return;
		List<T> left;
		List<T> right;

		size_t mid = list_size / 2;
		size_t index = 0;
		for (Node<T>* current = head; current != nullptr; current = current->next)
		{
			if (index < mid)
			{
				left.push_back(current->data);
			}
			else
			{
				right.push_back(current->data);
			}
			++index;
		}
		left.sort();
		right.sort();
		clear();
		merge_sort(left, right);
	}

	/// <summary>
	/// Оператор присваивания для List. Выполняет копирование элементов из другого списка.
	/// </summary>
	/// <param name="other">Константная ссылка на исходный список, элементы которого копируются.</param>
	/// <returns>Ссылка на текущий объект ( *this ).</returns>
	List& operator=(const List& other)
	{
		if (this != &other)
		{
			clear();
			for (Node<T>* current = other.head; current != nullptr; current = current->next)
			{
				push_back(current->data);
			}
		}
		return *this;
	}
	/// <summary>
	/// Перемещающее присваивание. Освобождает текущие ресурсы и принимает ресурсы из other (noexcept)
	/// </summary>
	/// <param name="other">Источник для перемещения (rvalue-ссылка). После операции other становится пустым (head и tail = nullptr, размер = 0).</param>
	/// <returns>Ссылка на текущий объект List (*this) после перемещения.</returns>
	List& operator=(List&& other) noexcept
	{
		if (this != &other)
		{
			clear();
			head = other.head;
			tail = other.tail;
			list_size = other.list_size;
			other.head = nullptr;
			other.tail = nullptr;
			other.list_size = 0;
		}
		return *this;
	}
	/// <summary>
	/// Заменяет содержимое списка элементами из std::initializer_list.
	/// </summary>
	/// <param name="init_list">Список инициализаторов типа std::initializer_list<T>, элементы которого будут скопированы в список.</param>
	/// <returns>Ссылка на текущий объект списка (то есть *this).</returns>
	List& operator=(std::initializer_list<T> init_list)
	{
		clear();
		for (auto& item : init_list)
		{
			push_back(item);
		}
		return *this;
	}
	/// <summary>
	/// Сравнивает текущий список с другим списком по равенству: проверяет одинаковый размер и поэлементное равенство.
	/// </summary>
	/// <param name="other">Другой список того же типа, с которым сравнивается текущий список.</param>
	/// <returns>true, если списки имеют одинаковый размер и все соответствующие элементы равны; в противном случае — false.</returns>
	bool operator==(const List& other) const
	{
		if (list_size != other.list_size)
			return false;
		Node<T>* current = head;
		Node<T>* other_current = other.head;
		while (current && other_current)
		{
			if (current->data != other_current->data)
				return false;
			current = current->next;
			other_current = other_current->next;
		}
		return true;
	}

	/// <summary>
	/// Удаляет все узлы списка, освобождая память, и сбрасывает указатели head и tail, а также размер списка.
	/// </summary>
	void clear()
	{
		Node<T>* current = head;
		while (current)
		{
			Node<T>* next_node = current->next;
			delete current;
			current = next_node;
		}
		head = nullptr;
		tail = nullptr;
		list_size = 0;
	}
};