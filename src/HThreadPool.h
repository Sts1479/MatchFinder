/*
 * HthreadPool.h
 *
 *  Created on: 7 сент. 2022 г.
 *      Author: stan
 */

#ifndef HTHREADPOOL_H_
#define HTHREADPOOL_H_

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>

class HThreadPool{

public:
    using Task = std::function<void()>;

    HThreadPool(int size = std::thread::hardware_concurrency()) : pool_size(size), idle_num(size), status(STOP){}

    ~HThreadPool(){
        Stop();
    }

    int Start() {
        if (status == STOP) { //if threads stopped...
            status = RUNNING; // we've starting
            for (int i = 0; i < pool_size; ++i){
                workers.emplace_back(std::thread([this]{
                    while (status != STOP){
                        while (status == PAUSE){
                            std::this_thread::yield(); // switch to next..
                        }

                        Task task; //..task
                        {
                            std::unique_lock<std::mutex> locker(mutex);
                            cond.wait(locker, [this]{
                                return status == STOP || !tasks.empty();
                            });

                            /* even if status = STOP =0, once tasks is not empty, then
							 * excucte the task until tasks queue become empty
							 */

                            if (status == STOP) return;

                            if (!tasks.empty()){ //if not empty - move to front of queue pool
                                task = std::move(tasks.front());
                                tasks.pop(); //delete in the begin of queue
                            }
                        }

						--idle_num;
						task();
						++idle_num;
                    }
                }));
            }
        }
        return 0;
    }

    int Stop() {
        if (status != STOP) {
            status = STOP;
            cond.notify_all();
            for (auto& thread: workers){
                thread.join();
            }
        }
        return 0;
    }

    int Pause() {
        if (status == RUNNING) {
            status = PAUSE;
        }
        return 0;
    }

    int Resume() {
        if (status == PAUSE) {
            status = RUNNING;
        }
        return 0;
    }

    void Wait()
    {
    	while (1)
    	{
            std::unique_lock<std::mutex> locker(mutex);
            if (tasks.empty())
            {
            	return;
            }
            locker.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    	}
    }

    // return a future, calling future.get() will wait task done and return RetType.
    // AddTask(fn, args...)
    // AddTask(std::bind(&Class::mem_fn, &obj))
    // AddTask(std::mem_fn(&Class::mem_fn, &obj))

    template<class Fn, class... Args>
    void AddTask(Fn&& fn, Args&&... args) {
        using RetType = decltype(fn(args...));
        auto task = std::make_shared<std::packaged_task<RetType()> >(
            std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...)
        );
        {
            std::lock_guard<std::mutex> locker(mutex);
            tasks.emplace([task]{
                (*task)();
            });

        }
        cond.notify_one();
    }

private:
    int pool_size;
    std::atomic<int> idle_num;

    enum Status {
        STOP,
        RUNNING,
        PAUSE,
    };
    std::atomic<Status> status;
    std::vector<std::thread> workers;

    std::queue<Task> tasks;
    std::mutex        mutex;
    std::condition_variable cond;
};

#endif /* HTHREADPOOL_H_ */
