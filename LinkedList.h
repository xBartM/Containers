#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template <typename Type>
class LinkedList
{

public:
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using value_type = Type;
    using pointer = Type*;
    using reference = Type&;
    using const_pointer = const Type*;
    using const_reference = const Type&;

    class ConstIterator;
    class Iterator;
    using iterator = Iterator;
    using const_iterator = ConstIterator;

    int nr_occu;          /* ilosc okupowanych miejsc */
    struct Node           /* wezel */
    {
        value_type data;
        Node *next;
        Node *prev;
    } *node, *root, *lastest;   /* lastest - za ostatnim, pilnuje porzadku */


    LinkedList()
        :   nr_occu(0)
        ,   node(new Node)
        ,   root(node)
        ,   lastest(node)
    {
        node->next = nullptr;
        node->prev = nullptr;
    }

    LinkedList(std::initializer_list<Type> l)
        :   nr_occu(0)
        ,   node(new Node)
        ,   root(node)
        ,   lastest(node)
    {
        node->next = nullptr;
        node->prev = nullptr;
        const value_type *tmp;
        tmp = l.begin();
        for (int i=0; i < l.size(); i++)
        {
            append(*tmp);
            tmp++;
        }
        return;
    }

    LinkedList(const LinkedList& other)
        :   nr_occu(0)
        ,   node(new Node)
        ,   root(node)
        ,   lastest(node)
    {
        node->next = nullptr;
        node->prev = nullptr;
        ConstIterator tmp;
        tmp = other.begin();
        for (int i = 0; i < other.nr_occu; i++)
        {
            append((tmp.iter)->data);
            tmp++;
        }
        return;
    }

    LinkedList(LinkedList&& other)
        :   nr_occu(other.nr_occu)
        ,   node(new Node)
    {
        node->next = nullptr;
        node->prev = nullptr;
        other.nr_occu = 0;
        root = other.root;
        other.root = node;
        lastest = other.lastest;
        other.lastest = node;
        other.node = node;
        node = root;
        return;
    }

    ~LinkedList()
    {
        erase(begin(), end());
        delete root;
    }

    LinkedList& operator=(const LinkedList& other)
    {
        erase (begin(), end());
        ConstIterator tmp;
        tmp = other.begin();
        for (int i = 0; i < other.nr_occu; i++)
        {
            append((tmp.iter)->data);
            tmp++;
        }
        return *this;
    }

    LinkedList& operator=(LinkedList&& other)
    {
        erase(begin(), end());
        this->root = new Node;
        this->lastest = this->node = this->root;
        nr_occu = other.nr_occu;
        other.nr_occu = 0;
        this->root = other.root;
        other.root = this->node;
        this->lastest = other.lastest;
        other.lastest = other.root;
        other.node = other.root;
        other.root->next = other.root->prev = nullptr;
        return *this;
    }

    bool isEmpty() const
    {
        return !(bool)nr_occu;
    }

    size_type getSize() const
    {
        return nr_occu;
    }

    void append(const Type& item)
    {
        node = lastest;
        node->next = new Node;
        node->data = item;
        lastest = node->next;
        lastest->next = nullptr;
        lastest->prev = node;
        nr_occu++;
        return;
    }

    void prepend(const Type& item)
    {
        node = new Node;
        root->prev = node;
        node->next = root;
        root = node;
        root->prev = nullptr;
        root->data = item;
        nr_occu++;
        return;
    }

    void insert(const const_iterator& insertPosition, const Type& item)
    {
        if (insertPosition.iter == root)
            prepend(item);
        else if (insertPosition.iter == lastest)    /* else, bo inaczej dla pustej kolekcji wstawia sie 2 itemy */
            append(item);
        else
        {
            Node *tmp;
            node = root;
            while (insertPosition.iter != node->next)
                node = node->next;
            tmp = node->next;
            node->next = new Node;
            node = node->next;
            node->next = tmp;
            node->prev = tmp->prev;
            tmp->prev = node;
            node->data = item;
            nr_occu++;
        }
        return;
    }

    Type popFirst()
    {
        if (nr_occu)
        {
            Type tmp;
            tmp = root->data;
            node = root;
            root = root->next;
            root->prev = nullptr;
            delete node;
            nr_occu--;
            return tmp;
        }
        else
            throw std::logic_error("popFirst() on empty collection");
    }

    Type popLast()
    {
        if (nr_occu)
        {

            Type tmp;
            node = lastest;
            lastest = root;
            while (lastest->next != node)
                lastest = lastest-> next;
            tmp = lastest->data;
            delete node;
            lastest->next = nullptr;
            nr_occu--;

            return tmp;
        }
        else
            throw std::logic_error("popLast() on empty collection");
    }

    void erase(const const_iterator& possition)
    {
        if (nr_occu)
        {
            if (possition == end())
                throw std::out_of_range ("erasing from beyond range");
            if (possition == begin())
                popFirst();
            else if (possition.iter->next == end().iter)
                popLast();
            else
            {
                Node *tmp;
                node = root;
                while (node->next != possition.iter)
                    node = node->next;
                tmp = node->next;            //tmp jest takie jak possition.iter
                node->next = tmp->next;
                delete tmp;
                tmp = node->next;
                tmp->prev = node;
                nr_occu--;
            }
            return;
        }
        else
            throw std::out_of_range ("erasing from empty collection");
    }

    void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
    {
        ConstIterator tmp;
        tmp = firstIncluded;
        while (tmp != lastExcluded)
        {
            erase(tmp++);

        }
        return;
    }

    iterator begin()
    {
        iterator tmp;
        tmp.iter = root;
        tmp.ilis = this;
        return tmp;
    }

    iterator end()
    {
        iterator tmp;
        tmp.iter = lastest;
        tmp.ilis = this;
        return tmp;
    }

    const_iterator cbegin() const
    {
        const_iterator tmp;
        tmp.iter = root;
        tmp.ilis = this;
        return tmp;
    }

    const_iterator cend() const
    {
        const_iterator tmp;
        tmp.iter = lastest;
        tmp.ilis = this;
        return tmp;
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }
};

template <typename Type>
class LinkedList<Type>::ConstIterator
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename LinkedList::value_type;
    using difference_type = typename LinkedList::difference_type;
    using pointer = typename LinkedList::const_pointer;
    using reference = typename LinkedList::const_reference;

    const Node *iter;
    const LinkedList *ilis;

    explicit ConstIterator()
    {}

    reference operator*() const
    {
        const Node *tmp;
        const value_type *tmpData;
        tmp = (ilis->end()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer dereferenced at the end()");
        tmpData = &(iter->data);
        return *(tmpData);
    }

    ConstIterator& operator++()
    {
        const Node *tmp;
        tmp = (ilis->end()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer past the end()");
        iter = iter->next;
        return *this;
    }

    ConstIterator operator++(int)
    {
        const Node *tmp;
        tmp = (ilis->end()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer past the end()");
        ConstIterator tmp2;
        tmp2.iter = this->iter;
        this->iter = iter->next;
        return tmp2;
    }

    ConstIterator& operator--() /* pre */
    {
        const Node *tmp;
        tmp = (ilis->begin()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer before the begin()");
        iter = iter->prev;
        return *this;
    }

    ConstIterator operator--(int)
    {
        const Node *tmp;
        tmp = (ilis->begin()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer before the begin()");
        ConstIterator tmp2;
        tmp2.iter = this->iter;
        iter = iter->prev;
        return tmp2;
    }

    ConstIterator operator+(difference_type d) const
    {
        const Node *tmp;
        ConstIterator tmp2;
        tmp = (ilis->end()).iter;
        tmp2.iter = this->iter;
        for (int i = 0; i < d; i++)
        {
            if (tmp2.iter == tmp)
                throw std::out_of_range("Pointer past the end()");
            tmp2.iter = (tmp2.iter)->next;
        }
        return tmp2;
    }

    ConstIterator operator-(difference_type d) const
    {
        const Node *tmp;
        ConstIterator tmp2;
        ConstIterator tmp3;
        tmp3.iter = ilis->root;
        tmp = (ilis->begin()).iter;
        tmp2.iter = this->iter;
        for (int i = 0; i < d; i++)
        {
            if (tmp2.iter == tmp)
                throw std::out_of_range("Pointer before the begin()");
            tmp2.iter = (tmp2.iter)->prev;
        }
        return tmp2;
    }

    bool operator==(const ConstIterator& other) const
    {
        return (iter == other.iter);
    }

    bool operator!=(const ConstIterator& other) const
    {
        return (iter != other.iter);
    }
};

template <typename Type>
class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator
{
public:
    using pointer = typename LinkedList::pointer;
    using reference = typename LinkedList::reference;


    explicit Iterator()
    {}

    Iterator(const ConstIterator& other)
        : ConstIterator(other)
    {}

    Iterator& operator++()
    {
        ConstIterator::operator++();
        return *this;
    }

    Iterator operator++(int)
    {
        auto result = *this;
        ConstIterator::operator++();
        return result;
    }

    Iterator& operator--()
    {
        ConstIterator::operator--();
        return *this;
    }

    Iterator operator--(int)
    {
        auto result = *this;
        ConstIterator::operator--();
        return result;
    }

    Iterator operator+(difference_type d) const
    {
        return ConstIterator::operator+(d);
    }

    Iterator operator-(difference_type d) const
    {
        return ConstIterator::operator-(d);
    }

    reference operator*() const
    {
        // ugly cast, yet reduces code duplication.
        return const_cast<reference>(ConstIterator::operator*());
    }
};

}

#endif // AISDI_LINEAR_LINKEDLIST_H
