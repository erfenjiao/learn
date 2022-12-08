#ifndef ALGOBASE_H_
#define ALGOBASE_H_

// 这个头文件包含了 mystl 的基本算法

#include <cstring>
#include <cstdio>
#include <iostream>

#include "iterator.h"
#include "util.h"

namespace mystl
{
    #ifdef max
    #pragma message("#undefing marco max")
    #undef max
    #endif

    #ifdef min
    #pragma message("#undefing marco min")
    #undef min
    #endif

    /**
     * max
     * 返回两者中的较大值，语义相等时返回第一个
     */
    template<class T>
    const T& max(const T& lhs , const T&rhs)
    {
        return lhs >rhs ? lhs : rhs;
    }
    // 重载版本使用函数对象 comp 代替比较操作
    template<class T , class Compare>
    const T& max(const T& lhs , const T& rhs , Compare compare)
    {
        return compare(lhs , rhs) ? rhs : lhs;
    }

    /**
     * min
     */
    template<class T>
    const T& min(const T& lhs , const T&rhs)
    {
        return lhs < rhs ? lhs : rhs;
    }
    // 重载版本使用函数对象 comp 代替比较操作
    template<class T , class Compare>
    const T& min(const T& lhs , const T& rhs , Compare compare)
    {
        return compare(lhs , rhs) ? lhs : rhs;
    }

    /**
     * iter_swap
     * 将两个迭代器所指对象对调 
     */
    template<class Fiter1 , class Fiter2>
    void iter_swap(Fiter1 lhs , Fiter2 rhs)
    {
        mystl::iter_swap(*lhs , *rhs);
    }

    /**
     * copy
     * 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内 
     */
    template<class InputIter, class OutputIter>
    OutputIter unchecked_copy_cat(InputIter first , InputIter last ,
                                 OutputIter result,  mystl::input_iterator_tag)
    {
        for ( ; first != last ; first++)
        {
            *result = *first;
        }
        return result;
    }
    // ramdom_access_iterator_tag 版本
    template <class RandomIter, class OutputIter>
    OutputIter 
    unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result,
                    mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++result)
        {
            *result = *first;
        }
        return result;
    }
    template <class InputIter, class OutputIter>
    OutputIter  unchecked_copy(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_copy_cat(first , last , result , iterator_category(first));
    }

    // 为 trivially_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_copy_assignable<Up>::value,
    Up*>::type
    unchecked_copy(Tp* first, Tp* last, Up* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
            std::memmove(result, first, n * sizeof(Up));
        return result + n;
    }

    template <class InputIter, class OutputIter>
    OutputIter copy(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_copy(first, last, result);
    }
    
    /**
     * copy_backward 
     * 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
     */
    template< class BidirectionalIter1 , class BidirectionalIter2>
    BidirectionalIter2  
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
    {
        while(first != last)
            *--result = *-- first;
        return result;
    }

    template< class BidirectionalIter1 , class BidirectionalIter2>
    BidirectionalIter2  
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
    {
        while(first != last)
            *--result = *-- first;
        return result;
    }

}; // namespace mystl



#endif