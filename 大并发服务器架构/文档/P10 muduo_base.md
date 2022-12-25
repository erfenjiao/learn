[TOC]



# muduo基础库

muduo的目录结构如下

```c++
muduo
|-- build.sh
|-- ChangeLog
|-- CMakeLists.txt
|-- License
|-- README
|-- muduo
| 	|-- base
| 	\-- net
| 		|-- poller
|		|-- http
|		|-- inspect
| 		\-- protorpc
|-- examples
\-- TODO

```

muduo基础库

```c++
muduo
\-- base
	|-- AsyncLogging.{h,cc}
	|-- Atomic.h
	|-- BlockingQueue.h
	|-- BoundedBlockingQueue.h
	|-- Condition.h
	|-- copyable.h
	|-- CountDownLatch.{h,cc}
	|-- Date.{h,cc}
	|-- Exception.{h,cc}
	|-- Logging.{h,cc}
	|-- Mutex.h
	|-- ProcessInfo.{h,cc}
	|-- Singleton.h
	|-- StringPiece.h
	|-- tests
	|-- Thread.{h,cc}
	|-- ThreadLocal.h
	|-- ThreadLocalSingleton.h
	|-- ThreadPool.{h,cc}
	|-- Timestamp.{h,cc}
	|-- TimeZone.{h,cc}
	\-- Types.h

```



# P10 Timestamp.{h,cc}

值语义：可以拷贝的，拷贝之后与原对象脱离关系

对象语义：要么是不能拷贝的，要么可以拷贝，拷贝之后与原对象仍然存在一定的关系，比如共享底层资源（要实现自己的拷贝构造函数）



muduo::copyable 空基类，标识类，值类型

public boost::less_than_comparable<Timestamp>  要求实现 < ，可自动实现> <= >=



时间起始点

1970-01-01 00:00:00

1970-01-02 00:00:00    可用 864000



//编译时断言

BOOST_STATIC_ASSERT(sizeof(Timestamp) == sizeof(int64_t));



## PRId64

可跨平台

![10.1](/home/erfenjiao/learn/大并发服务器架构/截图/P10/10.1.png)



![10.2](/home/erfenjiao/learn/大并发服务器架构/截图/P10/10.2.png)



#define __STDC_FORMAT_MACROS

![10.3](/home/erfenjiao/learn/大并发服务器架构/截图/P10/10.3.png)



# P11 为什么需要原子性操作



## gcc原子操作

```c++
// 原子自增操作
type __sync_fetch_and_add (type *ptr, type value)

// 原子比较和交换（设置）操作
type __sync_val_compare_and_swap (type *ptr, type oldval type newval)  //返回oldval
bool __sync_bool_compare_and_swap (type *ptr, type oldval type newval)

// 原子赋值操作
type __sync_lock_test_and_set (type *ptr, type value)
使用这些原子性操作，编译的时候需要加-march=cpu-type  本地cpu

无锁队列实现
http://coolshell.cn/articles/8239.html
```













