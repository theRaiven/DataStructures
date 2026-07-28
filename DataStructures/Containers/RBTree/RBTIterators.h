#pragma once
#include <stdexcept>
#include "RBTNode.h"

namespace ds
{
    template <class T>
    class RBTree;

    /// <summary>
    /// Базовый класс итератора для обхода узлов красно-чёрного дерева.
    /// </summary>
    template <class T>
    class iterator_base
    {
    protected:
        NodeRBT<T>* node;
        NodeRBT<T>* root;

        iterator_base(NodeRBT<T>* n, NodeRBT<T>* r) : node(n), root(r) {}

    public:
        iterator_base() : node(nullptr), root(nullptr) {}
        /// <summary>
        /// Проверяет, равны ли два итератора.
        /// </summary>
        /// <param name="other">Итератор, с которым происходит сравнение.</param>
        /// <returns>True, если итераторы равны, иначе False.</returns>
        bool operator==(const iterator_base<T>& other) const
        {
            return node == other.node;
        }
        /// <summary>
        /// Проверяет, не равны ли два итератора.
        /// </summary>
        /// <param name="other">Итератор, с которым происходит сравнение.</param>
        /// <returns>True, если итераторы не равны, иначе False.</returns>
        bool operator!=(const iterator_base<T>& other) const
        {
            return node != other.node;
        }
        /// <summary>
        /// Переходит к следующему элементу.
        /// </summary>
        /// <returns>Ссылка на итератор.</returns>
        iterator_base& increment()
        {
            if (!node) return *this;

            if (node->right)
            {
                node = node->right;
                while (node->left)
                    node = node->left;
            }
            else
            {
                NodeRBT<T>* parent = node->parent;
                while (parent && node == parent->right)
                {
                    node = parent;
                    parent = parent->parent;
                }
                node = parent;
            }
            return *this;
        }
        /// <summary>
        /// Переходит к предыдущему элементу.
        /// </summary>
        /// <returns>Ссылка на итератор.</returns>
        iterator_base& decrement()
        {
            if (!node)
            {
                node = root;
                while (node && node->right)
                    node = node->right;
                return *this;
            }

            if (node->left)
            {
                node = node->left;
                while (node->right)
                    node = node->right;
            }
            else
            {
                NodeRBT<T>* parent = node->parent;
                while (parent && node == parent->left)
                {
                    node = parent;
                    parent = parent->parent;
                }
                node = parent;
            }
            return *this;
        }
    };

    /// <summary>
    /// Итератор для обхода узлов красно?чёрного дерева.
    /// Выполняет обход in-order (возрастающий порядок)
    /// поддерживает разыменование, доступ к членам, сравнение и переходы (префиксные и постфиксные ++/--).
    /// </summary>
    template <class T>
    class RBTiterator : public iterator_base<T>
    {
        friend class RBTree<T>;
    public:
        RBTiterator() = default;
        RBTiterator(NodeRBT<T>* n, NodeRBT<T>* r) : iterator_base<T>(n, r) {}

        T& operator*() const
        {
            return this->node->data;
        }
        T* operator->() const
        {
            return &this->node->data;
        }
        RBTiterator& operator++()
        {
            this->increment();
            return *this;
        }

        RBTiterator operator++(int)
        {
            RBTiterator tmp = *this;
            this->increment();
            return tmp;
        }

        RBTiterator& operator--()
        {
            this->decrement();
            return *this;
        }

        RBTiterator operator--(int)
        {
            RBTiterator tmp = *this;
            this->decrement();
            return tmp;
        }
    };
    /// <summary>
    /// Константный итератор для обхода узлов красно?чёрного дерева.
    /// Выполняет обход in-order (возрастающий порядок)
    /// поддерживает разыменование, доступ к членам, сравнение и переходы (префиксные и постфиксные ++/--).
    /// </summary>
    template <class T>
    class RBTconst_iterator : public iterator_base<T>
    {
        friend class RBTree<T>;
    public:
        RBTconst_iterator(NodeRBT<T>* n, NodeRBT<T>* r) : iterator_base<T>(n, r) {}
        RBTconst_iterator() = default;
        RBTconst_iterator(const RBTiterator<T>& it) : iterator_base<T>(it.node, it.root) {}

        const T& operator*() const
        {
            return this->node->data;
        }
        const T* operator->() const
        {
            return &this->node->data;
        }
        RBTconst_iterator& operator++()
        {
            this->increment();
            return *this;
        }

        RBTconst_iterator operator++(int)
        {
            RBTconst_iterator tmp = *this;
            this->increment();
            return tmp;
        }

        RBTconst_iterator& operator--()
        {
            this->decrement();
            return *this;
        }

        RBTconst_iterator operator--(int)
        {
            RBTconst_iterator tmp = *this;
            this->decrement();
            return tmp;
        }
    };
}