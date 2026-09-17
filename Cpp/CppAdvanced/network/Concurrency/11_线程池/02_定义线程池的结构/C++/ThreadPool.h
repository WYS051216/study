#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "TaskQueue.h"

template <typename T>
class ThreadPool
{
public:
    //创建线程池并初始化
    ThreadPool(int min, int max);
    //销毁线程池
    ~ThreadPool();
    //给线程池添加任务
    void addTask(Task<T> task);
    //获取线程池中工作的线程的个数
    int getBusyNum();
    //获取线程池中工作的线程的个数
    int getAliveNum();

private:
    //工作的线程
    static void* worker(void* arg);
    //管理者线程任务函数
    static void* manager(void* arg);
    //单个线程退出
    void threadExit();

private:
    //任务队列
    TaskQueue<T>* taskQ;
    pthread_t managerID;
    pthread_t* threadIDs;
    int minNum;
    int maxNum;
    int busyNum;
    int liveNum;
    int exitNum;
    pthread_mutex_t mutexPool;
    pthread_cond_t notEmpty;
    static const int NUMBER = 2;
    bool shutdown;
};
#include "ThreadPool.cpp"
#endif