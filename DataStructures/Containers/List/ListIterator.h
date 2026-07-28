#pragma once

#include <stdexcept>
#include "ListNode.h"

namespace ds
{
    template<typename T>
    class List;

    template<typename T>
    class ListIterator;

    template<typename T>
    class ListConstIterator;

    template<typename Pointer, typename Reference, typename Derived, typename T>
    class base_iterator
    {
        friend class List<T>;

        // разрешает разным специализациям base_iterator сравнивать свои указатели ptr.
        template<typename, typename, typename, typename>
        friend class base_iterator;

        // Нужен для преобразования:
        // ListIterator<T> -> ListConstIterator<T>
        friend class ListConstIterator<T>;

    protected:
        Pointer ptr;

    public:
        explicit base_iterator(Pointer ptr = nullptr) noexcept : ptr(ptr) {}

        /// <summary>
        /// Возвращает ссылку на элемент, на который указывает итератор.
        /// </summary>
        Reference operator*() const
        {
            if (!ptr)
            {
                throw std::runtime_error("Dereferencing null iterator");
            }

            return ptr->data;
        }

        /// <summary>
        /// Проверяет, равны ли два итератора.
        /// Поддерживает сравнение iterator и const_iterator.
        /// </summary>
        template<typename OtherPointer, typename OtherReference, typename OtherDerived>
        bool operator==(const base_iterator<OtherPointer, OtherReference, OtherDerived, T>& other) const noexcept
        {
            return ptr == other.ptr;
        }

        /// <summary>
        /// Проверяет, не равны ли два итератора.
        /// </summary>
        template<typename OtherPointer, typename OtherReference, typename OtherDerived>
        bool operator!=(const base_iterator<OtherPointer, OtherReference, OtherDerived, T>& other) const noexcept
        {
            return ptr != other.ptr;
        }

        /// <summary>
        /// Перемещает итератор на следующий элемент.
        /// </summary>
        Derived& operator++()
        {
            if (ptr)
            {
                ptr = ptr->next;
            }

            return static_cast<Derived&>(*this);
        }

        /// <summary>
        /// Возвращает старое состояние итератора,
        /// затем перемещает его на следующий элемент.
        /// </summary>
        Derived operator++(int)
        {
            Derived temp = static_cast<const Derived&>(*this);
            ++(*this);
            return temp;
        }

        /// <summary>
        /// Перемещает итератор на предыдущий элемент.
        /// </summary>
        Derived& operator--()
        {
            if (ptr)
            {
                ptr = ptr->prev;
            }

            return static_cast<Derived&>(*this);
        }

        /// <summary>
        /// Возвращает старое состояние итератора,
        /// затем перемещает его на предыдущий элемент.
        /// </summary>
        Derived operator--(int)
        {
            Derived temp = static_cast<const Derived&>(*this);
            --(*this);
            return temp;
        }
    };

    /// <summary>
    /// Двунаправленный итератор для контейнера List.
    /// Позволяет изменять элементы списка.
    /// </summary>
    template<typename T>
    class ListIterator : public base_iterator<NodeList<T>*, T&, ListIterator<T>, T>
    {
    private:
        using Base = base_iterator<NodeList<T>*, T&, ListIterator<T>, T>;

    public:
        using Base::Base;
    };

    /// <summary>
    /// Константный двунаправленный итератор для контейнера List.
    /// Не позволяет изменять элементы списка.
    /// </summary>
    template<typename T>
    class ListConstIterator : public base_iterator<const NodeList<T>*, const T&, ListConstIterator<T>, T>
    {
    private:
        using Base = base_iterator<const NodeList<T>*, const T&, ListConstIterator<T>, T>;

    public:
        using Base::Base;

        ListConstIterator() noexcept = default;

        /// <summary>
        /// Преобразует обычный итератор в константный.
        /// </summary>
        ListConstIterator(const ListIterator<T>& it) noexcept : Base(it.ptr) {}
    };
}