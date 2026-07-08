#pragma once

/// <summary>
/// Структура узла Node для двусвязного списка, содержащая данные и указатели на следующий и предыдущий узлы.
/// </summary>
/// <typeparam name="T">Тип данных узла</typeparam>
template <typename T>
struct NodeList
{
	T data;
	NodeList* next;
	NodeList* prev;
	NodeList(const T& value) : data(value), next(nullptr), prev(nullptr) {}
	~NodeList() = default;
};