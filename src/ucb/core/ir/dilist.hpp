#pragma once

#include <cassert>
#include <iostream>
#include <iterator>

namespace ucb
{
    template <typename T>
    class DIList;

    template<typename T>
    class DIListNode;

    template<typename T>
    class DIListIterator;

    template<typename T>
    class DIList
    {
    public:
        DIList():
            _head{nullptr},
            _tail{nullptr}
        {
        }

        DIListIterator<T> begin() { return DIListIterator<T>(nullptr, _head); }
        DIListIterator<T> end() { return DIListIterator<T>(_tail, nullptr); }

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
            n->_prev = _tail;
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

                    if (n->_next)
                    {
                        n->_next->_prev = n;
                    }

                    n->_prev = it;
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
                else
                {
                    _head->_prev = nullptr;
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
                        _tail == it;
                    }
                    else
                    {
                        aux->_next->_prev = it;
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
    class DIListNode
    {
    public:
        DIListNode():
            _prev{nullptr},
            _next{nullptr}
        {
        }

    private:
        friend DIList<T>;
        friend DIListIterator<T>;

        T *_next;
        T *_prev;
    };

    template<typename T>
    class DIListIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = size_t;
        using pointer = T*;
        using reference = T&;

        DIListIterator():
            _cur{nullptr},
            _prev{nullptr}
        {
        }

        DIListIterator(T *prev, T *cur):
            _cur{cur},
            _prev{_prev}
        {
        }

        T& operator * () { return *_cur; }

        bool operator != (const T& other)
        {
            return _cur != other._cur;
        }

        T& operator ++ ()
        {
            assert(_cur);
            _prev = _cur;
            _cur = _cur->_next;
            return *this;
        }

        T& operator -- ()
        {
            assert(_prev);
            _cur = _prev;
            _prev = _prev->_prev;
            return *this;
        }

        T& operator ++ (int)
        {
            assert(_cur);
            auto prev = _prev;
            _prev = _cur;
            _cur = _cur->_next;
            return DIListIterator(prev, _prev);
        }

        T& operator -- (int)
        {
            assert(_prev);
            auto cur = _cur;
            _cur = _prev;
            _prev = _prev->_prev;
            return IDListIterator(_cur, cur);
        }

    private:
        pointer _cur;
        pointer _prev;
    };
}
