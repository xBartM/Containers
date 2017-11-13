#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>


namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
{
public:
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const key_type, mapped_type>;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    class ConstIterator;
    class Iterator;
    using iterator = Iterator;
    using const_iterator = ConstIterator;


    struct Node
    {
        std::pair<const KeyType, ValueType> *data;

        Node *parent;
        Node *left;
        Node *right;

        Node()
        {}

        Node(KeyType f, ValueType s = {})
        {
            data    = new std::pair<const KeyType, ValueType>(f, s);
            parent  = nullptr;
            left    = nullptr;
            right   = nullptr;
        }
    };

    Node *root, *last, *curr;
    int nr_occu;                    /* ilosc elementow */

    Node * copyHelper(Node *toCopy)
    {
        if (!toCopy)
            return nullptr;
        Node *copyNode = new Node(toCopy->data->first, toCopy->data->second);
        copyNode->left = copyHelper(toCopy->left);
        copyNode->right = copyHelper(toCopy->right);
        return copyNode;
    }

    void linkParents(Node *toLink)
    {
        if(toLink->left)
        {
            toLink->left->parent = toLink;
            linkParents(toLink->left);
        }
        if(toLink->right)
        {
            toLink->right->parent = toLink;
            linkParents(toLink->right);
        }
    }

    void deleteHelper(Node *toDelete)
    {
        if(toDelete->left)
            deleteHelper(toDelete->left);
        if(toDelete->right)
            deleteHelper(toDelete->right);
        delete toDelete->data;
        delete toDelete;
    }

    TreeMap()
        :   root(new Node({}))
    ,   nr_occu(0)
    {
        root->parent        = nullptr;
        root->left          = nullptr;
        root->right         = nullptr;

        last = curr = root;
    }

    TreeMap(std::initializer_list<value_type> list)
        :   TreeMap()
    {
        auto temp = list.begin();
        for (int i=0; i < list.size(); i++)
        {
            (*this)[std::get<0>(*temp)] = std::get<1>(*temp);
            temp++;
        }
    }

    TreeMap(const TreeMap& other)
        :   nr_occu(other.nr_occu)
    {
        root = curr = copyHelper(other.root);
        linkParents(root);
        root->parent = nullptr;
        while (curr->right)
            curr = curr->right;
        last = curr;
    }

    TreeMap(TreeMap&& other)
        :   curr(new Node({}))
    {
        curr->parent        = nullptr;
        curr->left          = nullptr;
        curr->right         = nullptr;

        root = other.root;
        other.root = curr;
        last = other.last;
        other.last = curr;
        nr_occu = other.nr_occu;
        other.nr_occu = 0;
        curr = other.curr;
        other.curr = other.root;
    }

    ~TreeMap()
    {
        deleteHelper(root);
    }

    TreeMap& operator=(const TreeMap& other)
    {
        if (root == other.root)
            return *this;
        deleteHelper(root);
        root = curr = copyHelper(other.root);
        linkParents(root);
        while (curr->right)
            curr = curr->right;
        last = curr;
        nr_occu = other.nr_occu;
        return *this;
    }

    TreeMap& operator=(TreeMap&& other)
    {
        if (root == other.root)
            return *this;
        deleteHelper(root);

        curr = new Node({});
        curr->parent        = nullptr;
        curr->left          = nullptr;
        curr->right         = nullptr;

        root = other.root;
        other.root = curr;
        last = other.last;
        other.last = curr;
        nr_occu = other.nr_occu;
        other.nr_occu = 0;
        curr = other.curr;
        other.curr = other.root;

        return *this;
    }

    bool isEmpty() const
    {
        return !(bool)nr_occu;
    }

    mapped_type& operator[](const key_type& key)
    {
        if (!nr_occu)    //jezeli pierwszy
        {
            delete root->data;
            delete root;
            root = new Node(key);
            nr_occu++;
            root->right = last = new Node({});                 /* element "za ostatnim" - end() */
            last->parent = root;
            last->left = nullptr;
            last->right = nullptr;
            return (root->data->second);
        }

        curr = root;

        while (curr)                                            /* szukam do zmiany */
        {
            if (curr->data->first < key)
                curr = curr->right;
            else if (curr->data->first == key)
                return curr->data->second;                      /* zwroc jezeli znalazles */
            else
                curr = curr->left;
        }

        curr = root;

        while (curr)                                            /* szukam do wstawienia */
        {
            if ((curr->data->first < key) || (curr == last))
            {
                if (curr->right)                                /* jezeli istnieje to ide dalej */
                    curr = curr->right;
                else                                            /* jezeli nie, to wstawiam tutaj tylko moze to byc end() */
                {
                    if (curr == last)                           /* jezeli jestem na end() to usun i wstaw dalej */
                    {
                        last = curr->parent;                    /* uzywam last jako zmiennej zapasowej */
                        delete curr->data;
                        delete curr;
                        curr = new Node(key);
                        nr_occu++;
                        curr->parent = last;
                        curr->left = nullptr;
                        last->right = curr;
                        curr->right = last = new Node({});
                        last->parent = curr;
                        last->left = nullptr;
                        last->right = nullptr;
                        return curr->data->second;
                    }
                    else                                        /* nie jest endem */
                    {
                        curr->right = new Node(key);
                        curr->right->parent = curr;
                        curr->right->left = nullptr;
                        curr->right->right = nullptr;
                        return curr->right->data->second;
                    }
                }
            }
            else if (curr->data->first == key)
            {
                return curr->data->second;                      /* zwroc jezeli znalazles */
            }
            else
            {
                if (curr->left)
                    curr = curr->left;
                else
                {
                    curr->left = new Node(key);
                    nr_occu++;
                    curr->left->parent = curr;
                    curr->left->left = nullptr;
                    curr->left->right = nullptr;
                    return curr->left->data->second;
                }
            }
        }
        throw std::runtime_error("This isn't supposed to happen.");
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        auto temp = find(key).iter;
        if (last == temp)
            throw std::out_of_range("Key non-existant");
        return temp->data->second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        auto temp = find(key).iter;
        if (last == temp)
            throw std::out_of_range("Key non-existant");
        return temp->data->second;
    }

    const_iterator find(const key_type& key) const
    {
        const_iterator temp;
        temp.itre = this;
        temp.iter = root;

        while (temp.iter)   /* jezeli wejde na nullptr to zwracam koniec */
        {
            if (temp.iter->data->first < key)
                temp.iter = temp.iter->right;
            else if (temp.iter->data->first == key)
                return temp;
            else
                temp.iter = temp.iter->left;
        }

        temp.iter = last;
        return temp;
    }

    iterator find(const key_type& key)
    {
        iterator temp;
        temp.itre = this;
        temp.iter = root;

        while (temp.iter)   /* jezeli wejde na nullptr to zwracam koniec */
        {
            if (temp.iter->data->first < key)
                temp.iter = temp.iter->right;
            else if (temp.iter->data->first == key)
                return temp;
            else
                temp.iter = temp.iter->left;
        }

        temp.iter = last;
        return temp;

        (void)key;
        throw std::runtime_error("find bez const");
    }

    void remove(const key_type& key)
    {
        auto temp = find(key).iter;
        if (last == temp)
            throw std::out_of_range("Key non-existant");

        if (temp->parent == nullptr) /* gdy usuwamy roota */
        {
            if (temp->left) /* jezeli istnieje lewy od roota to go awansuje zeby uniknac awansowania end() */
            {
                curr = root = temp->left;
                root->parent = nullptr;
                while (curr->right) /* podpinam po prawej stronie oczywiscie */
                    curr = curr->right;
                curr->right = temp->right;
            }
            else /* jezeli lewy nie istnieje to awansuje prawy */
            {
                curr = root = temp->right;
                root->parent = nullptr;
                /* nie ma nic do podpiecia z lewej strony */
            }
        }
        else if (temp->parent->right == temp) /* gdy usuwamy prawego */
        {
            last->parent->right = nullptr; /* pozbywam sie last zeby nie przeszkadzal */
            delete last->data;
            delete last;

            curr = temp->parent;
            curr->right = temp->right;
            if (curr->right) /* jezeli istnieje prawy*/
            {
                curr->right->parent = curr;
                curr = curr->right;
            }

            while (curr->left)
                curr = curr->left;
            curr->left = temp->left;
            if (curr->left)
                curr->left->parent = curr;

            last = root;
            while(last->right)
                last = last->right;
            last->right = new Node({});
            last->right->parent = last;
            last = last->right;
            last->right = nullptr;
            last->left = nullptr;
        }
        else    /* gdy lewego */
        {
            last->parent->right = nullptr; /* pozbywam sie last zeby nie przeszkadzal */
            delete last->data;
            delete last;

            curr = temp->parent;
            curr->left = temp->left;
            if (curr->left) /* jezeli istnieje lewy */
            {
                curr->left->parent = curr;
                curr = curr->left;
            }

            while (curr->right)
                curr = curr->right;
            curr->right = temp->right;
            if (curr->right)
                curr->right->parent = curr;

            last = root;
            while(last->right)
                last = last->right;
            last->right = new Node({});
            last->right->parent = last;
            last = last->right;
            last->right = nullptr;
            last->left = nullptr;
        }

        delete temp->data;
        delete temp;
        nr_occu--;
    }

    void remove(const const_iterator& it)
    {
        remove(it.iter->data->first);
    }

    size_type getSize() const
    {
        return nr_occu;
    }

    bool operator==(const TreeMap& other) const
    {
        if (nr_occu != other.nr_occu) /* jezeli rozne rozmiary to od razu mozna stwierdzic ze nierowne */
            return false;

        auto temp = other.begin();
        const Node *temp2;
        while(temp.iter != other.last)
        {
            temp2 = find(temp.iter->data->first).iter;
            if ((temp2 == last) || (temp2->data->second != temp.iter->data->second))
                return false;
            temp++;
        }
        return true;
    }

    bool operator!=(const TreeMap& other) const
    {
        return !(*this == other);
    }

    iterator begin()
    {
        iterator temp;
        temp.itre = this;
        temp.iter = root;
        while(temp.iter->left)
            temp.iter = temp.iter->left;
        return temp;
    }

    iterator end()
    {
        iterator temp;
        temp.itre = this;
        temp.iter = last;
        temp.up   = last->parent;
        return temp;
    }

    const_iterator cbegin() const
    {
        const_iterator temp;
        temp.itre = this;
        temp.iter = root;
        while(temp.iter->left)
            temp.iter = temp.iter->left;
        return temp;
    }

    const_iterator cend() const
    {
        const_iterator temp;
        temp.itre = this;
        temp.iter = last;
        temp.up   = last->parent;
        return temp;
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

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename TreeMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename TreeMap::value_type;
    using pointer = const typename TreeMap::value_type*;

    const TreeMap *itre;
    const typename TreeMap<KeyType, ValueType>::Node *iter, *up;

    explicit ConstIterator()
    {}

    ConstIterator(const ConstIterator& other)
    {
        itre = other.itre;
        iter = other.iter;
        up = other.up;
    }

    ConstIterator& operator++()
    {
        if (iter == (itre->end()).iter)
            throw std::out_of_range("Pointer past the end()");

        if (!iter->parent)                  /* jezeli jestem rootem to ide w prawo i maksymalnie na lewo */
        {
            iter = iter->right;
            while(iter->left)
                iter = iter->left;
            return *this;
        }

        if (iter->right)                   /* jezeli istnieje w prawo to tam ide a potem max w lewo     */
        {
            iter = iter->right;
            while(iter->left)
                iter = iter->left;
            return *this;
        }

        while (iter == iter->parent->right) /* wychodze az wyjde z lewej */
            iter = iter->parent;
        iter = iter->parent;
        return *this;
    }

    ConstIterator operator++(int)
    {
        auto temp = *this;
        this->operator++();
        return temp;
    }

    ConstIterator& operator--()
    {
        if (iter == (itre->begin()).iter)
            throw std::out_of_range("Pointer before the begin()");

        if (!iter->parent)                  /* jezeli jestem rootem to ide w lewo i maksymalnie na prawo */
        {
            iter = iter->left;
            while(iter->right)///tu sie wypuerdala
                iter = iter->right;
            return *this;
        }

        if (iter->left)                   /* jezeli istnieje w lewo to tam ide a potem max w prawo     */
        {
            iter = iter->left;
            while(iter->right)
                iter = iter->right;
            return *this;
        }

        while (iter == iter->parent->left) /* wychodze az wyjde z prawej */
            iter = iter->parent;
        iter = iter->parent;
        return *this;
    }

    ConstIterator operator--(int)
    {
        auto temp = *this;
        this->operator--();
        return temp;
    }

    reference operator*() const
    {
        if (this->iter == (itre->end()).iter)
            throw std::out_of_range("Pointer dereferenced at the end()");
        return *(this->iter->data);
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    bool operator==(const ConstIterator& other) const
    {
        if (this->iter == other.iter)
            return true;
        else
            return false;
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename TreeMap::reference;
    using pointer = typename TreeMap::value_type*;


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

    pointer operator->() const
    {
        return &this->operator*();
    }

    reference operator*() const
    {
        // ugly cast, yet reduces code duplication.
        return const_cast<reference>(ConstIterator::operator*());
    }

};

}

#endif /* AISDI_MAPS_MAP_H */
