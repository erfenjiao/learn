# RAII
资源一获取就是初始化，一旦不需要，自动释放
```c++
    class 类名{
        public:

        private:

    };
```
## 例一
RAII分配堆内存
```c++
    class HeapObjectWrapper {
        public:
            HeapObjectWrapper(int size) {
                m_p = new char[size];
            }
            ~HeapObjectWrapper() {
                delete[] m_p;
                m_p = NULL;
            }
        private:
            char* m_p;
    };
    int main() {
        HeapObjectWrapper heapobj(1024);
        if(操作不成功)
            return;
    }
```
## 其他情景：
对多线程锁的获取和释放


# 小结
理解并熟练运用RAII惯用法能让我们在开发阶段避免一些资源泄漏和死锁问题。