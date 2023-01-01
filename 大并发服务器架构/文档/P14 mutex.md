# P14 

## 本章目标

```markdown
MutexLock/MutexLockGuard 互斥锁
Condition 条件变量
CountDownLatch 倒计时门闩类
```

## Mutex 类图

### MuteLock

![image-20230101205702458](/home/erfenjiao/learn/大并发服务器架构/截图/P14/14.1.png)

isLockedByThisThread() 是否当前线程拥有该锁

### MutexLockGuard

使用 RAII 封装

![image-20230101205828389](/home/erfenjiao/learn/大并发服务器架构/截图/P14/14.2.png)

RAII ， 避免忘记解锁

MutexLockGuard 与 MutexLock 是关联关系

> 关联关系 ：二者之间没有存在整体与局部的关系
>
> 聚合关系： 二者之间存在整体与局部的关系
>
> 组合关系： 二者之间既存在整体与局部的关系，又负责对象的生存期



### mutex_test测试输出

single thread without lock 0.236612

single thread with lock 1.307962

1 thread(s) with lock 1.289032

2 thread(s) with lock 2.552723

3 thread(s) with lock 3.815750

4 thread(s) with lock 5.169259

5 thread(s) with lock 6.392334

6 thread(s) with lock 7.560353

7 thread(s) with lock 8.858318



## Condition 条件变量

### 条件变量使用规范

条件变量：是利用线程间共享的全局变量进行同步的一种机制，主要包括两个动作：

​		一个等待“条件变量的条件成立”而挂起

​		另一个线程使“条件成立” （给出条件成立信号）

为了防止竞争，条件变量的使用总是和一个互斥锁结合在一起。

```
1. 执行
   锁住 mutex
   while(condition)
   	wait
   解锁 mutex
2. 改变条件
   锁住 mutex
   更改条件
   signal 或 broadcast
   解锁
```

条件变量类型是 pthread_cond_t



### 类图

![image-20230101221040627](/home/erfenjiao/learn/大并发服务器架构/截图/P14/condition.png)



### CountDownLatch类图

![image-20230101231523056](/home/erfenjiao/learn/大并发服务器架构/截图/P14/countdownMath.png)

既可以用于所有子线程等待主线程发起 “起跑” 

也可以用于主线程等待子线程初始化完毕才开始工作



#### CountDownLatch_test1.cc

```c++
class Test {
 public:
  Test(int numThreads)
    : latch_(1),
      threads_(numThreads) {
    for (int i = 0; i < numThreads; ++i) //创建出三个线程 {
      char name[32];
      snprintf(name, sizeof name, "work thread %d", i);
      threads_.push_back(new muduo::Thread(
            boost::bind(&Test::threadFunc, this), muduo::string(name)));
    }
    for_each(threads_.begin(), threads_.end(), boost::bind(&Thread::start, _1));
  }

  void run() {
    latch_.countDown();
  }

  void joinAll() {
    for_each(threads_.begin(), threads_.end(), boost::bind(&Thread::join, _1));
  }

 private:

  void threadFunc() {
    latch_.wait();
    printf("tid=%d, %s started\n",
           CurrentThread::tid(),
           CurrentThread::name());

    

    printf("tid=%d, %s stopped\n",
           CurrentThread::tid(),
           CurrentThread::name());
  }

  CountDownLatch latch_;
  boost::ptr_vector<Thread> threads_;
};

int main() {
  printf("pid=%d, tid=%d\n", ::getpid(), CurrentThread::tid());
  Test t(3);
  sleep(3);
  printf("pid=%d, tid=%d %s running ...\n", ::getpid(), CurrentThread::tid(), CurrentThread::name());
  t.run();
  t.joinAll();

  printf("number of created threads %d\n", Thread::numCreated());
}
```



