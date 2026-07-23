#pragma once
#include "IHashTable.h"
#include <stdexcept>

template<class Key,	class Value, class Hash, class KeyEqual>
class HashMapChaining;

/// <summary>
/// Итератор для хеш-таблицы с цепочками.
/// </summary>
template <class Key, class Value, class Hash, class KeyEqual>
class HashTableIterator
{
	friend class HashMapChaining<Key, Value, Hash, KeyEqual>;
private:
	HashMapChaining<Key, Value, Hash, KeyEqual>* map_ = nullptr;
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
	HashTableIterator() = default;
	HashTableIterator(HashMapChaining<Key, Value, Hash, KeyEqual>* map, size_t b, size_t e) : map_(map), bucket_idx_(b), elem_idx_(e)
	{
		if (map_) skip_empty();
	}

	std::pair<const Key, Value>& operator*()
	{
		if (!map_ || bucket_idx_ >= map_->buckets_.size())
		{
			throw std::runtime_error(
				"Dereferencing invalid iterator");
		}

		auto& bucket = map_->buckets_[bucket_idx_];

		auto it = bucket.begin();
		std::advance(it, elem_idx_);

		return *it;
	}

	const std::pair<const Key, Value>& operator*() const
	{
		if (!map_ || bucket_idx_ >= map_->buckets_.size())
		{
			throw std::runtime_error(
				"Dereferencing invalid iterator");
		}

		const auto& bucket = map_->buckets_[bucket_idx_];

		auto it = bucket.cbegin();
		std::advance(it, elem_idx_);

		return *it;
	}

	HashTableIterator& operator++()
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
	HashTableIterator operator++(int)
	{
		HashTableIterator tmp = *this;
		++(*this);
		return tmp;
	}
	HashTableIterator& operator--()
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
			{
				throw std::runtime_error("Iterator underflow");
			}

			--bucket_idx_;
			while (bucket_idx_ > 0 && map_->buckets_[bucket_idx_].empty())
			{
				--bucket_idx_;
			}

			elem_idx_ = map_->buckets_[bucket_idx_].size() - 1;
		}

		return *this;
	}
	HashTableIterator operator--(int)
	{
		HashTableIterator tmp = *this;
		--(*this);
		return tmp;
	}

	bool operator==(const HashTableIterator& other) const
	{
		return map_ == other.map_ &&
			bucket_idx_ == other.bucket_idx_ &&
			elem_idx_ == other.elem_idx_;
	}
	bool operator!=(const HashTableIterator& other) const
	{
		return !(*this == other);
	}
};