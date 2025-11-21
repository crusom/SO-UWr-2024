#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

typedef struct {
  /* TODO: Use this structure to store barrier internal state. */
  sem_t turnsnile;
  sem_t turnsnile2;
  sem_t mutex;
  int waiting;
  int n;
} barrier_t;

static barrier_t *barrier_init(int n) {
  if (n < 1)
    app_error("barrier_init requires n > 0");

  barrier_t *b = Mmap(NULL, sizeof(barrier_t), PROT_READ|PROT_WRITE,
                      MAP_ANON|MAP_SHARED, -1, 0);

  /* TODO: Initialize barrier internal state. */
  sem_init(&b->turnsnile, 1, 0);
  sem_init(&b->turnsnile2, 1, 1);
  sem_init(&b->mutex, 1, 1);
  b->waiting = 0;
  b->n = n;
  return b;
}

static void barrier_wait(barrier_t *b) {
  /* TODO: Provide wait procedure implementation here. */
  sem_wait(&b->mutex);
    b->waiting++;
    if (b->waiting == b->n) {
      sem_wait(&b->turnsnile2);
      sem_post(&b->turnsnile);
    }
  sem_post(&b->mutex);

  sem_wait(&b->turnsnile);
  sem_post(&b->turnsnile);

  sem_wait(&b->mutex);
    b->waiting--;
    if (b->waiting == 0) {
      sem_wait(&b->turnsnile);
      sem_post(&b->turnsnile2);
    }
  sem_post(&b->mutex);

  sem_wait(&b->turnsnile2);
  sem_post(&b->turnsnile2);


  sem_wait(&b->mutex);
    b->waiting++;
  sem_post(&b->mutex);
  if (b->waiting == b->n)
    sem_post(&b->turnsnile);

  sem_wait(&b->turnsnile);
  sem_post(&b->turnsnile);
}

static void barrier_destroy(barrier_t *b) {
  /* TODO: Provide destroy procedure implementation here. */
  munmap(b, sizeof(barrier_t));
}

#define K 100
#define N 50
#define P 100

static noreturn void horse(barrier_t *b) {
  int n = rand() % K + K;


  // jeden koń biegnie n okrążeń, potem jest zastąpiony przez innego konia
  for (int i = 0; i < n; i++) {
  outc('+');
    barrier_wait(b);
  outc('-');
    //if (i == 0)
    //  outc('S');
    //else
    //  outc('R');
    usleep(rand() % 2000 + 1000);
  }


  exit(EXIT_SUCCESS);
}

/* Do not bother cleaning up after this process. Let's assume that controlling
 * terminal sends SIGINT to the process group on CTRL+C. */
int main(void) {
  barrier_t *b = barrier_init(N);

  int horses = 0;

  //for (;;) {
  for (int i = 0; i < 10; i++) {
    do {
      if (Fork() == 0) {
        srand(getpid());
        horse(b);
      }
      horses++;
    } while (horses < P);

    Wait(NULL);
    horses--;
  }

  barrier_destroy(b);

  return EXIT_SUCCESS;
}
