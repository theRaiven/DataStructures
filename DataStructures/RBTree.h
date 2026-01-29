#pragma once
#include <algorithm>
#include <utility>
#include <initializer_list>
#include <iostream>

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
    ~NodeRBT()
    {
		left = nullptr;
		right = nullptr;
		parent = nullptr;
		delete left;
		delete right;
		delete parent;
    }
};

/// <summary>
/// Шаблонный контейнер - реализация красно-черного дерева.
/// </summary>
/// <typeparam name="T">Тип элементов, хранимых в дереве.</typeparam>
template <class T>
class RBTree
{
private:
    NodeRBT<T>* root;
    size_t tree_size;

    /// <summary>
	/// Базовый класс итератора для обхода узлов красно-чёрного дерева.
    /// </summary>
    class iterator_base
    {
    protected:
        NodeRBT<T>* node;
        NodeRBT<T>* root;

        iterator_base(NodeRBT<T>* n, NodeRBT<T>* r) : node(n), root(r) { }

    public:
        iterator_base() : node(nullptr), root(nullptr) {}

        bool operator==(const iterator_base& other) const
        {
            return node == other.node;
        }

        bool operator!=(const iterator_base& other) const
        {
            return node != other.node;
        }

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
public:
    /// <summary>
    /// Итератор для обхода узлов красно?чёрного дерева.
    /// Выполняет обход in-order (возрастающий порядок)
    /// поддерживает разыменование, доступ к членам, сравнение и переходы (префиксные и постфиксные ++/--).
    /// </summary>
    class iterator : public iterator_base
    {
        friend class RBTree<T>;
    public:
        iterator() = default;
        iterator(NodeRBT<T>* n, NodeRBT<T>* r): iterator_base(n, r) { }

        T& operator*() const
        {
            return this->node->data;
        }
        T* operator->() const
        {
            return &this->node->data;
        }
        iterator& operator++()
        {
            this->increment();
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            this->increment();
            return tmp;
        }

        iterator& operator--()
        {
            this->decrement();
            return *this;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            this->decrement();
            return tmp;
        }        
    };
    /// <summary>
    /// Константный итератор для обхода узлов красно?чёрного дерева.
    /// Выполняет обход in-order (возрастающий порядок)
    /// поддерживает разыменование, доступ к членам, сравнение и переходы (префиксные и постфиксные ++/--).
    /// </summary>
	class const_iterator : public iterator_base
    {
        friend class RBTree<T>;
    public:
        const_iterator(NodeRBT<T>* n, NodeRBT<T>* r) : iterator_base(n, r) {}
        const_iterator() = default;
        const_iterator(const iterator& it): iterator_base(it.node, it.root) { }
        const T& operator*() const
        {
            return this->node->data;
        }
        const T* operator->() const
        {
            return &this->node->data;
        }
        const_iterator& operator++()
        {
            this->increment();
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            this->increment();
            return tmp;
        }

        const_iterator& operator--()
        {
            this->decrement();
            return *this;
        }

        const_iterator operator--(int)
        {
            const_iterator tmp = *this;
            this->decrement();
            return tmp;
        }
    };
    /// <summary>
    /// Конструктор по умолчанию класса RBTree. Создает пустое дерево.
    /// </summary>
    RBTree() : root(nullptr), tree_size(0) {}
    /// <summary>
    /// Конструктор копирования класса RBTree: создаёт новый объект и инициализирует его элементами другого дерева.
    /// </summary>
    /// <param name="other">Константная ссылка на исходное дерево RBTree, элементы которого копируются в новый объект.</param>
    RBTree(const RBTree& other) : root(nullptr), tree_size(0)
    {
        for (const auto& value : other)
            insert(value);
	}
	/// <summary>
	/// Перемещающий конструктор RBTree: переносит корень и размер из другого дерева и оставляет источник в пустом состоянии.
	/// </summary>
	/// <param name="other">rvalue-ссылка на объект RBTree, из которого переносится внутренняя структура. После выполнения конструктора исходное дерево обнуляется.</param>
	RBTree(RBTree&& other) noexcept : root(other.root), tree_size(other.tree_size)
    {
        other.root = nullptr;
        other.tree_size = 0;
    }
	/// <summary>
	/// Инициализирует RBTree и заполняет его элементами из списка инициализации.
	/// </summary>
	/// <param name="init_list">Список инициализации значений, которые будут поочерёдно вставлены в дерево.</param>
	RBTree(std::initializer_list<T> init_list) : root(nullptr), tree_size(0)
    {
        for (const auto& value : init_list)
            insert(value);
    }
    /// <summary>
    /// Создаёт и инициализирует объект RBTree, вставляя элементы из заданного диапазона итераторов.
    /// </summary>
    /// <typeparam name="It">Тип итератора. Ожидается, что *It даёт значение, совместимое с insert(), и итератор удовлетворяет требованиям InputIterator.</typeparam>
    /// <param name="first">Итератор на начало диапазона элементов для вставки (включительно).</param>
    /// <param name="last">Итератор на конец диапазона (не включая); вставка выполняется для элементов до этого итератора.</param>
    template<class It>
    RBTree(It first, It last) : root(nullptr), tree_size(0)
    {
        for (; first != last; ++first)
            insert(*first);
    }
    /// <summary>
    /// Деструктор класса RBTree. Освобождает ресурсы и удаляет все элементы, вызывая clear().
    /// </summary>
    ~RBTree()
    {
        clear();
    }

    /// <summary>
    /// Возвращает количество элементов в контейнере (размер дерева).
    /// </summary>
    /// <returns>Количество элементов (size_t) - текущее значение поля tree_size.</returns>
    size_t size() const
    {
        return tree_size;
    }

    /// <summary>
    /// Проверяет, пуста ли структура данных.
    /// </summary>
    /// <returns>true, если root равен nullptr (структура пуста); в противном случае — false.</returns>
    bool empty() const
    {
        return root == nullptr;
    }

    /// <summary>
    /// Рекурсивно освобождает память для указанного узла и всех его потомков (удаляет поддерево).
    /// </summary>
    /// <param name="node">Указатель на корень (или текущий узел) поддерева, которое нужно удалить.</param>
    void clear(NodeRBT<T>* node)
    {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }
    /// <summary>
    /// Очищает структуру дерева: удаляет все узлы, устанавливает root в nullptr и сбрасывает размер до 0.
    /// </summary>
    void clear()
    {
        clear(root);
        root = nullptr;
        tree_size = 0;
    }

    /// <summary>
    /// Проверяет, содержится ли заданное значение в контейнере.
    /// </summary>
    /// <param name="value">Значение для поиска в контейнере.</param>
    /// <returns>true, если значение найдено; в противном случае — false.</returns>
    bool contains(const T& value) const
    {
        return find(value) != cend();
    }

    /// <summary>
    /// Вставляет значение в дерево: создаёт новый узел, пытается вставить его как в BST, при успешной вставке восстанавливает свойства R-B и увеличивает размер дерева.
    /// </summary>
    /// <param name="value">Значение для вставки в дерево. Передаётся по константной ссылке и копируется в создаваемый узел.</param>
    void insert(const T& value)
    {
        NodeRBT<T>* new_node = new NodeRBT<T>(value);
        if (!bst_insert(new_node))
        {
            //delete new_node;
            return;
        }
        insert_fixup(new_node);
        tree_size++;
    }
    /// <summary>
    /// Удаляет узел с заданным значением из дерева, если таковой существует, балансирует дерево, уменьшает размер
    /// </summary>
    /// <param name="value">Удаляемое значение</param>
    /// <returns>true - значение удалено, false - не удалось удалить(пустое дерево/значения не существует)</returns>
    bool erase(const T& value)
    {
        NodeRBT<T>* delete_node = find_node(value);
        if (!delete_node) return false;

        erase_node(delete_node);
        tree_size--;
        return true;
    }

    /// <summary>
    /// Находит минимальный узел в поддереве, начиная с указанного узла.
    /// </summary>
    /// <param name="node">Указатель на корень поддерева, в котором выполняется поиск. Может быть nullptr.</param>
    /// <returns>Указатель на узел с минимальным ключом в данном поддереве, либо nullptr, если входной указатель равен nullptr.</returns>
    NodeRBT<T>* minimum(NodeRBT<T>* node)
    {
        if (!node) return nullptr;
        while (node->left)
            node = node->left;
        return node;
    }
    /// <summary>
    /// Находит максимальный узел в поддереве, начиная с указанного узла.
    /// </summary>
    /// <param name="node">Указатель на корень поддерева, в котором выполняется поиск. Может быть nullptr.</param>
    /// <returns>Указатель на узел с минимальным ключом в данном поддереве, либо nullptr, если входной указатель равен nullptr.</returns>
    NodeRBT<T>* maximum(NodeRBT<T>* node)
    {
        if (!node) return nullptr;
        while (node && node->right)
            node = node->right;
        return node;
    }

    /// <summary>
    /// Выполняет проверку корректности структуры дерева: наличие корня, цвет корня, свойства бинарного поиска, правила красно?черного дерева, корректная чёрная высота и корректность указателей parent.
    /// </summary>
    /// <returns>true, если все проверки пройдены и структура корректна; false в противном случае.</returns>
    bool validate() const
    {
        if (!root) return true;

        if (root->color != Color::BLACK)
            return false;

        if (!validate_bst(root, nullptr, nullptr))
            return false;

        if (!validate_red(root))
            return false;

        if (validate_black_height(root) == 0)
            return false;

        if (!validate_parent(root))
            return false;

        return true;
    }

    /// <summary>
    /// Ищет элемент в дереве (по свойствам бинарного поиска)
    /// </summary>
    /// <param name="value">Значение для поиска; передаётся по константной ссылке.</param>
    /// <returns>Итератор, указывающий на узел с совпадающим значением; если элемент отсутствует, возвращается end().</returns>
    iterator find(const T& value)
    {
        NodeRBT<T>* cur = root;
        while (cur)
        {
            if (value == cur->data)
                return iterator(cur, root);
            else if (value < cur->data)
                cur = cur->left;
            else
                cur = cur->right;
        }
        return end();
    }
    /// <summary>
    /// Ищет элемент в дереве (по свойствам бинарного поиска)
    /// </summary>
    /// <param name="value">Значение для поиска; передаётся по константной ссылке.</param>
    /// <returns>Константный итератор, указывающий на узел с совпадающим значением; если элемент отсутствует, возвращается end().</returns>
    const_iterator find(const T& value) const
    {
        NodeRBT<T>* cur = root;
        while (cur)
        {
            if (value == cur->data)
                return const_iterator(cur, root);
            else if (value < cur->data)
                cur = cur->left;
            else
                cur = cur->right;
        }
        return end();
    }

    /// <summary>
    /// Вставляет значение в красно?чёрное дерево: при успешной вставке возвращает итератор на новый узел и true, при наличии равного элемента — итератор на существующий элемент и false.
    /// </summary>
    /// <param name="value">Значение для вставки в дерево. Передаётся по константной ссылке и копируется в создаваемый узел.</param>
    /// <returns>std::pair<iterator, bool> — первый элемент пары: итератор на вставленный или уже существующий элемент; второй: true, если элемент был вставлен, false, если элемент уже существовал.</returns>
    std::pair<iterator, bool> insert_it(const T& value)
    {
        NodeRBT<T>* node = new NodeRBT<T>(value);
        if (!bst_insert(node))
        {
            delete node;
            return { find(value), false };
        }

        insert_fixup(node);
        ++tree_size;

        return { iterator(node, root), true };
    }
    /// <summary>
    /// Удаляет элемент, на который указывает итератор
    /// </summary>
    /// <param name="it">Итератор на удаляемый элемент. Если it равен end(), ничего не удаляется.</param>
    /// <returns>Итератор на элемент, следующий за удалённым. Если удалённый был последним элементом, возвращается end(). Если на вход подан end(), возвращается тот же итератор.</returns>
    iterator erase(iterator it)
    {
        if (it == end())
            return it;

        iterator next = it;
        ++next;

        erase_node(it.node);
        --tree_size;

        return next;
    }

    /// <summary>
    /// Возвращает итератор, указывающий на первый (наименьший) элемент дерева.
    /// </summary>
    /// <returns>Итератор на первый (наименьший) элемент контейнера; если контейнер пуст, возвращает end().</returns>
    iterator begin()
    {
		NodeRBT<T>* current = root;
        if (!current) return end();

        while (current->left)
			current = current->left;
		return iterator(current, root);
    }
    /// <summary>
    /// Возвращает итератор, соответствующий позиции конца контейнера (после последнего элемента).
    /// </summary>
    /// <returns>Итератор, указывающий на позицию после последнего элемента контейнера.</returns>
    iterator end()
    {
        return iterator(nullptr, root);
    }

    /// <summary>
    /// Константная перегрузка begin(), возвращающая const_iterator на первый элемент контейнера.
    /// </summary>
    /// <returns>Константный итератор на первый элемент контейнера; если контейнер пуст, возвращает итератор, равный cend().</returns>
    const_iterator begin() const { return cbegin(); }
    /// <summary>
    /// Возвращает константный итератор, указывающий на позицию после последнего элемента контейнера (эквивалентно cend()).
    /// </summary>
    /// <returns>Константный итератор (const_iterator), указывающий на past-the-end позицию контейнера.</returns>
    const_iterator end()   const { return cend(); }

    /// <summary>
    /// Возвращает константный итератор на первый (самый левый) элемент дерева.
    /// </summary>
    /// <returns>Константный итератор, указывающий на наименьший элемент (левый узел). Если дерево пусто — эквивалент cend().</returns>
    const_iterator cbegin() const
    {
        NodeRBT<T>* current = root;
        if (!current) return cend();

        while (current->left)
            current = current->left;
        return const_iterator(current, root);
    }
    /// <summary>
    /// Возвращает константный итератор, указывающий на положение конца контейнера (после последнего элемента).
    /// </summary>
    /// <returns>Константный итератор, указывающий на конечную позицию контейнера (one-past-the-end).</returns>
    const_iterator cend() const
    {
        return const_iterator(nullptr, root);
    }

    /// <summary>
    /// Поиск итератора, указывающего на первый элемент в дереве, который не меньше указанного значения (нижняя граница).
    /// </summary>
    /// <param name="value">Константная ссылка на значение, для которого ищется нижняя граница.</param>
    /// <returns>Итератор, указывающий на первый элемент, не меньший, чем value; или end(), если подходящего элемента нет.</returns>
    iterator lower_bound(const T& value)
    {
        NodeRBT<T>* current = root;
        NodeRBT<T>* candidate = nullptr;

        while (current)
        {
            if (!(current->data < value))
            {
                candidate = current;
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }

        return candidate ? iterator(candidate, root) : end();
    }
    /// <summary>
    /// Находит итератор на первый элемент, строго больший заданного значения.
    /// </summary>
    /// <param name="value">Значение для сравнения; ищется первый элемент дерева, который строго больше этого значения.</param>
    /// <returns>Итератор на первый элемент, значение которого строго больше value. Если такого элемента нет, возвращается end().</returns>
    iterator upper_bound(const T& value)
    {
        NodeRBT<T>* current = root;
        NodeRBT<T>* candidate = nullptr;

        while (current)
        {
            if (current->data > value)
            {
                candidate = current;
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }

        return candidate ? iterator(candidate, root) : end();
    }

    /// <summary>
    /// Нахождение диапазона элементов, равных заданному значению, используя lower_bound и upper_bound.
    /// </summary>
    /// <param name="value">Значение для поиска; по нему определяется диапазон равных элементов.</param>
    /// <returns>std::pair<iterator, iterator> — пара итераторов, задающая полуоткрытый диапазон [first, second) элементов, равных value (первый — нижняя граница, второй — верхняя граница).</returns>
    std::pair<iterator, iterator> equal_range(const T& value)
    {
        return {
            lower_bound(value),
            upper_bound(value)
        };
    }

    /// <summary>
	/// Метод замены содержимого этого дерева с другим деревом: обменивает корни и размеры двух деревьев.
    /// </summary>
    /// <param name="other">Заменяемое дерево</param>
    void swap(RBTree& other) noexcept
    {
        std::swap(root, other.root);
        std::swap(tree_size, other.tree_size);
    }

    /// <summary>
    /// Выводит представление дерева в std::cout. Если дерево пустое, выводит <empty tree>\n; в противном случае вызывает внутреннюю функцию print_impl для рекурсивного форматированного вывода.
    /// </summary>
    void print_tree() const
    {
        if (!root)
        {
            std::cout << "<empty tree>\n";
            return;
        }
        print_impl(root, "", true);
    }
    /// <summary>
    /// Итерирует от cbegin() до cend() и выводит каждое значение в std::cout, за ним следует пробел.
    /// </summary>
    void print()
    {
        for (auto it = cbegin(); it != cend(); ++it)
            std::cout << *it << " ";
    }

    /// <summary>
    /// Оператор копирующего присваивания для RBTree. При самоприсваивании ничего не делает; в противном случае очищает текущее дерево и копирует элементы из other.
    /// </summary>
    /// <param name="other">Другое дерево RBTree, из которого копируются элементы.</param>
    /// <returns>Ссылка на текущий объект (обычно *this).</returns>
    RBTree& operator=(const RBTree& other)
    {
        if (this == &other) return *this;
        clear();
        for (const auto& v : other)
            insert(v);
        return *this;
    }
    /// <summary>
    /// Оператор перемещающего присваивания для RBTree: освобождает текущие ресурсы, переносит корень и размер из other в этот объект и оставляет other в пустом валидном состоянии.
    /// </summary>
    /// <param name="other">Источник (rvalue-ссылка) RBTree, содержимое которого будет перемещено. После операции other становится пустым (root == nullptr, tree_size == 0).</param>
    /// <returns>Ссылка на этот объект (RBTree&), позволяющая цепочку присваиваний.</returns>
    RBTree& operator=(RBTree&& other) noexcept
    {
        if (this == &other) return *this;
        clear();
        root = other.root;
        tree_size = other.tree_size;
        other.root = nullptr;
        other.tree_size = 0;
        return *this;
    }
    /// <summary>
    /// Сравнивает текущее RBTree с другим на равенство: сначала проверяет равенство размеров, затем поэлементно сравнивает последовательности элементов.
    /// </summary>
    /// <param name="other">Другое RBTree для сравнения (константная ссылка).</param>
    /// <returns>true, если деревья имеют одинаковый размер и все соответствующие элементы равны; в противном случае false.</returns>
    bool operator==(const RBTree& other) const
    {
        if (size() != other.size()) return false;
        return std::equal(begin(), end(), other.begin());
    }

private:
    /// <summary>
    /// Рекурсивно проверяет, удовлетворяет ли поддерево, заданное node, свойствам бинарного дерева поиска с учётом верхней и нижней границ.
    /// </summary>
    /// <param name="node">Указатель на текущий узел (поддерево) для проверки. Если nullptr, считается корректным.</param>
    /// <param name="min">Указатель на нижнюю границу (исключительно). Если не nullptr, все значения в поддереве должны быть больше *min.</param>
    /// <param name="max">Указатель на верхнюю границу (исключительно). Если не nullptr, все значения в поддереве должны быть меньше *max.</param>
    /// <returns>true, если поддерево корректно как бинарное дерево поиска с заданными ограничениями; иначе false.</returns>
    bool validate_bst(NodeRBT<T>* node, const T* min, const T* max) const
    {
        if (!node) return true;
        if ((min && node->data <= *min) ||
            (max && node->data >= *max))
            return false;
        return validate_bst(node->left, min, &node->data) &&
            validate_bst(node->right, &node->data, max);
    }
    /// <summary>
    /// Проверяет свойство красных узлов в поддереве: у красного узла не должно быть красных дочерних узлов.
    /// </summary>
    /// <param name="node">Указатель на проверяемый узел (корень поддерева). Если node равен nullptr, считается корректным (возвращается true).</param>
    /// <returns>true, если для данного узла и всех его потомков выполняется правило (нет двух подряд красных узлов); в противном случае false.</returns>
    bool validate_red(NodeRBT<T>* node) const
    {
        if (!node) return true;
        if (node->color == Color::RED)
        {
            if ((node->left && node->left->color == Color::RED) ||
                (node->right && node->right->color == Color::RED))
                return false;
        }
        return validate_red(node->left) &&
            validate_red(node->right);
    }
    /// <summary>
    /// Рекурсивно проверяет свойство черной высоты для поддерева: если в обоих детях одинаковая черная высота, возвращает эту высоту (с учётом цвета текущего узла), иначе возвращает 0.
    /// </summary>
    /// <param name="node">Указатель на проверяемый узел (корень поддерева). Если node == nullptr, функция рассматривает это как лист и возвращает базовую высоту (код возвращает 1 для пустого узла).</param>
    /// <returns>0 — при обнаружении нарушения; положительное число — корректная черная высота поддерева (количество черных узлов на любом пути до листа, включая текущий узел, если он чёрный).</returns>
    int validate_black_height(NodeRBT<T>* node) const
    {
		if (!node) return 1;

		int l = validate_black_height(node->left);
		int r = validate_black_height(node->right);
        if (l == 0 || r == 0 || l != r) return 0;

		return l + (node->color == Color::BLACK ? 1 : 0);
    }
    /// <summary>
	/// Проверка корректности указателей parent в поддереве: для каждого узла проверяет, что его дети ссылаются на него как на родителя.
    /// </summary>
    /// <param name="node">Проверяемое поддерево. node - корень этого поддерева</param>
    /// <returns>true - если все ссылки корректны, false - в противном случае</returns>
    bool validate_parent(NodeRBT<T>* node) const
    {
        if (!node) return true;

        if (node->left && node->left->parent != node)
            return false;
        if (node->right && node->right->parent != node)
            return false;

        return validate_parent(node->left) &&
            validate_parent(node->right);
    }

    /// <summary>
    /// Рекурсивно выводит структуру красно-чёрного дерева в консоль
    /// </summary>
    /// <param name="node">Текущий узел для печати.</param>
    /// <param name="prefix"> Строка-отступ, формирующая визуальную иерархию дерева. </param>
    /// <param name="is_tail"> Флаг, указывающий является ли текущий узел последним потомком </param>
    /// <remarks>
    /// Используется только для отладочного вывода.
    /// Не влияет на структуру дерева.
    /// </remarks>
    void print_impl(NodeRBT<T>* node, const std::string& prefix, bool is_tail) const
    {
        if (!node) return;

        std::cout << prefix
                  << (is_tail ? "\\-- " : "/-- ")
                  << node->data
                  << (node->color == Color::RED ? "(R)" : "(B)")
                  << "\n";

        // есть ли правый ребенок — влияет на вертикальную линию
        bool has_right = (node->right != nullptr);

        if (node->left)
        {
            print_impl(
                node->left,
                prefix + (is_tail ? "    " : "|   "),
                !has_right
            );
        }

        if (node->right)
        {
            print_impl(
                node->right,
                prefix + (is_tail ? "    " : "|   "),
                true
            );
        }
    }
    /// <summary>
    /// Выполняет левый поворот вокруг заданного узла.
    /// </summary>
    /// <param name="x"> Узел, относительно которого выполняется поворот. </param>
    /// <remarks>
    /// Используется в операциях балансировки красно-чёрного дерева.
    /// Предполагается, что x->right != nullptr.
    /// </remarks>
    void left_rotate(NodeRBT<T>* x)
    {
        NodeRBT<T>* y = x->right;
		x->right = y->left;
		y->parent = x->parent;

        if (!x->parent) { root = y; }
        else if (x == x->parent->left) { x->parent->left = y; }
        else { x->parent->right = y; }

        if(y->left)
        {
            y->left->parent = x;
        }
		y->left = x;
		x->parent = y;
    }
    /// <summary>
    /// Выполняет правый поворот вокруг заданного узла.
    /// </summary>
    /// <param name="x"> Узел, относительно которого выполняется поворот. </param>
    /// <remarks>
    /// Используется в операциях балансировки красно-чёрного дерева.
    /// Предполагается, что x->left != nullptr.
    /// </remarks>
    void right_rotate(NodeRBT<T>* x)
    {
        NodeRBT<T>* y = x->left;
        x->left = y->right;
        y->parent = x->parent;

        if (!x->parent) { root = y; }
        else if (x == x->parent->left) { x->parent->left = y; }
        else { x->parent->right = y; }

        if (y->right)
        {
            y->right->parent = x;
        }
        y->right = x;
        x->parent = y;
    }

    /// <summary>
    /// Выполняет бинарную вставку узла в дерево без учёта правил балансировки.
    /// </summary>
    /// <param name="new_node">Узел для вставки.</param>
    /// <returns> true — если вставка выполнена успешно; false — если элемент с таким ключом уже существует. </returns>
    /// <remarks>
    /// Используется как первый этап вставки в красно-чёрное дерево.
    /// Балансировка выполняется отдельно.
    /// </remarks>
    bool bst_insert(NodeRBT<T>* new_node)
    {
		if (!root)
        {
            root = new_node;
            return true;
        }
		NodeRBT<T>* current = root;
        while (current)
        {
            if(new_node->data < current->data)
            {
                if (!current->left)
                {
                    current->left = new_node;
                    new_node->parent = current;
                    return true;
                }
                current = current->left;
            }
            else if (new_node->data == current->data)
            {
                delete new_node;
				return false;
            }
            else
            {
                if (!current->right)
                {
                    current->right = new_node;
                    new_node->parent = current;
                    return true;
                }
                current = current->right;
			}
        }
        return false;
    }
    /// <summary> 
    /// Восстанавливает свойства красно-чёрного дерева после вставки узла.
    /// </summary>
    /// <param name="z">Вставленный узел.</param>
    /// <remarks>
    /// Реализует стандартный алгоритм fix-up для RB-tree, включая перекраску и повороты.
    /// </remarks>
    void insert_fixup(NodeRBT<T>* z)
    {
        while (z != root && z->parent->color == Color::RED)
        {
			// сделаю условное обозначение для родителя и "дяди"
			auto parent = z->parent;
			auto grandparent = parent->parent;
			auto uncle = (parent == grandparent->left) ? grandparent->right : grandparent->left;

            if (uncle && uncle->color == Color::RED)
            {
				grandparent->color = Color::RED;
				parent->color = Color::BLACK;
				uncle->color = Color::BLACK;
				z = grandparent;
            }
            else
            {
                if (z == parent->right && parent == grandparent->left)
                {
                    left_rotate(parent);
                    z = parent;
					parent = z->parent;
                }
                else if (z == parent->left && parent == grandparent->right)
                {
                    right_rotate(parent);
                    z = parent;
                    parent = z->parent;
                }
                else if (z == parent->left && parent == grandparent->left)
                {
                    right_rotate(grandparent);
                    std::swap(parent->color, grandparent->color);
                    z = parent;
                    break;
                }
                else if (z == parent->right && parent == grandparent->right)
                {
                    left_rotate(grandparent);
                    std::swap(parent->color, grandparent->color);
                    z = parent;
                    break;
                }
            }
        }
        root->color = Color::BLACK;
    }

    /// <summary>
    /// Удаляет указанный узел из красно-чёрного дерева.
    /// </summary>
    /// <param name="z">Узел для удаления.</param>
    /// <remarks>
    /// После физического удаления узла при необходимости выполняется восстановление свойств дерева.
    /// </remarks>
    void erase_node(NodeRBT<T>* z)
    {
        NodeRBT<T>* x = nullptr;
        NodeRBT<T>* x_parent = nullptr;
        
        auto removed_color = z->color;

        if (!z->left && !z->right)
        {
            x = nullptr;
            x_parent = z->parent;
            transplant(z, nullptr);
            delete z;
        }
        else if (!z->left || !z->right)
        {
            x = z->left ? z->left : z->right;
            transplant(z, x);
            x_parent = x->parent;
            delete z;
        }
        else
        {
            NodeRBT<T>* y = minimum(z->right);
            removed_color = y->color;

            x = y->right;
            x_parent = nullptr;

            if (y->parent == z)
            {
                x_parent = y;
            }
            else
            {
                transplant(y, y->right);
                x_parent = y->parent;
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;

            delete z;
        }
        
        if (removed_color == Color::BLACK)
        {
            erase_fixup(x, x_parent);
        }
    }
    /// <summary>
    /// Восстанавливает свойства красно-чёрного дерева после удаления узла.
    /// </summary>
    /// <param name="x"> Узел, занявший место удалённого (может быть nullptr). </param>
    /// <param name="parent"> Родитель узла x, используется при x == nullptr. </param>
    void erase_fixup(NodeRBT<T>* x, NodeRBT<T>* parent)
    {
        auto color = [](NodeRBT<T>* n)
        {
            return n ? n->color : Color::BLACK;
        };

        while (x != root && (x == nullptr || x->color == Color::BLACK))
        {
            parent = (x ? x->parent : parent);

            if (!parent) return;
            bool isLeft = ( parent->left == x );
            auto brother = isLeft ? parent->right : parent->left;
            
            if (!brother)
            {
                x = parent;
                continue;
            }

            if (color(brother) == Color::RED)
            {
                brother->color = Color::BLACK;
                parent->color = Color::RED;

                if (isLeft) left_rotate(parent);
                else        right_rotate(parent);


                brother = (isLeft) ? parent->right : parent->left;
            }

            if (color(brother->left) == Color::BLACK && 
                color(brother->right) == Color::BLACK)
            {
                brother->color = Color::RED;
                x = parent;
            }
            else
            {
                if (isLeft)
                {
                    if (color(brother->right) == Color::BLACK)
                    {
                        if (brother->left) brother->left->color = Color::BLACK;
                        brother->color = Color::RED;
                        right_rotate(brother);
                        brother = parent->right;
                    }

                    brother->color = parent->color;
                    parent->color = Color::BLACK;
                    if (brother->right) brother->right->color = Color::BLACK;
                    left_rotate(parent);
                }
                else
                {
                    if (color(brother->left) == Color::BLACK)
                    {
                        if (brother->right) brother->right->color = Color::BLACK;
                        brother->color = Color::RED;
                        left_rotate(brother);
                        brother = parent->left;
                    }

                    brother->color = parent->color;
                    parent->color = Color::BLACK;
                    if (brother->left) brother->left->color = Color::BLACK;
                    right_rotate(parent);
                }
				x = root;
            }
        }
        if (x) x->color = Color::BLACK;
    }
    /// <summary>
    /// Ищет узел с заданным значением в дереве.
    /// </summary>
    /// <param name="value">Значение для поиска.</param>
    /// <returns> Указатель на найденный узел или nullptr, если элемент отсутствует. </returns>
    NodeRBT<T>* find_node(const T& value) const
    {
		NodeRBT<T>* current = root;
        while (current)
        {
            if (current->data == value)
            {
                return current;
            }
            else if (current->data < value)
            {
				current = current->right;
            }
            else if (current->data > value)
            {
                current = current->left;
            }
        }

        return nullptr;
    }
    /// <summary>
    /// Заменяет одно поддерево другим.
    /// </summary>
    /// <param name="u">Корень заменяемого поддерева.</param>
    /// <param name="v">Корень нового поддерева.</param>
    /// <remarks>
    /// Вспомогательная функция, используемая при удалении узлов.
    /// Корректно обновляет связи с родителем.
    /// </remarks>>
    void transplant(NodeRBT<T>* u, NodeRBT<T>* v)
    {
        if (!u->parent)
        {
            root = v;
        }
        else if (u == u->parent->left)
        {
            u->parent->left = v;
        }
        else
        {
            u->parent->right = v;
        }

        if (v)
        {
            v->parent = u->parent;
        }
    }
};
