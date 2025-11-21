#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Sem { pthread_mutex_t mutex; pthread_cond_t waiters; int value; } Sem_t;
void init(Sem_t *sem, int val) {
  if (val < 0) {
    fprintf(stderr, "init value can't be negative!\n");
    exit(-1);
  }
  sem->value = val;

  pthread_cond_init(&sem->waiters, NULL); // "the cond_attr parameter is actually ignored."
  pthread_mutex_init(&sem->mutex, NULL);  // there're 3 attributs 'fast', 'recursive' and 'error checking' and fast is the default one  
}
// P (decrease)
void wait(Sem_t *sem) {
  pthread_mutex_lock(&sem->mutex);
  while (sem->value == 0)
    pthread_cond_wait(&sem->waiters, &sem->mutex);
    // have somebody produced a new item?
  
  // we take the item!
  sem->value--;
  pthread_mutex_unlock(&sem->mutex);
}
// V (increase)
void post(Sem_t *sem) {
  pthread_mutex_lock(&sem->mutex);
  // i assume there's no buffer size limit cause it's not specified in the assignment
  sem->value++;
  // If no threads are waiting on cond, nothing happens
  pthread_cond_signal(&sem->waiters);
  pthread_mutex_unlock(&sem->mutex);
}
