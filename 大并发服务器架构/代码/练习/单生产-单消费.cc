#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
//#include <sleep>

// 变量
static const int bufsize = 10;
static const int ProNum = 20; //产品数量
struct resource {
    int buf[bufsize]; // 环形缓冲区
    int read_pos;  // 消费者读取产品的位置
    int write_pos; // 生产者写入产品的位置

    std::mutex mutex; // 互斥量
    
    std::condition_variable not_full; //条件变量
    std::condition_variable not_empty;
}instance;

typedef struct resource resource;

void Producer(resource *ir, int item) {
    // 判断、生产、通知
    std::unique_lock<std::mutex> lock(ir->mutex);
    while(((ir->write_pos) % bufsize) == ir->read_pos) { // 缓冲区已满
        std::cout << "Producer is waiting for an empty slot...\n";
        (ir->not_full).wait(lock); // 生产者等待 “缓冲区不为满” 这一条件发生
    }
    // 写入产品
    (ir->buf)[ir->write_pos] = item;
    ir->write_pos++;

    if(ir->write_pos == bufsize) { // 写入位置若是在队列最后则重新设置为初始位置.
        ir->write_pos = 0;
    }

    (ir->not_empty).notify_all(); //通知消费者缓冲区不为空
}

int Consumer(resource *ir) {
    int data;
    std::unique_lock<std::mutex> lock(ir->mutex);

    while(ir->write_pos == ir->read_pos) {
        std::cout << "Consumer is waiting for items...\n";
        (ir->not_empty).wait(lock); //等待不为空
    }
    //读取产品
    data = (ir->buf)[ir->read_pos];
    ir->read_pos++;

    if(ir->read_pos >= bufsize) {
        ir->read_pos = 0;
    }

    //通知
    (ir->not_full).notify_all();

    return data;
}

void ProducerTask() {
    for(int i = 0 ; i < ProNum ; i++) {
        std::cout << "Produce the " << i << "^th item." << std::endl;
        Producer(&instance, i);
    }
}

void ConsumerTask() {
    	static int cnt = 0;
	while (1) {
		//sleep(1);
		int item = Consumer(&instance); // 消费一个产品.
		std::cout << "Consume the " << item << "^th item" << std::endl;
		if (++cnt == ProNum) break; // 如果产品消费个数为 ProNum, 则退出.
	}
}

// 初始化
void Initresource(resource *ir) {
    ir->write_pos = 0;
    ir->read_pos = 0;
}

int main() {
    Initresource(&instance);
    std::thread producer(ProducerTask);
    std::thread consumer(ConsumerTask);
    producer.join();
    consumer.join();
}