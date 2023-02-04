/**
 * 依赖：thread库
 * 互斥锁 mutex
 * 条件变量 condition_variable 两个，分别控制pro con
 * 队列 queue 阻塞队列的实现方式
 * 原子操作 atomic
 * 
*/


/**
 * PV 操作
 * 环形缓冲池
 * full 表示放有产品的缓冲区数，初值为0
 * empty 表示现有缓冲区个数，初值为N
 * int in = 0, out = 0;

void Producer() {
    while(1) {
       /**
        * P(empty);
        * P(mutex) 初值为1
        * 生产产品
        * in = (in + 1) mod N;
        * V(mutex)
        * V(full)
    }
}

void Consumer() {
    while(1) {
        /**
         * P(full);
         * P(mutex);
         * out = (out + 1) mod N;
         * V(mutex);
         * V(empty);
         * 
        
    }
}
*/