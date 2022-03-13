#pragma once

#include <cassert>
#include <iostream>
#include <iterator>

namespace ucb
{
    template <typename T>
    class IList;

    template<typename T>
    class IListNode;

    template<typename T>
    class IListIterator;

    template<typename T>
    class IList
    {
    public:
        friend IListNode<T>;

        IList():
            _head{nullptr},
            _tail{nullptr}
        {
        }

        IListIterator<T> begin() { return IListIterator<T>(_head); }

        IListIterator<T> end() { return IListIterator<T>(_head); }

        size_t size() const
        {
            size_t cnt = 0;
            auto it = begin();

            while (it != end())
            {
                cnt++;
            }

            return cnt;
        }

        T& append(T *n)
        {
            assert(n);

            if (_head == nullptr)
            {
                _head = n;
                _tail = n;
                return *n;
            }

            _tail->_next = n;
            _tail = n;
            return *n;
        }

        T& insert(T *n, int idx)
        {
            assert(n);

            auto it = begin();
            auto count = 0;

            if (idx == 0)
            {
                n->_next = _head;
                _head = n;

                if (_tail == nullptr)
                {
                    _tail = _head;
                }

                return;
            }

            while (it != end())
            {
                if (count == idx - 1)
                {
                    n->_next = it->_next;
                    it->next = n;

                    if (it == _tail)
                    {
                        _tail = n;
                    }

                    return;
                }

                it++;
            }

            std::cerr << "operand index out of bounds" << std::endl;
            abort();
        }

        void remove(int idx)
        {
            auto it = begin();
            int count = 0;

            if (idx == 0 && _head != nullptr)
            {
                auto aux = _head;
                _head = _head->_next;

                if (_head == nullptr)
                {
                    _tail = nullptr;
                }

                delete aux;
                return;
            }

            while (it != end())
            {
                if (count == idx - 1)
                {
                    auto aux = it->_next;

                    if (aux == nullptr)
                    {
                        break;
                    }

                    it->_next = aux->_next;

                    if (aux == _tail)
                    {
                        _tail = it;
                    }

                    delete aux;
                    return;
                }

                count++;
                it++;
            }

            std::cerr << "operand index out of bounds" << std::endl;
            abort();
        }

    private:
        T *_head;
        T *_tail;
    };

    template<typename T>
    class IListNode
    {
    public:
        IListNode():
            _next{nullptr}
        {
        }

    private:
        friend IList<T>;
        friend IListIterator<T>;

        T *_next;
    };

    template<typename T>
    class IListIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = size_t;
        using pointer = T*;
        using reference = T&;

        IListIterator():
            _cur{nullptr}
        {
        }

        IListIterator(T *cur):
            _cur{cur}
        {
        }

        T& operator* () { return *_cur; }

        bool operator != (const T& other)
        {
            return _cur != other._cur;
        }

        T& operator++ ()
        {
            _cur = _cur->_next;
            return *this;
        }

        T& operator++ (int)
        {
            auto cur = _cur;
            _cur = _cur->_next;
            return IListIterator(cur);
        }

    private:
        pointer _cur;
    };
}
