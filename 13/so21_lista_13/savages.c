#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

#define N 100
#define M 100

// dosłownie producent konsument
static struct {
  /* TODO: Put semaphores and shared variables here. */
  int meals;
  sem_t mutex;
  sem_t empty;
  sem_t full;
} *shared = NULL;


static void savage(void) {
  for (;;) {
    /* TODO Take a meal or wait for it to be prepared. */
    sem_wait(&shared->mutex);
    if (shared->meals == 0) {
      sem_post(&shared->empty); // wake up cook
      sem_wait(&shared->full); // wait for full
    }
    outc('E');
    shared->meals--;
    sem_post(&shared->mutex);
    /* Sleep and digest. */
    usleep(rand() % 1000 + 1000);
  }

  exit(EXIT_SUCCESS);
}

static void cook(void) {
  for (;;) {
    /* TODO Cook is asleep as long as there are meals.
     * If woken up they cook exactly M meals. */
     sem_wait(&shared->empty);
     outc('C');
     // make meals
     shared->meals = M;
     sem_post(&shared->full);
  }
}

/* Do not bother cleaning up after this process. Let's assume that controlling
 * terminal sends SIGINT to the process group on CTRL+C. */
int main(void) {
  shared = Mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED,
                -1, 0);

  /* TODO: Initialize semaphores and other shared state. */
  sem_init(&shared->mutex, 1, 1);
  sem_init(&shared->empty, 1, 0);
  sem_init(&shared->full, 1, 0);
  shared->meals = 0;

  for (int i = 0; i < N; i++)
    if (Fork() == 0)
      savage();

  cook();

  return EXIT_SUCCESS;
}
