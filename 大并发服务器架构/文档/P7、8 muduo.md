muduo：多线程网络服务器开发库

使用Boost机制

作者博客：http://blog.csdn.net/solstice/

#  P7 muduo网络库简介

## 理想网络库

基于对象的编程风格   boost bind/function

只做library 不做framework

库中的类相对比较独立，我们编写应用的时候需要编写一些“胶水代码”来粘合

框架时能够应用于特定应用领域的，不需要编写过多的胶水代码

框架提供用户注册一些回调函数，使得框架能够调用我们所编写的回调函数，这就使得控制反转了

 

任务服务器，相当于负载均衡器，把任务分配到后端的应用服务器



# P8 面向对象编程风格

muduo库只暴露具体类，不暴露抽象类，也不使用虚函数做接口，即muduo库不使用面向对象的编程思想，而是使用基于对象的编程思想



![8.1](/home/erfenjiao/learn/大并发服务器架构/截图/P8/8.1.png)

各自具体的线程类，要实现纯虚函数，从而得到不同的线程类，具有各自的执行体。

例子：p08

Thread.h

```c++
#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>

class Thread
{
public:
	Thread();
	virtual ~Thread();

	void Start();
	void Join();

	void SetAutoDelete(bool autoDelete);

private:
	static void* ThreadRoutine(void* arg);
	virtual void Run() = 0;
	pthread_t threadId_;
	bool autoDelete_;  //是否自动销毁
};

#endif // _THREAD_H_
```

Thread.cpp

```c++
#include "Thread.h"
#include <iostream>
using namespace std;

Thread::Thread() : autoDelete_(false) {
	cout<<"Thread ..."<<endl;
}

Thread::~Thread() {
	cout<<"~Thread ..."<<endl;
}

void Thread::Start() {
	pthread_create(&threadId_, NULL, ThreadRoutine, this);
}

void Thread::Join() {
	pthread_join(threadId_, NULL);
}

void* Thread::ThreadRoutine(void* arg) {
	Thread* thread = static_cast<Thread*>(arg);
	thread->Run();
	if (thread->autoDelete_)
		delete thread;
	return NULL;
}

void Thread::SetAutoDelete(bool autoDelete) {
	autoDelete_ = autoDelete;
}
```

Thread_test.cpp

```c++
#include "Thread.h"
#include <unistd.h>
#include <iostream>
using namespace std;

class TestThread : public Thread {
public:
	TestThread(int count) : count_(count) {
		cout<<"TestThread ..."<<endl;
	}

	~TestThread() {
		cout<<"~TestThread ..."<<endl;
	}

private:
	void Run() {
		while (count_--) {
			cout<<"this is a test ..."<<endl;
			sleep(1);
		}
	}

	int count_;
};

int main(void) {
	/*
	TestThread t(5);
	t.Start();

	t.Join();
	*/
	
	/**
		可以实现线程执行完毕，线程对象也被销毁的功能
		需要使用动态创建对象才可以
	*/
	TestThread* t2 = new TestThread(5);
	t2->SetAutoDelete(true);
	t2->Start();
	t2->Join();

	for (; ; )
		pause();

	return 0;
}
```



CMakeList.txt

build.sh



线程对象的生命周期与线程的生命周期是不一样的

可能线程执行完毕，线程对象还没有被销毁，那么可以实现线程执行完毕，线程对象也被销毁的功能吗？

需要使用动态创建对象才可以

```c++
ThreadTest.cpp
    TestThread* t2 = new TestThread(5);
	t2->SetAutoDelete(true);  
	t2->Start();
	t2->Join();

	for (; ; )
		pause();

```



# P9 boost bind/function

boost bind/function 库的出现，替代了stl中的mem_fun, ptr_fun,gind2nd等函数

函数适配器

一种接口->另一种接口

```c++
#include <iostream>
#include <boost/function.hpp>
#include <boost/bind.hpp>
using namespace std;
class Foo {
public:
    //四个参数，隐含一个Foo*指针
	void memberFunc(double d, int i, int j) {
		cout << d << endl;//打印0.5
		cout << i << endl;//打印100       
		cout << j << endl;//打印10
	}
};
int main() {
	Foo foo;
    //              void f(int)                                     Foo*      _:占位符
	boost::function<void (int)> fp = boost::bind(&Foo::memberFunc, &foo, 0.5, _1, 10);
	//把成员函数Foo::memberFunc适配成<void (int)>，使用boost::function来接收
    fp(100);
	return 0;
}

```



rector模式实现的网络库



# 编程风格区分

c编程风格

注册三个全局函数到网络库，网络库通过函数指针来回调

面向对象编程风格

用一个Echoserver继承TcpServer(抽象类)，实现三个接口

基于对象风格

用一个Echoserver



