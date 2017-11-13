#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <functional> ///std::hash

#define TAB_SIZE 101

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
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
private:
    struct Bucket;

    int nr_occu;
    Bucket HashedMap[TAB_SIZE]; /* zainicjowana zerami */

    struct Bucket
    {
        std::pair<const KeyType, ValueType> *data;
        Bucket *linkLeft, *linkRight; /* pointer do poprzedniej i nastepnej wartosci */

        Bucket()
            :   data(nullptr)
            ,   linkLeft(nullptr)
            ,   linkRight(nullptr)
        {}

        ~Bucket()
        {
            delete data;
        }

        void makeData(KeyType key, ValueType value = {})
        {
            this->data = new std::pair<const KeyType, ValueType>(key, value);
        }
    };
public:

    HashMap()
        :   nr_occu(0)
    {}

    HashMap(std::initializer_list<value_type> list)
        :   HashMap()
    {
        auto temp = list.begin();
        for (int i=0; i < list.size(); i++)
        {
            (*this)[std::get<0>(*temp)] = std::get<1>(*temp);
            temp++;
        }
    }

    HashMap(const HashMap& other)
        :   HashMap()
    {
        auto flow = other.begin();
        while (flow != other.end())
        {
            (*this)[flow.pointerToBucket->data->first] = flow.pointerToBucket->data->second;
            ++flow;
        }
    }

    HashMap(HashMap&& other)
        :   HashMap(other)
    {
        Bucket *flow;
        flow = &other.HashedMap[0];

        while (other.nr_occu)
        {
            if (flow->data)
            {
                while (flow->linkRight)
                    flow = flow->linkRight;
                while (flow->linkLeft)
                {
                    flow = flow->linkLeft;
                    delete flow->linkRight;
                    --other.nr_occu;
                }
                flow->linkRight = nullptr;
                --other.nr_occu;
            }
            ++flow;
        }
    }

    HashMap& operator=(const HashMap& other)
    {
        if (this == &other)
            return *this;

        Bucket *flow;
        flow = &HashedMap[0];

        while (nr_occu)
        {
            if (flow->data)
            {
                while (flow->linkRight)
                    flow = flow->linkRight;
                while (flow->linkLeft)
                {
                    flow = flow->linkLeft;
                    delete flow->linkRight;
                    --nr_occu;
                }
                flow->linkRight = nullptr;
                --nr_occu;
            }
            ++flow;
        }

        auto flow2 = other.begin();
        while (flow2 != other.end())
        {
            (*this)[flow2.pointerToBucket->data->first] = flow2.pointerToBucket->data->second;
            ++flow2;
        }
        return *this;
    }

    ~HashMap()
    {
        Bucket *flow;
        flow = &HashedMap[0];

        while (nr_occu)
        {
            if (flow->data)
            {
                while (flow->linkRight)
                    flow = flow->linkRight;
                while (flow->linkLeft)
                {
                    flow = flow->linkLeft;
                    delete flow->linkRight;
                    --nr_occu;
                }
                flow->linkRight = nullptr;
                --nr_occu;
            }
            ++flow;
        }
    }

    HashMap& operator=(HashMap&& other)
    {
        if (this == &other)
            return *this;

        Bucket *flow;
        flow = &HashedMap[0];

        while (nr_occu)
        {
            if (flow->data)
            {
                while (flow->linkRight)
                    flow = flow->linkRight;
                while (flow->linkLeft)
                {
                    flow = flow->linkLeft;
                    delete flow->linkRight;
                    --nr_occu;
                }
                flow->linkRight = nullptr;
                --nr_occu;
            }
            ++flow;
        }

        auto flow2 = other.begin();
        while (flow2 != other.end())
        {
            (*this)[flow2.pointerToBucket->data->first] = flow2.pointerToBucket->data->second;
            ++flow2;
        }

        flow = &other.HashedMap[0];

        while (other.nr_occu)
        {
            if (flow->data)
            {
                while (flow->linkRight)
                    flow = flow->linkRight;
                while (flow->linkLeft)
                {
                    flow = flow->linkLeft;
                    delete flow->linkRight;
                    --other.nr_occu;
                }
                flow->linkRight = nullptr;
                --other.nr_occu;
            }
            ++flow;
        }
        return *this;
    }

    bool isEmpty() const
    {
        return !(bool)nr_occu;
    }

    mapped_type& operator[](const key_type& key)
    {
        int hashdIndex = (std::hash<key_type>()(key) % TAB_SIZE);
        Bucket *flow = &HashedMap[hashdIndex]; /* zmienna do poruszania sie w kubelkach */

        while (flow->linkRight) /* szukam sobie gdzie jest wolne miejsce */
        {
            if (flow->data->first == key) /* gdy klucz istnieje */
                return flow->data->second;
            flow = flow->linkRight;
        }
        if ((flow->data) && (flow->data->first == key)) /* gdy klucz istnieje */
            return flow->data->second;
        if (HashedMap[hashdIndex].data) /* jezeli sie istnieje data to robie nowe miejsce */
        {
            flow->linkRight = new Bucket;
            flow->linkRight->linkLeft = flow;
            flow = flow->linkRight;
        }
        flow->makeData(key); /* jak nie ma daty w zerowym to robie ja (lub dla innego miejsca wskazanego przez flow */
        ++nr_occu; /* paskudnie to wyglada no ale.. */
        return HashedMap[hashdIndex].data->second;
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        auto flow = find(key).pointerToBucket;
        if ((!flow) || (!flow->data))
            throw std::out_of_range("Key non-existant");
        return flow->data->second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        auto flow = find(key).pointerToBucket;
        if ((!flow) || (!flow->data))
            throw std::out_of_range("Key non-existant");
        return flow->data->second;
    }

    const_iterator find(const key_type& key) const
    {
        int hashdIndex = (std::hash<key_type>()(key) % TAB_SIZE);
        const Bucket *flow = &HashedMap[hashdIndex]; /* zmienna do poruszania sie w kubelkach */
        ConstIterator temporary = end();

        if (!flow->data) /* jezeli od poczatku nic nie ma to zwroc end */
            return temporary;

        while (flow->data->first != key)
        {
            flow = flow->linkRight;
            if (!flow) /* jezeli wpadlem na nullptr to znaczy ze nie ma */
                return temporary;
        }

        temporary.pointerToBucket = flow;

        return temporary;
    }

    iterator find(const key_type& key)
    {
        int hashdIndex = (std::hash<key_type>()(key) % TAB_SIZE);
        Bucket *flow = &HashedMap[hashdIndex]; /* zmienna do poruszania sie w kubelkach */
        Iterator temporary = end();

        if (!flow->data) /* jezeli od poczatku nic nie ma to zwroc end */
            return temporary;

        while (flow->data->first != key)
        {
            flow = flow->linkRight;
            if (!flow) /* jezeli wpadlem na nullptr to znaczy ze nie ma */
                return temporary;
        }

        temporary.pointerToBucket = flow;
        return temporary;
    }

    void remove(const key_type& key)
    {
        auto flow = find(key).pointerToBucket;
        if ((!flow) || (!flow->data))
            throw std::out_of_range("Key non-existant");
        Bucket *flow2;

        if (!flow->linkLeft)
        {
            --nr_occu;
            if (flow->linkRight)
            {
                flow2 = flow->linkRight->linkLeft;
                auto tmp = flow2->data;
                flow2->data = flow2->linkRight->data;
                flow2->linkRight->data = tmp;
                if (flow2->linkRight->linkRight)
                    flow2->linkRight->linkRight->linkLeft = flow2;
                flow2 = flow2->linkRight;
                flow2->linkLeft->linkRight = flow2->linkRight;
                delete flow2;
            }
            return;
        }

        flow2 = flow->linkLeft->linkRight;
        flow2->linkLeft->linkRight = flow2->linkRight;
        flow2->linkRight->linkLeft = flow2->linkLeft;
        delete flow2;
    }

    void remove(const const_iterator& it)
    {
        if (it == end())
            throw std::out_of_range("Key non-existant");
        remove(it.pointerToBucket->data->first);
    }

    size_type getSize() const
    {
        return nr_occu;
    }

    bool operator==(const HashMap& other) const
    {
        if (nr_occu != other.nr_occu)
            return false;

        if (!nr_occu)
            return true;

        int auxiliary = nr_occu;
        auto flow = other.begin();
        iterator temporary;

        while (auxiliary--)
        {
            temporary = find(flow.pointerToBucket->data->first);
            if ((temporary == other.end()) || (temporary.pointerToBucket->data->second != flow.pointerToBucket->data->second))
                return false;
            ++flow;
        }
        return true;
    }

    bool operator!=(const HashMap& other) const
    {
        return !(*this == other);
    }

    iterator begin()
    {
        iterator temporary;
        temporary.pointerToWhole = this;
        temporary.pointerToBucket = &HashedMap[0];

        if (!nr_occu)
            return temporary;

        while (!temporary.pointerToBucket->data)
            ++temporary.pointerToBucket;

        return temporary;
    }

    iterator end()
    {
        int auxiliarySize = TAB_SIZE;
        iterator temporary;
        temporary.pointerToWhole = this;
        temporary.pointerToBucket = &HashedMap[0];

        if (!nr_occu)
            return temporary;

        temporary.pointerToLast = &HashedMap[TAB_SIZE-1];

        while ((!temporary.pointerToLast->data) && (auxiliarySize))
        {
            --temporary.pointerToLast;
            --auxiliarySize;
        }

        while (temporary.pointerToLast->linkRight)
            temporary.pointerToLast = temporary.pointerToLast->linkRight;

        temporary.pointerToBucket = temporary.pointerToLast->linkRight;
        return temporary;
    }

    const_iterator cbegin() const
    {
        const_iterator temporary;
        temporary.pointerToWhole = this;
        temporary.pointerToBucket = &HashedMap[0];

        if (!nr_occu)
            return temporary;

        while (!temporary.pointerToBucket->data)
            ++temporary.pointerToBucket;

        return temporary;
    }

    const_iterator cend() const
    {
        int auxiliarySize = TAB_SIZE;
        const_iterator temporary;
        temporary.pointerToWhole = this;
        temporary.pointerToBucket = &HashedMap[0];

        if (!nr_occu)
            return temporary;

        temporary.pointerToLast = &HashedMap[TAB_SIZE-1];

        while ((!temporary.pointerToLast->data) && (auxiliarySize))
        {
            --temporary.pointerToLast;
            --auxiliarySize;
        }

        while (temporary.pointerToLast->linkRight)
            temporary.pointerToLast = temporary.pointerToLast->linkRight;

        temporary.pointerToBucket = temporary.pointerToLast->linkRight;
        return temporary;
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
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename HashMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename HashMap::value_type;
    using pointer = const typename HashMap::value_type*;


    const HashMap *pointerToWhole;
    const typename HashMap<KeyType, ValueType>::Bucket *pointerToBucket;
    const typename HashMap<KeyType, ValueType>::Bucket *pointerToLast;

    explicit ConstIterator()
    {}

    ConstIterator(const ConstIterator& other)
        :   pointerToWhole(other.pointerToWhole)
        ,   pointerToBucket(other.pointerToBucket)
        ,   pointerToLast(other.pointerToLast)
    {}

    ConstIterator& operator++()///pre
    {
        if ((!pointerToBucket) || (!pointerToBucket->data))
            throw std::out_of_range("Pointer past the end()");
        if (pointerToBucket->linkRight)
            pointerToBucket = pointerToBucket->linkRight;
        else
        {
            if (pointerToBucket == pointerToWhole->end().pointerToLast)
            {
                pointerToLast = pointerToBucket;
                pointerToBucket = pointerToBucket->linkRight;
            }
            else
            {
                while (pointerToBucket->linkLeft)
                    pointerToBucket = pointerToBucket->linkLeft;
                ++pointerToBucket;
                while (!pointerToBucket->data)
                    ++pointerToBucket;
            }
        }
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
        if (pointerToBucket == pointerToWhole->begin().pointerToBucket)
            throw std::out_of_range("Pointer before the begin()");

        if ((!pointerToBucket) || (!pointerToBucket->data))
        {
            pointerToBucket = pointerToWhole->end().pointerToLast;
            return *this;
        }

        if (pointerToBucket->linkLeft)
        {
            pointerToBucket = pointerToBucket->linkLeft;
            return *this;
        }

        while (!pointerToBucket->data)
            --pointerToBucket;
        while (!pointerToBucket->linkRight)
            pointerToBucket = pointerToBucket->linkRight;
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
        if (this->pointerToBucket == (pointerToWhole->end()).pointerToBucket)
            throw std::out_of_range("Pointer dereferenced at the end()");
        return *(this->pointerToBucket->data);
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    bool operator==(const ConstIterator& other) const
    {
        if ((pointerToBucket == other.pointerToBucket) && (pointerToWhole == other.pointerToWhole))
            return true;
        return false;
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename HashMap::reference;
    using pointer = typename HashMap::value_type*;

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

#endif /* AISDI_MAPS_HASHMAP_H */
