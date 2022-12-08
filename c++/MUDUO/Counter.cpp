#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<fstream>
#include<string>
#include<assert.h>
using namespace std;

class MutexLock : boost::noncopytable
{
    public:
        MutexLock() : holder_(0)
        {
            pthread_mutex_init(&mutex_ , NULL);
        }
        ~MutexLock()
        {
            assert(holder_ == 0);
            pthread_mutex_destroy(&mutex);
        }

        bool isLockedByThisThread()
        {
            return holder_ = CurrentThread::tid();
        }

        void assertLocked()
        {
            assert(isLockedByThisThread());
        }

        void lock()
        {
            pthread_mutex_lock(&mutex);
        }

        void unlock()
        {
            holder_ = 0;
            pthread_mutex_lock(&mutex);
        }

        pthread_mutex_t * getPthreadMutex()
        {
            return & mutex_;
        }
    private:
        pthread_mutex_t mutex;
        pid_t holder_;
};

class MutexLockGuard : boost::noncopyable
{
    public:
        explicit MutexLockGuard(MutexLock* mutex)
        : mutex_(mutex)
        {
            mutex_.lock();
        }
        ~MutexLockGuard()
        {
            mutex_.unlock;
        }
    private:
        MutexLock& mutex_;
};

#define MutexLockGuard(x) static_assert(false , "missing mutex guard var name")

class Counter : boost::noncopyable
{
    public :
        Counter() : value_(0){}
        int64_t value() const;
        int64_t getAndIncrease();
    private:
        int64_t value_;
        mutable MutexLock mutex_;
};

int64_t Counter::value() const
{
    MutexLockGuard lock(mutex_);
    return value_;
}

int64_t Counter::getAndIncrease()
{
    MutexLockGuard lock(mutex_);
    int64_t ret = value_++;
    return ret;
}