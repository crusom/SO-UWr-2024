#include "csapp.h"

// check if all threads used
// watch -n 0 ps H -p $(pgrep ride)

static __thread unsigned seed;

static void rand_usleep(int min, int max) {
//  return;
  usleep(rand_r(&seed) % (max - min + 1) + min);
}

#define DEBUG
#ifdef DEBUG
static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

/* XXX Please use following function to simulate malicious scheduler.
 * Just insert a call to rand_yield between instructions in your solution. */
static __unused void rand_yield(void) {
  /* Once every 100 calls to this function (on average)
   * it yields and lets kernel choose another thread. */
  if (rand_r(&seed) % 100 == 42)
    sched_yield();
}
#else
#define outc(c)
#define rand_yield()
#endif
#define PASSENGERS 50
#define SEATS 10

typedef struct ride {
  /* TODO: Put internal state & mutexes & condvars here. */
  pthread_mutex_t mutex;
  pthread_cond_t queue;
  pthread_cond_t cart;
  pthread_cond_t want_to_leave;
  sem_t passengers_ready_to_leave;
  sem_t all_ready;
  int waiting;
  int seats;
  int want_to_leave_count;
  int riding;
  bool must_wait;
} ride_t;

static void ride_init(ride_t *r, unsigned seats) {
  pthread_cond_init(&r->queue, NULL);
  sem_init(&r->all_ready,0,0);
  pthread_cond_init(&r->cart, NULL);
  pthread_cond_init(&r->want_to_leave, NULL);
  sem_init(&r->passengers_ready_to_leave, 0, 0);
  pthread_mutex_init(&r->mutex, NULL);
  r->seats = seats;
  r->riding = 0;
  r->waiting = 0;
  r->must_wait = false;
}

static void ride_destroy(ride_t *r) {
  /* TODO: Destroy all synchronization primitives. */
  pthread_mutex_destroy(&r->mutex);
  pthread_cond_destroy(&r->queue);
  sem_destroy(&r->all_ready);
  pthread_cond_destroy(&r->cart);
  pthread_cond_destroy(&r->want_to_leave);
  sem_destroy(&r->passengers_ready_to_leave);
}

static void cart_load(ride_t *r) {
  /* TODO: Wait for all seats to be taken and depart. */
  pthread_mutex_lock(&r->mutex);
  int min = (r->waiting < r->seats) ? r->waiting : r->seats;
  // zwolnijmy najpierw tych którzy już czekają
  for (int i = 0; i < min; i++)
    pthread_cond_signal(&r->queue);

  pthread_mutex_unlock(&r->mutex);
  // szczyt synchronizacji
  while (r->riding != min) continue;
  pthread_mutex_lock(&r->mutex);
  
  // opróżniliśmy kolejke, dajmy wejść kolejnym
  if (min < r->seats)
    r->must_wait = false;
  
  printf("min:%d,riding:%d",min, r->riding);
  fflush(stdout);
  outc('W');

  pthread_mutex_unlock(&r->mutex);
  sem_wait(&r->all_ready);
  pthread_mutex_lock(&r->mutex);
  // letsgooo
  for (int i = 0; i < r->riding; i++)
    pthread_cond_signal(&r->cart);

  pthread_mutex_unlock(&r->mutex);
}

static void cart_unload(ride_t *r) {
  /* TODO: Wait for all passangers to leave and be ready for next ride. */
  // czekamy az wszyscy pasazerowie chca wyjsc
  sem_wait(&r->passengers_ready_to_leave);
  pthread_mutex_lock(&r->mutex);
  //printf("sem_wait\n");
  // pozwalamy im wyjsc
  for (int i = 0; i < r->riding; i++)
    pthread_cond_signal(&r->want_to_leave);

  r->riding = 0;
  pthread_mutex_unlock(&r->mutex);
}

static void passenger_board(ride_t *r) { 
  /* TODO: Wait for the cart, enter it and wait for the ride to begin. */
  pthread_mutex_lock(&r->mutex);
  if (r->riding == r->seats || r->must_wait) {
    r->waiting++;
    pthread_cond_wait(&r->queue, &r->mutex);
    r->waiting--;
  }
  // heyyy im entering the cart ^^
  r->riding++;
  outc('I');
  assert(r->riding <= r->seats);
  if (r->riding == r->seats) {
    r->must_wait = true;
    sem_post(&r->all_ready);
  }
  pthread_cond_wait(&r->cart, &r->mutex);

  pthread_mutex_unlock(&r->mutex);
}

static void passenger_unboard(ride_t *r) {
  /* TODO: Wait for the ride to end and leave the cart. */
  pthread_mutex_lock(&r->mutex);
  r->want_to_leave_count++;
  if (r->want_to_leave_count == r->riding) {
    sem_post(&r->passengers_ready_to_leave);
    r->want_to_leave_count = 0;
  }
  pthread_cond_wait(&r->want_to_leave, &r->mutex);

  pthread_mutex_unlock(&r->mutex);
}

static void *cart(void *data) {
  ride_t *r = data;

  seed = (unsigned)pthread_self();

  while (true) {
    outc('L');
    cart_load(r);

    rand_usleep(1000, 2000);

    outc('U');
    cart_unload(r);
  }

  return NULL;
}

static void *passenger(void *data) {
  ride_t *r = data;

  seed = (unsigned)pthread_self();

  while(true) {   
    outc('-');
    passenger_board(r);

    rand_usleep(500, 1000);

    outc('!');
    passenger_unboard(r);

    outc('+');
    rand_usleep(2000, 4000);
  }

  return NULL;
}


int main(void) {
  ride_t r;
  ride_init(&r, SEATS);

  pthread_t cartThread;
  pthread_t passengerThread[PASSENGERS];

  pthread_create(&cartThread, NULL, cart, &r);
  for (int i = 0; i < 50; i++)
    pthread_create(&passengerThread[i], NULL, passenger, &r);

  pthread_join(cartThread, NULL);
  for (int i = 0; i < 50; i++)
    pthread_join(passengerThread[i], NULL);

  ride_destroy(&r);
  return 0;
}
