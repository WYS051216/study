#ifndef _THREADPOOL_H
#define _THREADPOOL_H

typedef struct ThreadPool ThreadPool;

//创建线程池并初始化的函数

ThreadPool *threadPoolCreate(int min, int max, int queuesize);

//销毁线程池的函数
int threadPoolDestroy(ThreadPool* pool);


//给线程添加任务的函数  
void threadPoolAdd(ThreadPool* pool, void(*func)(void*), void* arg);


//获取线程池中工作的线程个数
int threadPoolBusyNum(ThreadPool* pool);


//获取线程池中活着的线程的个数
int threadPoolaliveNum(ThreadPool* pool);


void* worker(void* arg);
void* manager(void* arg);
void threadExit(ThreadPool* pool);
#endif  // _THREADPOOL_H
