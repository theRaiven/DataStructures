#pragma once
#include <vector>
#include <utility>
#include <cmath>
#include <stdexcept>
// добавлю еще таблицу
/// <summary>
/// Интерфейс хеш-таблицы с основными операциями вставки, удаления, поиска и получения размера.
/// </summary>
/// <typeparam name="Key">Тип передаваемого ключа</typeparam>
/// <typeparam name="Value">Тип значение по ключу</typeparam>
template<class Key, class Value>
class IHashTable
{
public:
	/// <summary>
	/// метод для вставки пары ключ-значение в хеш-таблицу.
	/// </summary>
	/// <param name="key">Ключ для вставки в хеш-таблицу.</param>
	/// <param name="value">Значние для вставки по ключу</param>
	virtual void emplace(const Key& key, const Value& value) = 0;
	/// <summary>
	/// метод для удаления элемента по ключу из хеш-таблицы. находит элемент с заданным ключом и удаляет его из таблицы.
	/// </summary>
	/// <param name="key">Ключ для удаленияизв хеш-таблицы.</param>
	/// <returns>true - если элемент удален. false - если элемента не существовало</returns>
	virtual bool erase(const Key& key) = 0;
	/// <summary>
	/// Метод для поиска значения по ключу в хеш-таблице.
	/// </summary>
	/// <param name="key"> Ключ для поиска в хеш-таблице.</param>
	/// <param name="value"> Ссылка на переменную, в которую будет записано найденное значение, если ключ существует в таблице. </param>
	/// <returns></returns>
	virtual bool find(const Key& key, Value& value) const = 0;
	/// <summary>
	/// метод для получения количества элементов в хеш-таблице.
	/// </summary>
	/// <returns> Возаращает количество элементов в хеш-таблице типа size_t. </returns>
	virtual size_t size() const = 0;
};

/// <summary>
/// Класс хеш-таблицы с цепочками для разрешения коллизий.
/// </summary>
/// <typeparam name="Key"> Тип передаваемого ключа</typeparam>
/// <typeparam name="Value"> Тип значение по ключу</typeparam>
/// <typeparam name="Hash"> Функция хеширования для ключей (по умолчанию std::hash<Key>)</typeparam>
/// <typeparam name="KeyEqual"> Функция сравнения ключей на равенство (по умолчанию std::equal_to<Key>)</typeparam>
template<class Key,	class Value, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
class HashMapChaining : public IHashTable<Key, Value>
{
	using Bucket = std::vector<std::pair<Key, Value>>;
private:
	std::vector<Bucket> buckets_;
	size_t size_ = 0;

	Hash hash_;
	KeyEqual equal_;

	float max_load_factor_ = 1.0f;
public:
	/// <summary>
	/// Итератор для хеш-таблицы с цепочками.
	/// </summary>
	class iterator
	{
		friend class HashMapChaining;
	private:
		HashMapChaining* map_ = nullptr;
		size_t bucket_idx_ = 0;
		size_t elem_idx_ = 0;

		void skip_empty()
		{
			while (bucket_idx_ < map_->buckets_.size() &&
				map_->buckets_[bucket_idx_].empty())
			{
				++bucket_idx_;
				elem_idx_ = 0;
			}
		}
	public:
		iterator() = default;
		iterator(HashMapChaining* map, size_t b, size_t e) : map_(map), bucket_idx_(b), elem_idx_(e)
		{
			if (map_) skip_empty();
		}

		std::pair<Key, Value>& operator*()
		{
			if (!map_ || bucket_idx_ >= map_->buckets_.size())
				throw std::runtime_error("Dereferencing invalid iterator");
			return map_->buckets_[bucket_idx_][elem_idx_];
		}
		const std::pair<Key, Value>& operator*() const
		{
			if (!map_ || bucket_idx_ >= map_->buckets_.size())
				throw std::runtime_error("Dereferencing invalid iterator");
			return map_->buckets_[bucket_idx_][elem_idx_];
		}

		iterator& operator++()
		{
			if (!map_)
				throw std::runtime_error("Invalid iterator");

			if (map_->size_ == 0)
				throw std::runtime_error("Cannot increment iterator: empty table");

			++elem_idx_;
			if (elem_idx_ >= map_->buckets_[bucket_idx_].size())
			{
				++bucket_idx_;
				elem_idx_ = 0;
				skip_empty();
			}
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp = *this;
			++(*this);
			return tmp;
		}
		iterator& operator--()
		{
			if (!map_)
				throw std::runtime_error("Invalid iterator");

			if (map_->size_ == 0)
				throw std::runtime_error("Cannot decrement iterator: empty table");

			if (*this == map_->begin())
				throw std::runtime_error("Iterator underflow");

			if (bucket_idx_ == map_->buckets_.size())
			{
				bucket_idx_ = map_->buckets_.size() - 1;
				while (bucket_idx_ > 0 && map_->buckets_[bucket_idx_].empty())
					--bucket_idx_;

				elem_idx_ = map_->buckets_[bucket_idx_].size() - 1;
				return *this;
			}

			if (elem_idx_ > 0)
			{
				--elem_idx_;
			}
			else
			{
				if (bucket_idx_ == 0)
					throw std::runtime_error("Iterator underflow");

				--bucket_idx_;
				while (bucket_idx_ > 0 && map_->buckets_[bucket_idx_].empty())
					--bucket_idx_;

				elem_idx_ = map_->buckets_[bucket_idx_].size() - 1;
			}

			return *this;
		}
		iterator operator--(int)
		{
			iterator tmp = *this;
			--(*this);
			return tmp;
		}

		bool operator==(const iterator& other) const
		{
			return map_ == other.map_ &&
				bucket_idx_ == other.bucket_idx_ &&
				elem_idx_ == other.elem_idx_;
		}
		bool operator!=(const iterator& other) const
		{
			return !(*this == other);
		}
	};
	/// <summary>
	/// Конструктор хеш-таблицы с цепочками с пользовательскими функциями хеширования и сравнения.
	/// </summary>
	/// <param name="bucket_count"> Начальное количество бакетов (цепочек) в хеш-таблице</param>
	/// <param name="h"> Пользовательская функция хеширования для ключей.</param>
	/// <param name="eq"> Пользовательская функция сравнения ключей на равенство.</param>
	explicit HashMapChaining(size_t bucket_count = 8, const Hash& h = Hash{}, const KeyEqual& eq = KeyEqual{})
		: buckets_(bucket_count > 0 ? bucket_count : 1), hash_(h), equal_(eq) {}
	/// <summary>
	/// Деструктор хеш-таблицы с цепочками.
	/// </summary>
	~HashMapChaining()
	{
		clear();
	}

	/// <summary>
	/// метод для вставки пары ключ-значение в хеш-таблицу.
	/// </summary>
	/// <param name="key">Ключ для вставки в хеш-таблицу.</param>
	/// <param name="value">Значние для вставки по ключу</param>
	void emplace(const Key& key, const Value& value) override
	{
		size_t index = hash_(key) % buckets_.size();
		Bucket& bucket = buckets_[index];

		for (auto& data : bucket)
		{
			if (equal_(data.first, key))
			{
				data.second = value;
				return;
			}
		}

		buckets_[index].emplace_back(key, value);
		++size_;
		if (load_factor() > max_load_factor_)
			rehash(buckets_.size() * 2);
	}
	/// <summary>
	/// Метод для поиска значения по ключу в хеш-таблице.
	/// </summary>
	/// <param name="key"> Ключ для поиска в хеш-таблице.</param>
	/// <param name="value"> Ссылка на переменную, в которую будет записано найденное значение, если ключ существует в таблице. </param>
	/// <returns></returns>
	bool find(const Key& key, Value& value) const override
	{
		size_t index = hash_(key) % buckets_.size();
		const Bucket& bucket = buckets_[index];

		for (const auto& data : bucket)
		{
			if (equal_(data.first, key))
			{
				value = data.second;
				return true;
			}
		}
		return false;
	}
	/// <summary>
	/// метод для удаления элемента по ключу из хеш-таблицы. находит элемент с заданным ключом и удаляет его из таблицы.
	/// </summary>
	/// <param name="key">Ключ для удаленияизв хеш-таблицы.</param>
	/// <returns>true - если элемент удален. false - если элемента не существовало</returns>
	bool erase(const Key& key) override
	{
		size_t index = hash_(key) % buckets_.size();
		Bucket& bucket = buckets_[index];
		for (auto it = bucket.begin(); it != bucket.end(); ++it)
		{
			if (equal_(it->first, key))
			{
				bucket.erase(it);
				--size_;
				return true;
			}
		}
		return false;
	}
	/// <summary>
	/// метод для получения количества элементов в хеш-таблице.
	/// </summary>
	/// <returns> Возаращает количество элементов в хеш-таблице типа size_t. </returns>
	size_t size() const override
	{
		return size_;
	}

	/// <summary>
	/// Метод для изменения количества бакетов (цепочек) в хеш-таблице.
	/// </summary>
	/// <param name="new_bucket_count"> Новое количество бакетов (цепочек) в хеш-таблице.</param>
	void rehash(size_t new_bucket_count)
	{
		if (new_bucket_count < 1) new_bucket_count = 1;

		std::vector<Bucket> new_buckets(new_bucket_count);
		for (const auto& bucket : buckets_)
		{
			for (const auto& data : bucket)
			{
				size_t new_index = hash_(data.first) % new_bucket_count;
				new_buckets[new_index].emplace_back(data);
			}
		}

		buckets_.swap(new_buckets);
	}
	/// <summary>
	/// Метод для резервирования места в хеш-таблице.
	/// </summary>
	/// <param name="new_capacity"> Желаемая вместимость хеш-таблицы (количество элементов).</param>
	void reserve(size_t new_capacity)
	{
		if (new_capacity <= size_) return;

		size_t required_buckets = static_cast<size_t>(std::ceil(static_cast<float>(new_capacity) / max_load_factor_));

		if (required_buckets > buckets_.size()) rehash(required_buckets);
	}

	/// <summary>
	/// Метод для получения текущего коэффициента загрузки хеш-таблицы.
	/// </summary>
	/// <returns> Текущий коэффициент загрузки хеш-таблицы (число элементов / число бакетов).</returns>
	float load_factor() const
	{
		return static_cast<float>(size_) / buckets_.size();
	}
	/// <summary>
	/// Расчет максимального коэффициента загрузки хеш-таблицы.
	/// </summary>
	/// <param name="lf"> Желаемый максимальный коэффициент загрузки (float).</param>
	void max_load_factor(float lf)
	{
		max_load_factor_ = lf;
		reserve(size_);
	}

	/// <summary>
	/// Метод для доступа к значению по ключу с проверкой наличия ключа.
	/// </summary>
	/// <param name="key">Ключ искомого элемента</param>
	/// <returns>Ссылка на полученный элемент</returns>
	Value& at(const Key& key)
	{
		size_t index = hash_(key) % buckets_.size();
		Bucket& bucket = buckets_[index];

		for (auto& data : bucket)
		{
			if (equal_(data.first, key))
			{
				return data.second;
			}
		}

		throw std::out_of_range("HashMapChaining::at: key not found");
	}
	/// <summary>
	/// Метод для доступа к значению по ключу с проверкой наличия ключа.
	/// </summary>
	/// <param name="key">Ключ искомого элемента</param>
	/// <returns>Константная ссылка на полученный элемент</returns>
	const Value& at(const Key& key) const
	{
		size_t index = hash_(key) % buckets_.size();
		const Bucket& bucket = buckets_[index];

		for (const auto& data : bucket)
		{
			if (equal_(data.first, key))
			{
				return data.second;
			}
		}

		throw std::out_of_range("HashMapChaining::at: key not found");
	}
	/// <summary>
	/// Оператор для доступа к значению по ключу. Если ключ не существует, создается новый элемент с этим ключом и значением по умолчанию.
	/// </summary>
	/// <param name="key"> Ключ искомого элемента </param>
	/// <returns> Ссылка на полученный или созданный элемент</returns>
	Value& operator[](const Key& key)
	{
		size_t index = hash_(key) % buckets_.size();
		Bucket& bucket = buckets_[index];

		for(auto& data : bucket)
		{
			if (equal_(data.first, key))
			{
				return data.second;
			}
		}

		buckets_[index].emplace_back(key, Value{});
		++size_;

		if (load_factor() > max_load_factor_)
		{
			rehash(buckets_.size() * 2);
			return (*this)[key];
		}

		return bucket.back().second;
	}

	/// <summary>
	/// Очищает хеш-таблицу, удаляя все элементы.
	/// </summary>
	void clear()
	{
		for (auto& bucket : buckets_)
			bucket.clear();
		size_ = 0;
	}
	/// <summary>
	/// Метод для проверки, пуста ли хеш-таблица.
	/// </summary>
	/// <returns> true - если хеш-таблица пуста (не содержит элементов), иначе false</returns>
	bool empty() const
	{
		return size() == 0;
	}

	/// <summary>
	/// Итератор, указывающий на начало контейнера.
	/// </summary>
	/// <returns> Итератор, инициализированный значением begin, указывающий на первый элемент хеш-таблицы </returns>
	iterator begin()
	{
		return iterator(this, 0, 0);
	}
	/// <summary>
	/// Итератор, указывающий на позицию после последнего элемента.
	/// </summary>
	/// <returns> Итератор, созданный с позицией за последним элементом, обозначающий конец последовательности.</returns>
	iterator end()
	{
		return iterator(this, buckets_.size(), 0);
	}
};