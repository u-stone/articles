[TOC]

# 概述

作为C\++标准库终于在11标准中加入了多线程库。这为C\++的跨平台开发提供了一个利器。
C\++11中的多线程编程，涉及到的概念/工具有以下几个：

1. 线程对象 thread
2. std::this_thread空间中的几个方法：yield，get_id，sleep_for，sleep_until
3. 互斥量：mutex，timed_mutex， recusive_mutex，recusive_timed_mutex
4. 锁：lock_guard，unique_lock 保护冲突资源
5. std空间中的方法：try_lock，lock，可以同时锁定多个锁
6. 条件变量：condition_variable，condition_variable_any用来做线程间通信（主要是通知notify）
7. std命名空间中的方法：notify_all_at_thread_exit
8. C\++11为多线程编程提供的future范式（都是模版类型）：promise，packaged_task，future，shared_future. 还有一个支持函数std::async。

以上就是C\++11提供的全部多线程模块的重要工具。

除了上面直接关联的工具，还有就是和多线程天然关联甚多的时间类的库：chrono。

# 分述

## thread用法

thread类，定义在\<thread>文件中，位于std命名空间的。用法十分灵活，比如你可以按下面的方式初始化一个thread对象的执行体executor：

1. 全局/静态函数

```c
void work_func();
std::thread my_thread(work_func());
// 或者使用新统一的初始化语法
// std::thread my_thread{work_func()};
```

2. lambda表达式

这个应该是在C\++11中使用最多的方法了。原因当然是这样做十分方便。

```c
std::thread mythread([](parameter-list) -> return-value-type {
    // do something
});
```

3. functor

```c
class Task{
public:
    void operator()()const {
        // do something
    }
};

Task task;
std::thread mythread(task);

```

4. 甚至是一个类的成员函数member function

```c
class Task {
public:
    void func(int param) {
        // do something
    }
}

Task task;
int param = 250;
std::thread mythread(&Task::func, &task, param);
```

5. std::bind改造之后的函数

```c
void work_func(int a, char* s);
std::thread mythread(std::bind(work_func, 1, "234"));
```

6. std::pakaged_task

```c
#include <thread>
#include <future>

void work_func(const char* s, int a) {
}

void executor() {
    std::packaged_task<void(const char*, int)> task(std::bind(work_func, nullptr, std::placeholders::_2));
    std::thread t(std::move(task), "123", 2);
    t.join();
}

int main()
{
    executor();
}
```

一个关联了执行体的thread对象两种状态：joinable和非joinable。一旦一个线程对象创建好之后就启动了线程，这时候线程处于joinable状态；当调用join函数，状态就切换到了非joinable状态；如果调用了detach函数，状态也是切换到了非joinable状态。

初始化thread对象的时候，如果执行函数的参数是引用类型要注意：在一些**老**的编译器上，thread构造函数可能会无视函数期待的参数类型，并盲目拷贝一份实参数据，然后传给执行函数。所以这个时候需要使用std::ref/std::cref来包装一下待传的参数。比如：

```c
void work_func(std::string &s);

void executor() {
    std::string data = "123";
    // 错误的用法：在clang++中，这个方式出现了语法错误：attempt to use a deleted function.
    // std::thread t(work_func, data);
    // 正确的用法
    std::thread t(work_func, std::ref(data));
    t.join();
    handle_data(data);
}
```

thread类重载了赋值操作符=，这个意思是一旦将thread对象A赋值给另一个thread对象B，那么A将不再有对于该线程的ownership，并处于空构造状态，也就是没有关联任何线程。同时，如果赋值之前B对象是一个有效的线程对象，也就是有一个关联的joinable的、处于运行中的线程，那么赋值操作会导致B调用std::terminate()结束掉B所代表的线程。
这样的话，thread就可以当作参数传递。同时也可以使用标准库中的各类容器批量保存、传递thread对象。

std::thread::id为标识thread提供了一个工具，这是一个类。它提供了全套的比较操作：==、!=、>、>=、<、<=。标准库还提供了一个方法来获取当前线程的ID：std::this_thread::get_id();

另外，std::thread的使用，或者说多线程编程中，一般会用到计时工具，在C\++11中就是std::chrono。

## 锁和条件变量的用法

在多线程编程中，当多个线程访问一个共享数据d的时候，如果只是读取d，那么没有任何问题；但是如果有的线程要去写，有的线程要去读，那么就会出现访问冲突。出现访问冲突的时候会有各种不可知的问题，比如访问到的数据不正确，或者去访问一个被删除的数据内存，导致程序crash。
可以想到的下面的策略来访问/修改有冲突的共享数据：

1. 采用保护机制，保证各个线程访问的时候，数据要么没有被修改，要么已经修改完成。也就是说不会暴露给其他线程数据正在修改的状态
2. 无锁编程，说起来很理想，但是很难实现
3. 使用事务方式来处理，但是C\++没有提供这样的方案

所以，一般来说还是要用策略1的。下面详述如何保护共享数据。

### 互斥量

1. 最普通最常用的std::mutex，以及一个设置最大阻塞时间的锁std::timed_mutex。
2. 支持同一个线程多次上锁的std::recursive_mutex，以及对应的一个设置最大阻塞时间的锁std::recursive_timed_mutex。

比如下面的例子演示了如何使用std::timed_mutex，最后会先输出start ，之后等待大约5秒之后输出hello world. 也就是std::timed_mutex的方法try_lock_until阻塞了线程约5秒，等到5秒超时之后才继续执行后面的代码。

```c
#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>

std::timed_mutex test_mutex;

void f()
{
    auto now=std::chrono::steady_clock::now();
    test_mutex.try_lock_until(now + std::chrono::seconds(5));
    std::cout << "hello world\n";
}

int main()
{
    std::cout << "start\n";
    std::lock_guard<std::timed_mutex> l(test_mutex);
    std::thread t(f);
    t.join();
}
```

下面的例子演示了如何使用std::recursive_mutex.

```c
#include <iostream>
#include <mutex>

int main()
{
    std::recursive_mutex test;
    if (test.try_lock()==true) {
        std::cout << "lock acquired" << std::endl;
        test.unlock(); //now unlock the mutex
    } else {
        std::cout << "lock not acquired" << std::endl;
    }

    test.lock(); //to lock it again
    if (test.try_lock()) {
        std::cout << "lock acquired" << std::endl;
        test.unlock();
    } else {
        std::cout << "lock not acquired" << std::endl;
    }
    test.unlock();
}

输出：
lock acquired
lock acquired
```

### 锁的类型

核心做法是使用std::mutex系列对象做线程同步，一般对要保护的共享数据进行访问之前，要先获取mutex。而mutex保证了同一时刻最多有一个线程得到访问权。上面提到的情况是当多个线程访问同一个共享数据的情况。但是还有一种情况是多个线程需要同时获得多个共享数据的访问权，不然就不能工作，这个时候除了上面提到的数据访问冲突之外，还会有一种新情况 -- ***死锁***。

C\++11提供了一个std::lock函数，可以同时锁定多个mutex互斥量，而且没有死锁风险。不过要使用这种方法的话，需要先给拥有mutex的锁(std::lock_guard/std::unique_lock)设置锁定策略为std::adopt_lock_t。

其中锁的策略有三种:

1. defer_lock_t 表示不要求拥有mutex的所有权
2. try_to_lock_t 以非阻塞的方式去尝试获取mutex的所有权
3. adopt_lock_t 假定调用线程已经获取到了mutex的所有权

C\++11中提供了两种锁: std::lock_guard和std::unique_lock。后者比前者更为灵活，因为后者支持所有权的转移。
相比而言后者的缺点是体积较大，性能稍差，所以要尽量使用前者。

std::lock_guard很简单，就是使用一个std::mutex初始化（非默认构造函数）的时候上锁，析构的时候自动解锁，而且不支持拷贝，赋值，移动三种构造方式，是一种RAII类型的封装。

std::unique_lock支持所有权的转移、释放，主动上锁、解锁、还可以使用std::swap函数与另一把锁交换。所以可以把这种类型的锁当作参数传递，也可以将这个类型的局部变量作为函数返回值返回，也可以保存在容器中。它支持6种类型的初始化：

1. std::unique_lock\<std::mutex\> lk(some_mutex); 这种是同std::lock_guard\<std::mutex\>一样，创建的时候获取锁，析构的时候释放锁。
2. std::unique_lock\<std::mutex\> lk(some_mutex, std::defer_lock);  这个不会为some_mutex上锁
3. std::unique_lock\<std::mutex\> lk(some_mutex, std::try_to_lock);  这种锁会以一种非阻塞的方式上锁，假如当前线程已经获得了mutext的拥有权的话，只要mutex不是recursive类型的互斥量，那么行为未定义。
4. std::unique_lock\<std::mutex\> lk(some_mutex, std::adopt_lock); 不去获取锁，而且是假定当前线程已经获得了互斥量的拥有权。
5. std::unique_lock\<std::timed_mutex\> lk(some_mutex, time_duration);  这个会尝试调用some_mutex的try_lock_for(time_duration)方法，所以some_mutex必须是支持try_lock_for方法的互斥量。
6. std::unique_lock\<std::timed_mutex\> lk(some_mutex, time_point); 同上面一种类型，这个是调用了some_mutex的try_lock_until(time_point); 也是要求互斥量要支持try_lock_until方法。

下面两种锁的用法效果是一样的：

```c
// lock both mutexes without deadlock
std::lock(from.m, to.m);
// make sure both already-locked mutexes are unlocked at the end of scope
std::lock_guard<std::mutex> lock1(from.m, std::adopt_lock);
std::lock_guard<std::mutex> lock2(to.m, std::adopt_lock);

// equivalent approach:
std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);
std::lock(lock1, lock2);

// do something
```

由于std::unique_lock锁比std::lock_guard锁灵活，可以控制的粒度也比较细，比如说：

```c
std::unique_lock<std::mutex> my_lock(the_mutex);
some_class data_to_process=get_next_data_chunk();
my_lock.unlock(); // 1 不要让锁住的互斥量越过process()函数的调用
result_type result=process(data_to_process);
my_lock.lock(); // 2 为了写入数据，对互斥量再次上锁
write_result(data_to_process,result);
```

上面的里面my_lock_只在一定代码区间上了锁，如果要用std::lock_guard的话，就做不到这一点（除非使用多把std::lock_guard锁，并使用作用域的小技巧）。

#### 避免死锁的建议

1. 一个线程最多获取一个锁，如果需要获取多个锁的时候，使用std::lock。
2. 持有锁的时候避免调用用户代码（不可控代码）。
3. 使用固定的顺序获取锁，这样就不会出现不同的线程获取了不同的锁。
4. 使用有层次结构的锁，其实也是按顺序获取锁的一种特殊情况。意思就是不要跨级获取锁，而且每一级只有一个锁。

### 数据初始化阶段的保护

上面讨论到的是站在线程的角度，在线程运行起来之后对数据访问阶段的数据保护。还有一个特殊的时候，就是数据初始化的时候，尤其是我们在多线程环境下写单例代码的时候，我们会写出过度保护的代码，或者有漏洞的double-checked lock代码。于是C\++提供了一种类似swift的机制，在语言层面提供了两个工具：std::once_flag和std::call_once。

```c
std::shared_ptr<some_resource> resource_ptr;
std::once_flag resource_flag; // 1
void init_resource()
{
    resource_ptr.reset(new some_resource);
}

void foo()
{
    std::call_once(resource_flag,init_resource); // 可以完整的进行一次初始化
    resource_ptr->do_something();
}
```

std::call_once是一个模板函数，支持所有可调用类型的初始化代码。这一点类似std::thread的初始化参数。目前这些Callable类型有下面：

- 普通函数
- 成员函数
- 函数体：functor
- lambda表达式
- 函数指针
- std::function类型
- std::packaged_task

多提一句，在C\++17中提供了一种可以支持多个线程同时读取共享数据的工具：std::shared_lock\<std::shared_mutex> lk(some_shared_mutex);而对于写的时候，可以使用std::lock_guard\<std::shared_mutex> lk(some_shared_mutex)来取得some_shared_mute的独占权;

## 条件变量和future范式

前面提到的情况是对共享数据的保护，为此引入了互斥量、锁来阻塞线程，直到获取到互斥量的线程释放锁，才会由线程调度来唤起被互斥量阻塞的线程，所以说这种方法是被动等待线程调度的多线程同步方式。

但是，有时候会碰到需要在某个特定的时机之下主动唤起被阻塞的一个或多个线程去工作的情况，比如说典型的生产者-消费者模型中，当生产者生产的数据被消费殆尽的时候，消费者只能等待，注意这个时候不是因为要保护数据而阻塞线程，当生产者又有数据产出的时候，需要及时去通知消费者取数据。这种情况仅有互斥量和锁就不能达到效果了。于是便需要引入条件变量。

### 条件变量

C\++11提供了两种条件变量：std::condition_variable和std::condition_variable_any，还有一个唤醒线程的函数：std::notify_all_at_thread_exit。

其中，std::condition_variable只能配合std::mutex一起工作，一般是std::unique_lock\<std::mutex>。std::condition_variable_any可以配合所有类型的互斥量。

两种条件变量都支持通知方法notify_one、notify_all，都支持等待方法：wait、wait_for、wait_until。

```c
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;

void worker_thread()
{
    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{return ready;});

    // after the wait, we own the lock.
    std::cout << "Worker thread is processing data\n";
    data += " after processing";

    // Send data back to main()
    processed = true;
    std::cout << "Worker thread signals data processing completed\n";

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
}

int main()
{
    std::thread worker(worker_thread);

    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    cv.notify_one();

    // wait for the worker
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return processed;});
    }
    std::cout << "Back in main(), data = " << data << '\n';

    worker.join();
}

// output:
main() signals data ready for processing
Worker thread is processing data
Worker thread signals data processing completed
Back in main(), data = Example data after processing
```

### future范型

如果要实现一种线程同步以达到这样的效果：如果等待的某件事情或者说是某个前置条件e没有发生，那么就先去随便做一些别的事情，直到得到e发生了的通知，再回过头来完成需要以来e发生的后续工作。使用条件变量、互斥量也可以实现；但是当这个等待的e是一次性发生的话，上述实现略微繁琐，因为之前讨论的这些工具有一个背景，即假定程序中的这类事件是多次等待、多次发生、有重复性但是没有顺序性的。

针对这种情况，C\++11提供了一种工具future来简捷地处理这类一次性的等待事件，即三个模版类 std::promise、std::future、std::shared_future。不过future不提供线程同步功能，在多线程环境中访问处理结果的时候，需要使用线程同步工具来辅助，比如互斥量。

当只有一个线程等待另一个唯一的线程中处理某个事情的时候，可以使用std::future; 当多个线程等待另一个唯一的线程处理某个事情的时候，可以使用std::shared_future。

一个future是用来等待获取一个异步结果的。使用std::promise/std::async/std::pakaged_task得到一个std::future对象f，然后f调用get()或者wait***()方法准备得到结果，如果还没有计算好结果，那么这些函数会阻塞调用线程；如果计算好了，就可以通过修改关联到创建std::future对象的共享状态（shared state），通知可以得到结果。

#### promise的用法

一个std::promise对象关联有一个shared state对象s，这个s里面有**state信息**和**一个计算结果**（未计算的、计算完成的、计算得到异常结果的三种）。std::promise支持move语义，即可以保存、交换，但是copy constructor被定义为delete，所以不支持copy；综合一下说就是仅支持shared state转移。

promise-future范型要配合std::thread来完成。

```c
#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <iostream>
#include <chrono>

void accumulate(std::vector<int>::iterator first,
                std::vector<int>::iterator last,
                std::promise<int> accumulate_promise)
{
    int sum = std::accumulate(first, last, 0);
    accumulate_promise.set_value(sum);  // Notify future
}

void do_work(std::promise<void> barrier)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    barrier.set_value();
}

int main()
{
    // Demonstrate using promise<int> to transmit a result between threads.
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
    std::promise<int> accumulate_promise;
    std::future<int> accumulate_future = accumulate_promise.get_future();
    std::thread work_thread(accumulate, numbers.begin(), numbers.end(),
                            std::move(accumulate_promise));
    accumulate_future.wait();  // wait for result
    std::cout << "result=" << accumulate_future.get() << '\n';
    work_thread.join();  // wait for thread completion

    // Demonstrate using promise<void> to signal state between threads.
    std::promise<void> barrier;
    std::future<void> barrier_future = barrier.get_future();
    std::thread new_work_thread(do_work, std::move(barrier));
    barrier_future.wait();
    new_work_thread.join();
}
```

#### std::async的用法

std::async是一个模板函数，参数类似std::thread，可以传递各种Callable类型的参数，返回值是一个std::future对象。还可以指定2中调用的策略：

1. std::launch::async  启动一个新的线程去跑任务
2. std::launch::deferred 不启动新线程，而是在调用线程中延时执行，延迟的时间点是调用返回值std::future对象的get，wait***方法的时候

```c
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <future>
#include <string>
#include <mutex>

std::mutex m;
struct X {
    void foo(int i, const std::string& str) {
        std::lock_guard<std::mutex> lk(m);
        std::cout << str << ' ' << i << '\n';
    }
    void bar(const std::string& str) {
        std::lock_guard<std::mutex> lk(m);
        std::cout << str << '\n';
    }
    int operator()(int i) {
        std::lock_guard<std::mutex> lk(m);
        std::cout << i << '\n';
        return i + 10;
    }
};

template <typename RandomIt>
int parallel_sum(RandomIt beg, RandomIt end)
{
    auto len = end - beg;
    if (len < 1000)
        return std::accumulate(beg, end, 0);

    RandomIt mid = beg + len/2;
    auto handle = std::async(std::launch::async,
                             parallel_sum<RandomIt>, mid, end);
    int sum = parallel_sum(beg, mid);
    return sum + handle.get();
}

int main()
{
    std::vector<int> v(10000, 1);
    std::cout << "The sum is " << parallel_sum(v.begin(), v.end()) << '\n';

    X x;
    // Calls (&x)->foo(42, "Hello") with default policy:
    // may print "Hello 42" concurrently or defer execution
    auto a1 = std::async(&X::foo, &x, 42, "Hello");
    // Calls x.bar("world!") with deferred policy
    // prints "world!" when a2.get() or a2.wait() is called
    auto a2 = std::async(std::launch::deferred, &X::bar, x, "world!");
    // Calls X()(43); with async policy
    // prints "43" concurrently
    auto a3 = std::async(std::launch::async, X(), 43);
    a2.wait();                     // prints "world!"
    std::cout << a3.get() << '\n'; // prints "53"
} // if a1 is not done at this point, destructor of a1 prints "Hello 42" here
```

#### std::pakaged_task的用法

std::pakaged_task在类的设计上与std::promise以及std::thread类似

1. 类似std::thread的构造方法，即支持多种Callable类型参数构造对象，以及可以使用默认构造函数构造一个未关联任何执行代码的空对象
2. 也是提供了一个get_future方法用来获取一个future
3. 也是提供了move语义，而且禁用了copy语义，支持swap操作

不过，std::pakaged_task更加灵活，当然副作用是体积大、性能差这些不可避免的缺点。

下面的例子介绍各类初始化方法：

```c
#include <iostream>
#include <cmath>
#include <thread>
#include <future>
#include <functional>

// unique function to avoid disambiguating the std::pow overload set
int f(int x, int y) { return std::pow(x,y); }

void task_lambda()
{
    std::packaged_task<int(int,int)> task([](int a, int b) {
        return std::pow(a, b); 
    });
    std::future<int> result = task.get_future();

    task(2, 9);

    std::cout << "task_lambda:\t" << result.get() << '\n';
}

void task_bind()
{
    std::packaged_task<int()> task(std::bind(f, 2, 11));
    std::future<int> result = task.get_future();

    task();

    std::cout << "task_bind:\t" << result.get() << '\n';
}

void task_thread()
{
    std::packaged_task<int(int,int)> task(f);
    std::future<int> result = task.get_future();

    std::thread task_td(std::move(task), 2, 10);
    task_td.join();

    std::cout << "task_thread:\t" << result.get() << '\n';
}

int main()
{
    task_lambda();
    task_bind();
    task_thread();
}

//Output:
task_lambda: 512
task_bind:   2048
task_thread: 1024
```

### shared_future

最后看一个std::shared_future的例子，这个例子是说明了shared_future如何在多个线程中等待/阻塞std::promise状态激活的。

```c
#include <iostream>
#include <future>
#include <chrono>

int main()
{
    std::promise<void> ready_promise, t1_ready_promise, t2_ready_promise;
    std::shared_future<void> ready_future(ready_promise.get_future());

    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    auto fun1 = [&, ready_future]() -> std::chrono::duration<double, std::milli> 
    {
        t1_ready_promise.set_value();
        ready_future.wait(); // waits for the signal from main()
        return std::chrono::high_resolution_clock::now() - start;
    };

    auto fun2 = [&, ready_future]() -> std::chrono::duration<double, std::milli> 
    {
        t2_ready_promise.set_value();
        ready_future.wait(); // waits for the signal from main()
        return std::chrono::high_resolution_clock::now() - start;
    };

    auto fut1 = t1_ready_promise.get_future();
    auto fut2 = t2_ready_promise.get_future();

    auto result1 = std::async(std::launch::async, fun1);
    auto result2 = std::async(std::launch::async, fun2);

    // wait for the threads to become ready
    fut1.wait();
    fut2.wait();

    // the threads are ready, start the clock
    start = std::chrono::high_resolution_clock::now();

    // signal the threads to go
    ready_promise.set_value();

    std::cout << "Thread 1 received the signal "
              << result1.get().count() << " ms after start\n"
              << "Thread 2 received the signal "
              << result2.get().count() << " ms after start\n";
}

// 可能的输出：
Thread 1 received the signal 0.03835 ms after start
Thread 2 received the signal 0.019404 ms after start
```

# 参考资料

1. cppreference网站中的 [Thread support library](https://en.cppreference.com/w/cpp/thread)， 文中的例子大多参考这里。
2. 介绍C\++标准库的书籍 《C++ Concurency in Action》
