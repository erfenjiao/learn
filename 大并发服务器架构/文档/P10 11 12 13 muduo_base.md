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

<img src="/home/erfenjiao/learn/大并发服务器架构/截图/P10/10.1.png" alt="10.1" style="zoom: 33%;" />



![10.2](/home/erfenjiao/learn/大并发服务器架构/截图/P10/10.2.png)



#define __STDC_FORMAT_MACROS

<img src="/home/erfenjiao/learn/大并发服务器架构/截图/P10/10.3.png" alt="10.3" style="zoom: 33%;" />



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



## 无锁队列

首先，初始化节点

```c++
InitQueue(Q)
{
    node = new node()
    node->next = NULL;
    Q->head = Q->tail = node;
}
```

进队列用CAS实现的方式，基本上来说就是链表的两步操作：

1. 第一步，把tail指针的next指向要加入的结点。 `tail->next = p;`
2. 第二步，把tail指针移到队尾。 `tail = p;`

```c++
EnQueue(Q, data) //进队列
{
    //准备新加入的结点数据
    n = new node();
    n->value = data;
    n->next = NULL;

    do {
        p = Q->tail; //取链表尾指针的快照
    } while( CAS(p->next, NULL, n) != TRUE);   // 原子比较与设置
    //while条件注释：如果没有把结点链在尾指针上，再试
    /*
    	if(p->next == NULL){
    		p->next = q;
    		return TURE;
    	} else {
    		return FALSE;
    	}
    */

    CAS(Q->tail, p, n); //置尾结点 tail = n;
}
```



# P12 Exception

异常信息 <img src="/home/erfenjiao/learn/大并发服务器架构/截图/P10/12.1.png" alt="12.1" style="zoom:50%;" />



fillStackTrace() 保存异常栈信息，私有

## 代码分析

```c++
void Exception::fillStackTrace() //最关键函数
{
  const int len = 200;
  void* buffer[len];
  int nptrs = ::backtrace(buffer, len);
  char** strings = ::backtrace_symbols(buffer, nptrs); 指针数组
  if (strings)
  {
    for (int i = 0; i < nptrs; ++i)
    {
      // TODO demangle funcion name with abi::__cxa_demangle
      //stack_.append(strings[i]);
	  stack_.append(demangle(strings[i]));
      stack_.push_back('\n');
    }
    free(strings);
  }
}
```



### backtrace

```shell
SYNOPSIS
       #include <execinfo.h>

       int backtrace(void **buffer, int size);

       char **backtrace_symbols(void *const *buffer, int size);

       void backtrace_symbols_fd(void *const *buffer, int size, int fd);

DESCRIPTION
       backtrace() returns a backtrace for the calling program, in  the  array
       pointed  to  by  buffer.  
       backtrace（）返回数组中调用程序的backtrace，将这些信息保存在缓冲区buffer中
       
       A backtrace is the series of currently active function calls for the 		   program.  
       backtrace 是当前程序当中，活动的函数的一系列调用
       
       Each item in the array pointed  to  by buffer  is  of  type  void *, and 	   is the return address from the corresponding stack frame.  
       buffer 指向的数组中的每个项的类型为 void*，是相应缓冲区的返回地址
       
       The size argument specifies the  maximum  number of  addresses that can be 	      stored in buffer.  
       size参数指定最大数量 可以存储在缓冲区中的地址。
       
       If the backtrace is larger than size, then the addresses corresponding to  	     the  size  most  recent function  calls  are  returned;  to obtain the 		complete backtrace, make sure that buffer and size are large enough.
       如果回溯大于size，则对应于最近返回函数调用；要获得完整的回溯，请确保缓冲区和大小足够大。
```

<img src="/home/erfenjiao/learn/大并发服务器架构/截图/P10/12.2.png" alt="12.2" style="zoom:33%;" />

数组malloc出来，需要手动释放（free）



# P13 线程封装

## 本章目标

![13.1](/home/erfenjiao/learn/大并发服务器架构/截图/P10/13.1.png)



## 线程标识符

Linux中，每个进程有一个pid，类型pid_t，由**getpid()**取得。Linux下的POSIX线程也有一个id，类型 pthread_t，由**pthread_self()**取得，该id由线程库维护，其id空间是各个进程独立的（即不同进程中的线程可能有相同的id）。Linux中的POSIX线程库实现的线程其实也是一个进程（LWP），只是该进程与主进程（启动线程的进程）共享一些资源而已，比如代码段，数据段等。

有时候我们可能需要知道线程的真实pid。比如进程P1要向另外一个进程P2中的某个线程发送信号时，既不能使用P2的pid，更不能使用线程的pthread id，而只能使用该线程的真实pid，称为tid。

有一个函数gettid()可以得到tid，但glibc并没有实现该函数，只能通过Linux的系统调用syscall来获取。return syscall(SYS_gettid)

## Thread类图

​       typedef boost::function<void ()> ThreadFunc;

<img src="/home/erfenjiao/learn/大并发服务器架构/截图/P10/13.2.png" alt="image-20221226132504861" style="zoom:50%;" />

查看文件

```c++
Thread_test.cc
```





## __thread,gcc内置的线程局部存储设施



__thread修饰的变量是线程局部存储

如果没有 __ ，thread定义的变量就是全局的

```c++
namespace muduo
{
    namespace CurrentThread
    {
      // __thread修饰的变量是线程局部存储的。
      __thread int t_cachedTid = 0;		// 线程真实pid（tid）的缓存，
                                        // 是为了减少::syscall(SYS_gettid)系统调用的次数
                                        // 提高获取tid的效率
      __thread char t_tidString[32];	// 这是tid的字符串表示形式
      __thread const char* t_threadName = "unknown"; 
     // 判断两个类型是否相同，相同返回true
      const bool sameType = boost::is_same<int, pid_t>::value;
        // 成功
      BOOST_STATIC_ASSERT(sameType);
    }
}
```



## POD类型（plain old data）

与 c 兼容的原始数据。例如，结构和整型等 c 语言中的类型是 POD 类型，但带有用户定义的构造函数或虚函数的类则不是。即没有定义构造函数的话，就是POD类型

```c++
__thread string  t_obj("cppcourse");  // 错误，不能调用对象的构造函数
__thread string* t_obj2 = new string; // 错误，初始化只能是编译期常量
__thread string* t_obj3 = NULL;       // 正确
```



**如果非POD类型也想有POD类型的局部空间存储，要怎么办呢？**

答：网络编程阶段，线程特定数据，tsd



## pthread_atfork

被使用的地方

```c++
// Thread.cc
class ThreadNameInitializer
{
 public:
  ThreadNameInitializer()
  {
    muduo::CurrentThread::t_threadName = "main";
    CurrentThread::tid();
    pthread_atfork(NULL, NULL, &afterFork);
  }
};
```

### man

```c++
Name
    pthread_atfork - register fork handlers - 注册分叉处理程序
SYNOPSIS
    #include <pthread.h>
    int pthread_atfork(void(*prepare)(void), void(*parent)(void),
                       void (*child)(void));
    Link with -pthread.
DESCRIPTION
        The pthread_atfork() function registers fork handlers that are to
       be executed when fork(2) is called by this thread.  The handlers
       are executed in the context of the thread that calls fork(2).
		/*
			 pthread_atfork() 函数在当前线程调用 fork(2) 之前调用执行。
			 处理程序在调用 fork(2) 的线程的上下文中执行
		*/
       Three kinds of handler can be registered:
		/*
			可以注册三种处理程序
		*/

       *  prepare specifies a handler that is executed before fork(2)
          processing starts.
       /*
       		prepare 指定在 fork(2) 运行之前执行的处理程序
       */

       *  parent specifies a handler that is executed in the parent
          process after fork(2) processing completes.
       /*
       		parent 指定在fork（2）处理完成后在父进程中执行的处理程序
       */

       *  child specifies a handler that is executed in the child
          process after fork(2) processing completes.
       /*
       		指定在fork（2）处理完成后在子进程中执行的处理程序。
       */

       Any of the three arguments may be NULL if no handler is needed in
       the corresponding phase of fork(2) processing.
       /*
       		如果在fork（2）处理的相应阶段不需要处理程序，则三个参数中的任何一个都可以为NULL。
       */
```

调用 fork 时， 内部创建子进程前在父进程中会调用 prepare ，内部创建子进程成功后，父进程会调用parent，子进程会调用 child。



## 多线程中fork造成死锁问题

```c++
/*
	父进程创建了一个线程，并对mutex加锁，
	父进程创建了一个子进程，在子进程中调用doit，由于子进程会复制父进程的内存，这时候mutex处于锁的状态
	父进程在复制子进程的时候，只会复制当前线程的执行状态，其他线程不会复制，因为子进程会处理锁的状态。
*/
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = 
```



















