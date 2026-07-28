#pragma once

namespace ds
{
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

		virtual ~IHashTable() = default;
	};
}