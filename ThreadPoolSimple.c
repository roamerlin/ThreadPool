/**
 * @file ThreadPoolSimple.c
 * @author gaoxiaolin666@gmail.com
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "ThreadPoolSimple.h"

#define LL_ADD(item, list) \
  do {                     \
    item->prev = NULL;     \
    item->next = list;     \
    list = item;           \
  } while (0)

#define LL_REMOVE(item, list)                              \
  do {                                                     \
    if (item->prev != NULL) item->prev->next = item->next; \
    if (item->next != NULL) item->next->prev = item->prev; \
    if (list == item) list = item->next;                   \
    item->prev = item->next = NULL;                        \
  } while (0)

// 线程入口函数
static void *nWorkerTheadFunc(void *arg) {
  nWorker *worker = (nWorker *)arg;
  while (1) {
    pthread_mutex_lock(&worker->workManager->taskMutex);
    while (worker->workManager->tasks == NULL) {
      if (worker->terminate) break;
      pthread_cond_wait(&worker->workManager->taskCond,
                        &worker->workManager->taskMutex);
    }

    if (worker->terminate) {
      pthread_mutex_unlock(&worker->workManager->taskMutex);
      break;
    }

    nTask *task = worker->workManager->tasks;
    if (task != NULL) {
      LL_REMOVE(task, worker->workManager->tasks);
    }

    pthread_mutex_unlock(&worker->workManager->taskMutex);

    if (task == NULL) continue;

    task->taskFunction(task);
  }

  free(worker);
  pthread_exit(NULL);
}

// 创建线程池
int nThreaPoolCreate(nThreadPool *workmanager, int numworkers) {
  if(!workmanager) return 1;
  if (numworkers < 1) numworkers = 1;

  memset(workmanager, 0, sizeof(nThreadPool));
  pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
  memcpy(&workmanager->taskMutex, &blank_mutex, sizeof(pthread_mutex_t));

  pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
  memcpy(&workmanager->taskCond, &blank_cond, sizeof(pthread_cond_t));

  nWorker *worker = NULL;
  for (int i = 0; i < numworkers; i++) {
    worker = (nWorker *)malloc(sizeof(nWorker));
    if (worker == NULL) {
      perror("malloc");
      return 1;
    }
    memset(worker, 0, sizeof(nWorker));
    worker->workManager = workmanager;

    int ret = pthread_create(&worker->thead, NULL, nWorkerTheadFunc,
                             (void *)worker);
    if (ret) {
      perror("pthread_create");
      free(worker);
      return 1;
    }

    LL_ADD(worker, worker->workManager->workers);
  }

  return 0;
}

// 销毁线程池
void nThreadPoolDestory(nThreadPool *workmanager) {
  nWorker *worker = NULL;
  for (worker = workmanager->workers; worker != NULL; worker = worker->next) {
    worker->terminate = 1;
  }

  pthread_mutex_lock(&workmanager->taskMutex);
  workmanager->workers = NULL;
  workmanager->tasks = NULL;
  pthread_cond_broadcast(&workmanager->taskCond);
  pthread_mutex_unlock(&workmanager->taskMutex);
}

// 往线程池中添加任务
void nThreadPoolAddTask(nThreadPool *workmanager, nTask *task) {
  pthread_mutex_lock(&workmanager->taskMutex);
  LL_ADD(task, workmanager->tasks);
  pthread_cond_signal(&workmanager->taskCond);
  pthread_mutex_unlock(&workmanager->taskMutex);
}
