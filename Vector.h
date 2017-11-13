#ifndef AISDI_LINEAR_VECTOR_H
#define AISDI_LINEAR_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template <typename Type>
class Vector
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

    int n; /* rozmiar do new */
    int nr_occu;  /* ilosc okupowanych miejsc */
    value_type *vec;


    Vector()
        :   n(50)
        ,   nr_occu(0)
        ,   vec(new value_type[n])

    {}

    Vector(std::initializer_list<Type> l)
        :   n(l.size()+50)
        ,   nr_occu(l.size())
        ,   vec(new value_type[n])

    {
        const value_type *tmp;
        tmp = l.begin();
        for (int i=0; i<l.size(); i++)
            vec[i] = *(tmp++);
    }

    Vector(const Vector& other)
        :   n(other.n)
        ,   nr_occu(other.nr_occu)
    {
        vec = new value_type[n];
        for (int i=0; i<n; i++)
            this->vec[i]=other.vec[i];
    }

    Vector(Vector&& other)
        :   n(other.n)
        ,   nr_occu(other.nr_occu)
        ,   vec(other.vec)
    {
        other.vec = nullptr;
        other.nr_occu = 0;
    }

    ~Vector()
    {
        delete[] vec;
    }

    Vector& operator=(const Vector& other)
    {
// przypisanie na samego siebie
        delete[] vec;
        vec = new value_type[other.n];
        n = other.n;
        nr_occu = other.nr_occu;
        for (int i=0; i<nr_occu; i++)
            this->vec[i] = other.vec[i];
        return *this;
    }

    Vector& operator=(Vector&& other)
    {
        n = other.n;
        nr_occu = other.nr_occu;
        delete[] vec;
        vec = other.vec;
        other.vec = nullptr;
        other.nr_occu = 0;
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
        if ((nr_occu + 1) != n)
        {
            vec[nr_occu] = item;
            nr_occu++;
        }
        else
        {
            Vector tmp;
            tmp = *this;
            tmp.n += 50;
            *this = tmp;
            vec[nr_occu] = item;
            nr_occu++;
        }
        return;
    }

    void prepend(const Type& item)
    {
        if ((nr_occu +1 ) != n)
        {
            for (int i = nr_occu; i > 0; i--)
                vec[i] = vec[i - 1];
            vec[0] = item;
            nr_occu++;
        }
        else
        {
            Vector tmp = *this;
            tmp.n += 50;
            *this = tmp;
            for (int i = nr_occu; i > 0; i--)
                vec[i] = vec[i - 1];
            vec[0] = item;
            nr_occu++;
        }
    }

    void insert(const const_iterator& insertPosition, const Type& item)
    {
        if ((nr_occu +1 ) != n)
        {
            int i = nr_occu;
            while(insertPosition.iter != &vec[i])
            {
                vec[i] = vec[i - 1];
                i--;
            }
            vec[i] = item;
            nr_occu++;
        }
        else
        {
            Vector tmp = *this;
            tmp.n += 50;
            *this = tmp;
            int i = nr_occu;
            while(insertPosition.iter != &vec[i])
            {
                vec[i] = vec[i - 1];
                i--;
            }
            vec[i] = item;
            nr_occu++;
        }
    }

    Type popFirst()
    {
        if (nr_occu)
        {
            Type tmp;
            tmp = vec[0];
            for(int i = 0; i < nr_occu; i++)
                vec[i] = vec[i+1];
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
            tmp = vec[nr_occu-1];
            vec[nr_occu-1] = vec[nr_occu];
            nr_occu--;
            return tmp;
        }
        else
            throw std::logic_error("popLast() on empty collection");
    }

    void erase(const const_iterator& possition)
    {
        if (possition != possition.ivec->end())
        {
            int i = 0;
            for (; &vec[i] != possition.iter; i++);
            for (; i < nr_occu; i++)
                vec[i] = vec[i+1];
            nr_occu--;
        }
        else
            throw std::out_of_range("erasing from empty collection");
    }

    void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
    {
        if (firstIncluded == firstIncluded.ivec->end())
            throw std::out_of_range("erasing from forbidden range");

        const_iterator tmp;
        tmp = lastExcluded;
        while(firstIncluded != tmp)
        {
            erase(firstIncluded);
            tmp--;
        }
    }

    iterator begin()
    {
        iterator tmp;
        tmp.iter = &vec[0];
        tmp.ivec = this;
        return tmp;
    }

    iterator end()
    {
        iterator tmp;
        tmp.iter = &vec[nr_occu];
        tmp.ivec = this;
        return tmp;
    }

    const_iterator cbegin() const
    {
        const_iterator tmp;
        tmp.iter = &vec[0];
        tmp.ivec = this;
        return tmp;
    }

    const_iterator cend() const
    {
        const_iterator tmp;
        tmp.iter = &vec[nr_occu];
        tmp.ivec = this;
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
class Vector<Type>::ConstIterator
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename Vector::value_type;
    using difference_type = typename Vector::difference_type;
    using pointer = typename Vector::const_pointer;
    using reference = typename Vector::const_reference;

    const Type *iter;
    const Vector *ivec;

    explicit ConstIterator()
    {}

    reference operator*() const
    {
        const Type *tmp;
        tmp = (ivec->end()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer dereferenced at the end()");
        return *(this->iter);
    }

    ConstIterator& operator++()
    {
        const Type *tmp;
        tmp = (ivec->end()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer past the end()");
        this->iter += 1;
        return *this;
    }

    ConstIterator operator++(int)
    {
        const Type *tmp;
        tmp = (ivec->end()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer past the end()");
        ConstIterator tmp2;
        tmp2.iter = this->iter;
        this->iter += 1;
        return tmp2;
    }

    ConstIterator& operator--()
    {
        const Type *tmp;
        tmp = (ivec->begin()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer before the begin()");
        this->iter -= 1;
        return *this;
    }

    ConstIterator operator--(int)
    {
        const Type *tmp;
        tmp = (ivec->begin()).iter;
        if (this->iter == tmp)
            throw std::out_of_range("Pointer before the begin()");
        ConstIterator tmp2;
        tmp2.iter = this->iter;
        this->iter -= 1;
        return tmp2;
    }

    ConstIterator operator+(difference_type d) const
    {
        const Type *tmp;
        ConstIterator tmp2;
        tmp = (ivec->end()).iter;
        tmp2.iter = this->iter;
        for (int i = 0; i < d; i++)
        {
            if (tmp2.iter == tmp)
                throw std::out_of_range("Pointer past the end()");
            tmp2.iter += 1;
        }
        return tmp2;
    }

    ConstIterator operator-(difference_type d) const
    {
        const Type *tmp;
        ConstIterator tmp2;
        tmp = (ivec->begin()).iter;
        tmp2.iter = this->iter;
        for (int i = 0; i < d; i++)
        {
            if (this->iter == tmp)
                throw std::out_of_range("Pointer before the begin()");
            tmp2.iter -= 1;
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
class Vector<Type>::Iterator : public Vector<Type>::ConstIterator
{
public:
    using pointer = typename Vector::pointer;
    using reference = typename Vector::reference;

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

#endif // AISDI_LINEAR_VECTOR_H

