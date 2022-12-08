#ifndef MYTINYSTL_UNINITIALIZED_H_
#define MYTINYSTL_UNINITIALIZED_H_

// 这个头文件用于对未初始化空间构造元素
/**
 * STL 定义有五个全局函数，作用于未初始化空间上，这样的功能对于容器的实现很有帮助。
 *  前两个函数是　construct() destory() ，另外三个函数是　uninitialized_copy()　、
 * uninitialized_fill()　、 uninitialized_fill_n()
 * 分别对应于高层次函数copy()  fill()  fill_n()
 */


#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"


namespace mystl
{
    /**
     * uninitialized_copy 
     * 使我们能够将内存的配置与对象的构造行为分离开，如果作为输出目的地的
     * [reslut , result+(last-first)) 范围内的的每一个迭代器都指向未初始化区域，则uninitialized_copy
     * 会使用　copy constructor　给身为输入来源之[first , last)范围内的每一个对象产生一份复制品，
     * 放进输出范围中，换句话说，针对输入范围内的每一个迭代器 i ,该函数会调用 constructor(&*(result+(i+first)) , *i)
     * 产生　*i 的复制品，放置于输出范围的相对位置上
     */
    template<class InputIterator , class ForwardIterator>
    ForwardIterator
    uninitialized_copy(InputIterator first , InputIterator last,
    ForwardIterator result , std::true_type)
    {
        return mystl::copy(first , last , result);
    }

    template<class InputIterator , class ForwardIterator>
    ForwardIterator
    uninitialized_copy(InputIterator first , InputIterator last,
    ForwardIterator result , std::false_type)
    {
        auto cur = result;
        try
        {
            for (; first != last; ++first, ++cur)
            {
                mystl::construct(&*cur, *first);
            }
        }
        catch (...)
        {
            for (; result != cur; --cur)
                mystl::destroy(&*cur);
        }
        return cur;
    }

    template <class InputIter, class ForwardIter>
    ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
    {
        return mystl::unchecked_uninit_copy(first, last, result, 
                                            std::is_trivially_copy_assignable<
                                            typename iterator_traits<ForwardIter>::
                                            value_type>{});
    }
    /**
     * uninitialized_copy_n
     * 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置 
     */
    template <class InputIter, class Size, class ForwardIter>
    ForwardIter 
    unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type)
    {
        return mystl::copy_n(first, n, result).second;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter
    unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; n > 0; --n, ++cur, ++first)
            {
                mystl::construct(&*cur, *first);
            }
        }
        catch (...)
        {
            for (; result != cur; --cur)
                mystl::destroy(&*cur);
        }
        return cur;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result)
    {
        return mystl::unchecked_uninit_copy_n(first, n, result,
                                                std::is_trivially_copy_assignable<
                                                typename iterator_traits<InputIter>::
                                                value_type>{});
    }

    /**
     * uninitialized_fill
     * 在 [first, last) 区间内填充元素值
     * 也使我们能够将内存的配置与对象的构造行为分离开。如果 [first , last) 范围内的每一个迭代器都指向未初始化的内存
     * 那么　uninitialized_fill() 会在该范围内产生　ｘ　（第三个参数）的复制品，换句话说，uninitialized_fill()
     * 会针对操作范围内的每个迭代器 i ，调用 construct(&*i , x) , 在ｉ所指之处产生ｘ的复制品
     * “commit or rollback” 要么产生所有必要元素，要么不产生任何元素
     */
    template<class ForwardIterator , class T>
    void uninitialized_fill(ForwardIterator first , ForwardIterator last , const T&value ,
     std::true_type)
    {
        mystl(first , last , value);
    }

    template<class ForwardIterator , class T>
    void uninitialized_fill(ForwardIterator first , ForwardIterator last , const T&value ,
     std::false_type)
    {   
        auto cur = first;
        try
        {
            for(:cur != last ; cur++)
            {
                mystl::construct(&*cur , value);
            }
        }
        catch(...)
        {
            for( ; cur!= last ; cur++)
            {
                mystl::destroy(&*first);
            }
        }
    }

    template<class ForwardIterator , class T>
    void uninitialized_fill(ForwardIterator first , ForwardIterator last , const T&value)
    {
        mystl::uninitialized_fill(first , last , value , 
                                    std::is_trivially_copy_assignable<
                                    typename iterator_traits<ForwardIterator>>::value_type{});
    }

    /**
     * uninitialized_fill_n
     * 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
     * 能够使我们将内存配置与对象构造行为分离开，他会未指定范围内的所有元素设定相同的初值
     * 如果 [ first , first+n ) 范围内的每一个迭代器都指向未初始化的内存，那么uninitialized_fill_n()会调用
     * copy construct , 在该范围内产生　ｘ　（第三个参数）的复制品，换句话说，uninitialized_fill_n()
     * 会针对操作范围内的每个迭代器 i ，调用 construct(&*i , x) , 在ｉ所指之处产生ｘ的复制品
     */
    template<class ForwardIterator , class Size , class T>
    ForwardIterator
    uninitialized_fill_n(ForwardIterator first , Size n , const T& value , std::true_type)
    {
        return fill_n(first , n , value);
    }

    template<class ForwardIterator , class Size , class T>
    ForwardIterator
    uninitialized_fill_n(ForwardIterator first , Size n , const T& value , std::false_type)
    {
        auto cur;
        try
        {
            for( ; n > 0 ; n-- , ++cur)
            {
                mystl::construct(&*cur , value);
            }
        }
        catch(...)
        {
            for( ; n > 0 ; n-- , ++cur)
            {
                mystl::destory(&*first);
            }
        }
    }

    template<class ForwardIterator , class Size , class T>
    ForwardIterator
    uninitialized_fill_n(ForwardIterator first , Size n , const T& value)
    {
        return mystl::unchecked_uninit_fill_n(first, n, value, 
                                        std::is_trivially_copy_assignable<
                                        typename iterator_traits<ForwardIter>::
                                        value_type>{});
        //交由高阶函数执行
    }

    /**
     *  uninitialized_move
     *  把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
     */
    template<class InputIterator , class ForwardIterator>
    ForwardIterator
    unchecked_uninit_move(InputIterator first , InputIterator last , ForwardIterator result , std::true_type) 
    {
        return mystl::move(first , last , result);
    }

    template<class InputIterator , class ForwardIterator>
    ForwardIterator
    unchecked_uninit_move(InputIterator first , InputIterator last , ForwardIterator result , std::false_type) 
    {
        ForwardIterator cur;
        try
        {
            for( ; first != last ; first++ ,cur++)
            {
                mystl::construct(&*cur , mystl::move(*first));
            }
        }
        catch(...)
        {
            mystl::destory(result , cur);
        }
        return cur;
    }

    template<class InputIterator , class ForwardIterator>
    ForwardIterator
    uninitialized_move(InputIterator first , InputIterator last , ForwardIterator result) 
    {
        return mystl::unchecked_uninit_move(first , last , result ,
                                            std::is_trivially_move_assignable<
                                            typename iterator_traits<InputIterator>::value_type>{});                        
    }

    /**
     *  uninitialized_move_n
     *  把[first, first + n)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
     */
    template <class InputIter, class Size, class ForwardIter>
    ForwardIter 
    unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type)
    {
        return mystl::move(first, first + n, result);
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter
    unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; n > 0; --n, ++first, ++cur)
            {
                mystl::construct(&*cur, mystl::move(*first));
            }
        }
        catch (...)
        {
            for (; result != cur; ++result)
                mystl::destroy(&*result);
            throw;
        }
        return cur;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
    {
    return mystl::unchecked_uninit_move_n(first, n, result,
                                            std::is_trivially_move_assignable<
                                            typename iterator_traits<InputIter>::
                                            value_type>{});
    }
};

#endif