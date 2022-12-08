#ifndef ITERATOR_H_
#define ITERATOR_H_

/**
 * 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数
 * iterator 提供一种方法，使之能够依序寻访某个聚合物（容器）所含的各个元素，而又无需暴露
 * 该聚合物内部的表达式 
 */

#include"type_traits.h"
#include<cstddef>

namespace mystl
{
    //五种迭代器
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : public input_iterator_tag{};
    struct bidirectional_iterator_tag : public forward_iterator_tag{};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    //iterator 模板
    template <class Category , class T , class Distance = ptrdiff_t , 
              class Pointer = T* , class Reference = T&>
    struct iterator
    {
        typedef Category       iterator_category;
        typedef T              value;
        typedef Pointer        pointer;
        typedef Reference      refererce;
        typedef Distance       difference_type;
    };

    //iterator traits
    template<class T>
    struct has_iterator_cat
    {
        private:
            struct two{char a , char b};
            template <class U> static two test(...);
            template <class U> static char test(typedef U::iterator_category* = 0);
        public:
            static const bool value = sizeof(test<T>(0)) == sizrof(char);
    };

    template <class Iterator , bool>
    struct iterator_traits_impl{};

    template <class Iterator> 
    struct iterator_traits_impl<Iterator , true>
    {
        typedef typename Iterator::iterator_category       iterator_category;
        typedef typename Iterator::value_type              value_type;
        typedef typename Iterator::pointer                 pointer;
        typedef typename Iterator::refererce               reference;
        typedef typename Iterator::difference_type         difference_type;
    };
    
    template <class Iterator, bool>
    struct iterator_traits_helper {};

    template <class Iterator>
    struct iterator_traits_helper<Iterator, true> : public iterator_traits_impl<Iterator,
    std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
    std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
    {};
    
    // 萃取迭代器的特性
    template <class Iterator>
    struct iterator_traits
    : public iterator_traits_helper<Iterator , has_iterator_cat<Iterator>::value> {};

    // 针对原生指针的偏特化版本
    // template <class Category , class T , class Distance = ptrdiff_t , 
    //           class Pointer = T* , class Reference = T&>  上面
    template <class T>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag       iterator_category;
        typedef T                                value_type;
        typedef T*                               pointer;
        typedef T&                               reference;
        typedef ptrdiff_t                        difference_type;
    };

    template <class T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag           iterator_category;
        typedef T                                    value_type;
        typedef const T*                             pointer;
        typedef const T&                             reference;
        typedef ptrdiff_t                            difference_type;
    };

    template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of
    : public m_bool_constant<std::is_convertible
    <typename iterator_traits<T>::iterator_category, U>::value>
    {};

    // 萃取某种迭代器    //上文的五种迭代器
    template <class T, class U>
    struct has_iterator_cat_of<T, U, false> : public m_false_type {};

    template <class Iter>
    struct is_input_iterator  : public has_iterator_cat_of<Iter, input_iterator_tag> {};

    template <class Iter>
    struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

    template <class Iter>
    struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

    template <class Iter>
    struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

    template <class Iter>
    struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};


    // 萃取某个迭代器的 category
    template <class Iterator>
    typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&)
    {
        typedef typename iterator_traits<Iterator>::iterator_category Category;
        return Category();
    }

    //萃取某个迭代器的distance_type
    template <class Iterator>
    typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    // 萃取某个迭代器的 value_type
    template <class Iterator>
    typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    // 以下函数用于计算迭代器间的距离
    // distance 的 input_iterator_tag 的版本
    template<class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first , InputIterator last , input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while(first != last)
        {
            ++first;
            ++n;
        }
        return n;
    }

    // distance 的 random_access_iterator_tag 的版本
    template <class RandomIter>
    typename iterator_traits<RandomIter>::difference_type
    distance_dispatch(RandomIter first, RandomIter last,
                    random_access_iterator_tag)
    {
        return last - first;
    }

    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last)
    {
        return distance_dispatch(first, last, iterator_category(first));
    }

    // 模板类 : reverse_iterator
// 代表反向迭代器，使前进为后退，后退为前进
    template <class Iterator>
    class reverse_iterator
    {
        private:
            Iterator current;
        public:
            typedef typename iterator_traits<Iterator>::iterator_category       iterator_category;
            typedef typename iterator_traits<Iterator>::value_type              value_type;
            typedef typename iterator_traits<Iterator>::pointer                 pointer;
            typedef typename iterator_traits<Iterator>::refererce               reference;
            typedef typename iterator_traits<Iterator>::difference_type         difference_type;

            typedef Iterator                        iterator_type;
            typedef reverse_iterator<Iterator>      self;

        public :
            //构造函数
            reverse_iterator(){}
            explicit reverse_iterator(iterator_type i):current(i){}
            reverse_iterator(const self &rhs) : current(rhs.current){}
        
        public:
            // 取出对应的正向迭代器
            iterator_type base() const{ return current; }

            // 重载操作符
            reference operator*() const
            { 
                // 实际对应正向迭代器的前一个位置
                auto tmp = current;
                return *--tmp;
            }

            pointer operator->() const
            {
                return &(operator*());
            }

            //++ 变 --
            self& operator++()
            {
                --current;
                return *this;
            }
            self operator++(int)
            {
                self tmp = *this;
                --current;
                return tmp;
            }

            // 后退(--)变为前进(++)
            self& operator--()
            {
                ++current;
                return *this;
            }
            self operator--(int)
            {
                self tmp = *this;
                ++current;
                return tmp;
            }

            self& operator+=(difference_type n)
            {
                current -= n;
                return *this;
            }
            self operator+(difference_type n) const
            {
                return self(current - n);
            }
            self& operator-=(difference_type n)
            {
                current += n;
                return *this;
            }
            self operator-(difference_type n) const
            {
                return self(current + n);
            }
            reference operator[](difference_type n) const
            {
                return *(*this + n);
            }
    };
    // template <class Iterator> class reverse_iterator 完成

    template <class Iterator>
    typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator>& lhs , 
              const reverse_iterator<Iterator>&rhs)
    {
        return rhs.base() - lhs.base();
    }
    // 重载比较操作符
    template <class Iterator>
    bool operator==(const reverse_iterator<Iterator>& lhs,
                    const reverse_iterator<Iterator>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template <class Iterator>
    bool operator<(const reverse_iterator<Iterator>& lhs,
                   const reverse_iterator<Iterator>& rhs)
    {
        return rhs.base() < lhs.base();
    }

    template <class Iterator>
    bool operator!=(const reverse_iterator<Iterator>& lhs,
                    const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Iterator>
    bool operator>(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
    {
        return rhs < lhs;
    }

    template <class Iterator>
    bool operator<=(const reverse_iterator<Iterator>& lhs,
                    const reverse_iterator<Iterator>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class Iterator>
    bool operator>=(const reverse_iterator<Iterator>& lhs,
                    const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs < rhs);
    }
}

#endif