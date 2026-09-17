#include "threadpool.h"
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>


const int NUMBER = 2;
void *worker(void *arg);
void *manager(void *arg);
//任务结构体
typedef struct Task
{
    //这是一个函数指针，表示任务要执行的函数，这个函数接收一个void*类型的参数，返回值是void
    void (*function)(void* arg);
    //这个任务对应的参数，可以传任意类型的数据地址
    void* arg;
}Task;

//线程池的结构体
typedef struct ThreadPool
{
    Task* taskQ;        //任务队列
    int queueCapacity;  //容量
    int queueSize;      //当前任务个数
    int queueFront;     //队头--->取数据
    int queueRear;      //队尾--->放数据

    pthread_t manageID; //管理线程的线程ID
    pthread_t *threadIDs;//工作的线程ID数组，用来保存所有工作线程的ID
    int minNum;          //线程池最少保留多少个工作线程
    int maxNum;          //线程池最多允许多少个工作线程
    int busyNum;         //当前有多少个线程正在干活
    int liveNum;         //当前存活的线程数
    int exitNum;         //管理线程通知“要退出的线程数量”
    pthread_mutex_t mutexPool;  //用来保护线程池里面的共享资源
    pthread_mutex_t mutexBusy;  //单独保护busyNum
    pthread_cond_t notFull;     //任务队列是否满
    pthread_cond_t notEmpty;    //任务队列是否空
    int shutdown;        //是不是要销毁线程池，销毁为1，不销毁为0

}ThreadPool;

//创建线程池，min:最小线程数，max:最大线程数，queuesize:任务队列容量
ThreadPool *threadPoolCreate(int min, int max, int queuesize)
{
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (pool == NULL) return NULL;
    pool->taskQ = NULL;
    pool->threadIDs = NULL;
    pool->threadIDs = (pthread_t*)malloc(sizeof(pthread_t) * max);
    if (pool->threadIDs == NULL)
    {
        free(pool);
        return NULL;
    }
    pool->taskQ = (Task*)malloc(sizeof(Task) * queuesize);
    if (pool->taskQ == NULL)
    {
        free(pool->threadIDs);
        free(pool);
        return NULL;
    }
    memset(pool->threadIDs, 0, sizeof(pthread_t) * max);
    pool->minNum = min;
    pool->maxNum = max;
    pool->busyNum = 0;
    pool->liveNum = min;
    pool->exitNum = 0;
    pool->queueCapacity = queuesize;
    pool->queueSize = 0;
    pool->queueFront = 0;
    pool->queueRear = 0;
    pool->shutdown = 0;
    pthread_mutex_init(&pool->mutexPool, NULL);
    pthread_mutex_init(&pool->mutexBusy, NULL);
    pthread_cond_init(&pool->notEmpty, NULL);
    pthread_cond_init(&pool->notFull, NULL);
    pthread_create(&pool->manageID, NULL, manager, pool);
    for (int i = 0; i < min; i++)
    {
        pthread_create(&pool->threadIDs[i], NULL, worker, pool);
        pthread_detach(pool->threadIDs[i]);
    }
    return pool;
}

int threadPoolDestroy(ThreadPool *pool)
{
    if(pool == NULL)
    {
        return -1;
    }
    pthread_mutex_lock(&pool->mutexPool);
    //关闭线程池(关闭后，管理者线程就自动退出)
    pool->shutdown = 1;
    pthread_mutex_unlock(&pool->mutexPool);
    //阻塞回收管理者线程
    pthread_join(pool->manageID, NULL);
    //唤醒阻塞的消费者线程
    for(int i = 0; i < pool->liveNum; i++)
    {
        pthread_cond_signal(&pool->notEmpty);
    }
    pthread_mutex_destroy(&pool->mutexPool);
    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_cond_destroy(&pool->notEmpty);
    pthread_cond_destroy(&pool->notFull);
    free(pool->taskQ);
    free(pool->threadIDs);
    free(pool);
    pool = NULL;
    return 0;
}

//往线程池的任务队列添加一个新任务
//三个参数分别代表：线程池指针（要把任务加到哪个线程池），函数指针（这个函数要执行的任务），传给任务函数的参数
void threadPoolAdd(ThreadPool *pool, void (*func)(void *), void *arg)
{
    //先把线程池锁住
    pthread_mutex_lock(&pool->mutexPool);
    //要是当前任务数量等于队列容量（放不下新任务）同时线程池还没有关闭
    while(pool->queueSize == pool->queueCapacity && !pool->shutdown)
    {
        //就先让当前这个“加任务”的线程别加，先等着
        pthread_cond_wait(&pool->notFull, &pool->mutexPool);
    }
    //如果线程池关闭，就不再加任务
    if(pool->shutdown)
    {
        //先解锁后直接返回
        pthread_mutex_unlock(&pool->mutexPool);
        return;
    }
    //添加任务，将任务放到队尾
    pool->taskQ[pool->queueRear].function = func;
    pool->taskQ[pool->queueRear].arg = arg;
    //队尾后移
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
    //任务数量加1
    pool->queueSize++;
    //通知工作线程，现在队列非空了
    pthread_cond_signal(&pool->notEmpty);
    //解锁
    pthread_mutex_unlock(&pool->mutexPool);
}

int threadPoolBusyNum(ThreadPool *pool)
{
    pthread_mutex_lock(&pool->mutexBusy);
    int busyNum = pool->busyNum;
    pthread_mutex_unlock(&pool->mutexBusy);
    return busyNum;
}

int threadPoolaliveNum(ThreadPool *pool)
{
    pthread_mutex_lock(&pool->mutexPool);
    int liveNum = pool->liveNum;
    pthread_mutex_unlock(&pool->mutexPool);
    return liveNum;
}

// 工作线程函数
void *worker(void *arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    //工作线程一直循环工作，除非被要求退出
    while(1)
    {
        //给线程池加锁，因为后面要访问共享数据：任务队列、线程数等
        pthread_mutex_lock(&pool->mutexPool);
        //如果任务队列为空，并且线程池还没关闭，就进入等待
        //用while而不是if,因为被唤醒后要再次检查条件
        while(pool->queueSize == 0 && !pool->shutdown)
        {
            //阻塞工作线程,当前线程等待条件变量notEmpty,意思是：等到"任务队列非空"再继续
            //这个函数会：自动释放mutexPool,进入阻塞，被唤醒后重新加锁，返回
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
            //被唤醒后检查是否有“退出名额”，如果exitNum > 0，说明管理线程希望部分线程退出
            //当前线程先把这个名额减1
            if(pool->exitNum > 0 && pool->liveNum > pool->minNum)
            {
                pool->exitNum--;
                //只有当前存货线程数大于最小线程数时，线程才会被允许退出
                //存活线程数减1，
                pool->liveNum--;
                //退出前先解锁
                pthread_mutex_unlock(&pool->mutexPool);
                //真正执行线程退出逻辑
                threadExit(pool); 
            }
        }
        //判断当前线程池是否关闭，如果线程池关闭了，先解锁，然后线程退出
        if(pool->shutdown)
        {
            pool->liveNum--;
            pthread_mutex_unlock(&pool->mutexPool);
            threadExit(pool);
        }
        //从任务队列中取出一个任务
        Task task;
        //先找到队头的任务，再从这个任务里取出它的function
        task.function = pool->taskQ[pool->queueFront].function;
        //找到任务队头的任务，再从这个任务里面取出它的参数
        task.arg = pool->taskQ[pool->queueFront].arg;
        //队头向后移动一格，因为是循环队列，所以要%queueCapacity，同时任务数量减1
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
        pool->queueSize--;
        pthread_cond_signal(&pool->notFull);
        //任务已经取出来了，后面执行任务不需要继续持有队列锁，所以解锁，减少锁竞争
        pthread_mutex_unlock(&pool->mutexPool);
        //打印当前线程开始工作
        printf("thread %lu start working...\n", (unsigned long)pthread_self());
        //给busyNum加锁，表示当前线程开始忙起来了
        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexBusy);
        //真正执行任务，也就是调用任务函数，并把参数传进去
        task.function(task.arg);
        //执行完任务后释放任务参数的内存，然后把指针置空
        free(task.arg);
        task.arg = NULL;
        //打印线程工作结束
        printf("thread %lu end working...\n", (unsigned long)pthread_self());
        pthread_mutex_lock(&pool->mutexBusy);
        //任务执行完毕后，忙线程数量减1
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexBusy);

    }
    return NULL;
}

//管理线程函数
void *manager(void *arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    while(!pool->shutdown)
    {
        //每隔三秒检测一次
        sleep(3);
        //取出线程池中任务的数量和当前线程的数量
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->queueSize;
        int liveNum = pool->liveNum;
        pthread_mutex_unlock(&pool->mutexPool);

        //取出忙的线程的数量
        pthread_mutex_lock(&pool->mutexBusy);
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexBusy);

        //添加线程
        //任务的个数>存货的线程个数 && 存货的线程数 < 最大线程数,这说明线程可能不够，要增加线程
        if(queueSize > liveNum && liveNum < pool->maxNum)
        {
            //加锁，准备修改线程池状态
            pthread_mutex_lock(&pool->mutexPool);
            //counter记录本次已经新增了多少线程
            int counter = 0;
            //遍历线程ID数组
            //最多扫描到数组末尾，一次最多加NUMBER个线程，总线程数不能超过上限
            for(int i = 0; i < pool->maxNum && counter < NUMBER
                && pool->liveNum < pool->maxNum; i++)
            {
                //如果这个位置没有线程，就在这里创建新线程
                if(pool->threadIDs[i] == 0)
                {
                    //创建一个新线程
                    pthread_create(&pool->threadIDs[i], NULL, worker, pool);
                    pthread_detach(pool->threadIDs[i]);
                    //本次新线程数加1
                    counter++;
                    //总存货线程数加1
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }
        //销毁线程（如果忙线程太少，那就说明总的线程太多了，销毁线程的好处有：释放资源）
        //忙的线程乘以2 < 存活的线程数 && 存活的线程 > 最小的线程数
        if(busyNum*2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            //设置需要退出的线程数量为NUMBER，表示想让两个线程退出
            pool->exitNum += NUMBER;
            pthread_mutex_unlock(&pool->mutexPool);
            //让工作的线程自杀
            //发number次信号给notEmpty，作用是唤醒那些阻塞在pthread_cond_wait的工作线程
            //被唤醒后，他们会看到exitNum>0,然后自己退出
            for(int i = 0; i < NUMBER; i++)
            {
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    return NULL;
}

//线程退出函数
void threadExit(ThreadPool *pool)
{
    //获取当前线程自己的线程ID
    pthread_t tid = pthread_self();
    //遍历线程ID数组，找到当前线程在数组中的位置
    for(int i = 0; i < pool->maxNum; i++)
    {
        //如果数组中某个线程ID等于当前线程ID
        if(pthread_equal(pool->threadIDs[i], tid))
        {
            //把这个位置清零，表示这个线程已经退出
            pool->threadIDs[i] = 0;
            //打印退出信息
            printf("threadExit() called, %ld exiting...\n", (unsigned long)tid);
            break;
        }
    }
    //终止当前线程
    pthread_exit(NULL);
}