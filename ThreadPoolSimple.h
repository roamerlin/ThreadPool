/**
 * @file ThreadPoolSimple.h
 * @author gaoxiaolin666@gmail.com
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// 执行 worker
typedef struct WORKER {
  pthread_t thead;  //线程ID
  int terminate;  //线程终止标识
  struct WORKERMANAGER *workManager; //池管理组件对象
  struct WORKER *prev;
  struct WORKER *next;
} nWorker;

// 任务 task
typedef struct TASK {
  void (*taskFunction)(struct TASK *task);  //任务回调函数
  void *taskData; //任务执行的参数
  struct TASK *prev;
  struct TASK *next;
} nTask;

// 池管理组件 manager
typedef struct WORKERMANAGER {
  struct WORKER *workers; //worker队列
  struct TASK *tasks;  //task队列
  pthread_mutex_t taskMutex;
  pthread_cond_t taskCond;
} nWorkerManager;

typedef nWorkerManager nThreadPool;

// 创建线程池
int nThreaPoolCreate(nThreadPool *workmanager, int numworkers);

// 销毁线程池
void nThreadPoolDestory(nThreadPool *workmanager);

// 往线程池中添加任务
void nThreadPoolAddTask(nThreadPool *workmanager, nTask *task);

#endif