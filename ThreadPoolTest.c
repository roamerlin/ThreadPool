/**
 * @file ThreadPoolTest.c
 * @author gaoxiaolin666@gmail.com
 * @brief thread pool test
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#define THREAD_POOL_MAX_SIZE 80
#define TASK_LIST_MAX_SIZE 1000

#include "ThreadPoolSimple.h"


void printInfo(nTask *task) {
  int index = *(int *)task->taskData;

  printf("index:%d, tid:%lu\n", index, pthread_self());
  free(task->taskData);
  free(task);
}

int main(int argc, char **argv) {
  nThreadPool workermanager;
  int ret = nThreaPoolCreate(&workermanager, THREAD_POOL_MAX_SIZE);
  if (ret) {
    printf("nThreaPoolCreate failed !\n");
    return -1;
  }

  nTask *task = NULL;
  for (int i = 0; i < TASK_LIST_MAX_SIZE; i++) {
    task = (nTask *)malloc(sizeof(nTask));
    if (task == NULL) {
      perror("malloc");
      return -1;
    }

    task->taskFunction = printInfo;
    task->taskData = malloc(sizeof(int));
    *(int *)task->taskData = i;

    nThreadPoolAddTask(&workermanager, task);
  }

  nThreadPoolDestory(&workermanager);

  getchar();
  printf("\n");
  return 0;
}