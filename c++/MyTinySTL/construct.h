#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_

// 这个头文件包含两个函数 construct，destroy
// construct : 负责对象的构造
// destroy   : 负责对象的析构
//内存配置后的对象构造行为和内存释放后的对象析构行为
#include<new>
#include"type_traits.h"
#include"iterator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER

namespace mystl
{
    // construct 构造对象
    template<class T>
    void construct(T* ptr)
    {
        ::new ((void *)ptr) T();
    }

    template<class T1 , class T2>
    void construct(T1* ptr , const T2& value)
    {
        ::new((void*)ptr) T1(value);
    }

    template<class T1 , class ...Args>
    void construct(T1* ptr , Args&&... args)
    {
        ::new((void*)ptr) T1(mystl::forward<Args>(args)...);
    }

    //destory  析构对象
    template<class T>
    void destory_one(T* , std::true_type){}

    template<class T>
    void destory_one(T* ptr, std::false_type)
    {
        if(ptr != nullptr)
        {
            ptr->~T();
        }
    }

    template <class ForwardIter>
    void destroy_cat(ForwardIter , ForwardIter , std::true_type) {}

    template <class ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
    {
        for (; first != last; ++first)
            destroy(&*first);
    }

    template<class T>
    void destory(T* ptr)
    {
        destory_one(ptr , std::is_trivially_destructible<T>{});
    }

    template <class ForwardIter>
    void destory(ForwardIter first , ForwardIter last)
    {
        destroy_cat(first , last , std::is_trivially_destructible
        <typename iterator_traits<ForwardIter>::value_type>{});
    }

    #ifdef _MSC_VER
    #pragma warning(pop)
    #endif // _MSC_VER
};


#endif