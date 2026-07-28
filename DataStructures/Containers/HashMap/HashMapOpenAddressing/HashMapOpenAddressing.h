#pragma once
#include <cstddef>
#include <functional>
#include <optional>
#include <utility>
#include <vector>
#include "../IHashTable.h"

namespace ds
{
	/// <summary>
	///	Состояние ячейки хеш таблицы в текущий момент
	/// </summary>
	enum class SlotState
	{
		Empty,
		Occupied,
		Deleted
	};

	/// <summary>
	/// Структура ячейци хеш таблицы
	/// </summary>
	/// <typeparam name="Key">Ключ ячейки</typeparam>
	/// <typeparam name="Value">значние ячейки</typeparam>
	template<typename Key, typename Value>
	struct Slot
	{
		SlotState state = SlotState::Empty;
		std::optional<std::pair<Key, Value>> data;
	};

	template<typename Key, typename Value, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
	class HashMapOpenAddressing : public IHashTable<Key, Value>
	{
	private:
		std::vector<Slot<Key, Value>> slots_;

		std::size_t size_ = 0;
		std::size_t deleted_count_ = 0;

		Hash hash_;
		KeyEqual equal_;

		static constexpr float max_load_factor_ = 0.7f;

	public:
		/// <summary>
		/// Создаёт хеш-таблицу с открытой адресацией заданной начальной вместимости
		/// </summary>
		/// <param name="capacity">Начальный размер таблицы, если передается 0 - ставим 1</param>
		/// <param name="hash">Хеш-функция, используемая для вычисления начального индекса ключа</param>
		/// <param name="equal">Функция сравнения ключей на равенство</param>
		explicit HashMapOpenAddressing(std::size_t capacity = 8, const Hash& hash = Hash{}, const KeyEqual& equal = KeyEqual{}) : 
			slots_(capacity > 0 ? capacity : 1), hash_(hash), equal_(equal) {}

		/// <summary>
		/// Вставляет новую пару ключ-значение в хеш-таблицу.
		/// Если элемент с указанным ключом уже существует, обновляет его значение.
		/// </summary>
		/// <param name="key">Ключ вставляемого или обновляемого элемента.</param>
		/// <param name="value">Значение, связанное с указанным ключом.</param>
		void emplace(const Key& key, const Value& value) override
		{

			float projected_occupancy =
				static_cast<float>(size_ + deleted_count_ + 1) /
				static_cast<float>(slots_.size());

			if (projected_occupancy > max_load_factor_)
			{
				rehash(slots_.size() * 2);
			}

			std::size_t hash_value = hash_(key);
			std::size_t start_index = hash_value % slots_.size();
			
			std::optional<std::size_t> deleted_index;

			for (std::size_t  k = 0; k < slots_.size(); k++)
			{
				std::size_t index = (start_index + k) % slots_.size();
				
				// если Occupied и ключ равен - обновляем значение и выходим
				if (slots_[index].state == SlotState::Occupied)
				{
					if (equal_(slots_[index].data->first, key))
					{
						slots_[index].data->second = value;
						return;
					}
					continue;
				}
				// тут просто запоминаем
				else if (slots_[index].state == SlotState::Deleted)
				{
					if (!deleted_index.has_value())
					{
						deleted_index = index;
					}
				}
				// и вставляем 
				else
				{
					if (deleted_index.has_value())
					{
						slots_[*deleted_index].data.emplace(key, value);
						slots_[*deleted_index].state = SlotState::Occupied;

						++size_;
						--deleted_count_;

						return;
					}
					else
					{
						slots_[index].data.emplace(key, value);
						slots_[index].state = SlotState::Occupied;

						++size_;
						return;
					}
				}
			}
			// если нет ни одного empty, а только Occupied и Deleted
			if (deleted_index.has_value())
			{
				std::size_t index = *deleted_index;

				slots_[index].data.emplace(key, value);
				slots_[index].state = SlotState::Occupied;

				++size_;
				--deleted_count_;

				return;
			}
			// все ячейки заняты, свободных и удалённых нет
			rehash(slots_.size() * 2);
			return emplace(key, value);
		}

		/// <summary>
		/// Удаляет элемент из хеш-таблицы по заданному ключу.
		/// Освобождает хранимую пару ключ-значение и помечает ячейку как удалённую.
		/// </summary>
		/// <param name="key">Ключ удаляемого элемента.</param>
		/// <returns>
		/// <see langword="true"/>, если элемент был найден и удалён;
		/// иначе <see langword="false"/>.
		/// </returns>
		bool erase(const Key& key) override
		{
			std::size_t capacity = slots_.size();
			std::size_t start_index = hash_(key) % capacity;
			
			for (std::size_t k = 0; k < capacity; k++)
			{
				std::size_t index = (start_index + k) % capacity;

				if (slots_[index].state == SlotState::Occupied)
				{
					if (equal_(slots_[index].data->first, key))
					{
						slots_[index].data.reset();
						slots_[index].state = SlotState::Deleted;

						--size_;
						++deleted_count_;
						return true;
					}
				}
				if (slots_[index].state == SlotState::Empty)
				{
					return false;
				}
			}
			return false;
		}

		/// <summary> Выполняет поиск элемента в хеш-таблице по заданному ключу. </summary>
		/// <param name="key">Ключ искомого элемента.</param>
		/// <param name="value"> Ссылка на переменную, в которую записывается найденное значение. Если ключ не найден, переменная не изменяется. </param>
		/// <returns>
		/// <see langword="true"/>, если элемент с указанным ключом найден;
		/// иначе <see langword="false"/>.
		/// </returns>
		bool find(const Key& key, Value& value) const override
		{
			std::size_t capacity = slots_.size();
			std::size_t start_index = hash_(key) % capacity;
			
			for (std::size_t k = 0; k < capacity; k++)
			{
				std::size_t index = (start_index + k) % capacity;
				if (slots_[index].state == SlotState::Occupied)
				{
					if (equal_(slots_[index].data->first, key))
					{
						value = slots_[index].data->second;
						return true;
					}
				}
				else if (slots_[index].state == SlotState::Empty)
				{
					return false;
				}
			}
			return false;
		}

		/// <summary>
		/// Возвращает количество элементов, хранящихся в хеш-таблице.
		/// </summary>
		std::size_t size() const noexcept override
		{
			return size_;
		}

		/// <summary>
		/// Перестраивает хеш-таблицу с заданной вместимостью.
		/// Повторно распределяет все занятые элементы с учётом нового размера
		/// и удаляет ячейки, помеченные состоянием <see cref="SlotState::Deleted"/>.
		/// Если новая вместимость меньше текущей, таблица не изменяется.
		/// </summary>
		/// <param name="new_capacity">
		/// Новая вместимость хеш-таблицы. Значение должно быть не меньше
		/// текущей вместимости таблицы.
		/// </param>
		void rehash(std::size_t new_capacity)
		{
			std::size_t old_capacity = slots_.size();

			if (new_capacity == 0 || new_capacity < old_capacity)
			{
				return;
			}
			
			std::vector<Slot<Key, Value>> new_slots(new_capacity);

			for (std::size_t old_index = 0; old_index < old_capacity; ++old_index)
			{
				if (slots_[old_index].state == SlotState::Occupied)
				{
					std::size_t start_index = hash_(slots_[old_index].data->first) % new_capacity;

					for (std::size_t probe = 0; probe < new_capacity; probe++)
					{
						std::size_t index =	(start_index + probe) % new_capacity;

						if (new_slots[index].state == SlotState::Empty)
						{
							new_slots[index].data.emplace(
								slots_[old_index].data->first,
								slots_[old_index].data->second);

							new_slots[index].state = SlotState::Occupied;
							break;
						}
					}
				}
			}

			slots_ = std::move(new_slots);
			deleted_count_ = 0;
		}

		/// <summary>
		/// Резервация ячеек в хеш-таблице
		/// </summary>
		/// <param name="expected_size">Резервируемый размер</param>
		void reserve(std::size_t expected_size)
		{
			std::size_t required_capacity =	static_cast<std::size_t>(static_cast<float>(expected_size) / max_load_factor_) + 1;

			if (required_capacity > slots_.size())
			{
				rehash(required_capacity);
			}
		}

		/// <summary>
		/// Возвращает коэффициент заполнения хеш-таблицы.
		/// </summary>
		/// <returns>
		/// Отношение количества хранимых элементов к вместимости таблицы.
		/// </returns>
		float load_factor() const noexcept
		{
			return static_cast<float>(size_) /
				static_cast<float>(slots_.size());
		}

		/// <summary>
		/// Возвращает коэффициент фактической занятости ячеек.
		/// </summary>
		/// <returns>
		/// Отношение количества занятых и удалённых ячеек
		/// к вместимости таблицы.
		/// </returns>
		float occupancy_factor() const noexcept
		{
			return static_cast<float>(size_ + deleted_count_) /
				static_cast<float>(slots_.size());
		}
	};
}