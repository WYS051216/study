#ifndef TASKQUEUE_H
#define TASKQUEUE_H
#include<queue>
#include<pthread.h>

using namespace std;
using callback = void(*)(void* arg);

//任务结构体
template <typename T>
struct Task
{
    using callback = void(*)(void*);
    Task()
    {
        function = nullptr;
        arg = nullptr;
    }
    Task(callback f, void* arg)
    {
        this->arg = (T*)arg;
        function = f;
    }
    callback function;
    T* arg;
};
template <typename T>
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();
    //添加任务
    void addTask(Task<T> task);
    void addTask(callback f, void* arg);
    //取出一个任务
    Task<T> takeTask();
    //获取当前任务的个数
    inline size_t taskNumber()
    {
    pthread_mutex_lock(&m_mutex);
    size_t size = m_taskQ.size();
    pthread_mutex_unlock(&m_mutex);
    return size;
    }
private:
    pthread_mutex_t m_mutex;
    queue<Task<T>> m_taskQ;
};
#include "TaskQueue.cpp"
#endif