//#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/Thread.h>
#include <muduo/base/Timestamp.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>
#include <stdio.h>

using namespace muduo;
using namespace std;

MutexLock g_mutex; // 锁
vector<int> g_vec; // 向量
const int kCount = 10*1000*1000;

void threadFunc()
{
  for (int i = 0; i < kCount; ++i)  
  { 
    MutexLockGuard lock(g_mutex);
    g_vec.push_back(i);
  }
}

int main()
{
  const int kMaxThreads = 8;
  g_vec.reserve(kMaxThreads * kCount); //预留 8000 0000 个整数.300 多M

  Timestamp start(Timestamp::now());  
  for (int i = 0; i < kCount; ++i) // 1000 0000
  {
    g_vec.push_back(i);
  } 

  printf("single thread without lock %f\n", timeDifference(Timestamp::now(), start));

  start = Timestamp::now();
  threadFunc();
  printf("single thread with lock %f\n", timeDifference(Timestamp::now(), start));

  for (int nthreads = 1; nthreads < kMaxThreads; ++nthreads) // 8
  {
    boost::ptr_vector<Thread> threads; //存放 Thread 指针
    g_vec.clear(); 
    start = Timestamp::now();
    for (int i = 0; i < nthreads; ++i)
    {
      threads.push_back(new Thread(&threadFunc));
      threads.back().start();
    }
    for (int i = 0; i < nthreads; ++i)
    {
      threads[i].join();
    }
    printf("%d thread(s) with lock %f\n", nthreads, timeDifference(Timestamp::now(), start));
  }
}

