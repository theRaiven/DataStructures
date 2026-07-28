#pragma once

namespace ds
{
	/// <summary>
	/// Перечисление, представляющее доступные цвета.
	/// </summary>
	enum class Color
	{
		RED,
		BLACK
	};
	/// <summary>
	/// Узел красно?чёрного дерева, содержащий значение и ссылки на соседние узлы.
	/// </summary>
	/// <typeparam name="T">Тип хранимого значения в узле.</typeparam>
	template <class T>
	struct NodeRBT
	{
		Color color;
		T data;
		NodeRBT* left;
		NodeRBT* right;
		NodeRBT* parent;

		NodeRBT(const T& value) : data(value), left(nullptr), right(nullptr), parent(nullptr), color(Color::RED) {}
		~NodeRBT() = default;
	};
}