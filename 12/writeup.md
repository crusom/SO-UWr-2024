## Zadanie 1
```text
Zadanie 1. Zapoznaj się z poniższym programem. Rozważamy wartości przechowywane w zmiennych:
«myid», «strtab», «vargp», «cnt», «argc» i «argv[0]». Określ czy są one współdzielone i które z nich
będą źródłem wyścigów (ang. data race).
1 __thread long myid;
2 static char **strtab;
3
4 void *thread(void *vargp) {
5 myid = *(long *)vargp;
6 static int cnt = 0;
7 printf("[%ld]: %s (cnt=%d)\n", myid, strtab[myid], ++cnt);
8 return NULL;
9 }
10
11 int main(int argc, char *argv[]) {
12 ...
13 strtab = argv;
14 while (argc > 0) {
15 myid = --argc;
16 pthread_create(&tid, NULL, thread, (void *)&myid);
17 }
18 ...
19 }
```
myid - zmienna globalna, nie współdzielona  
strtab - zmienna globalna, wspoldzielona, nie jest źródłem wyścigu, bo tylko czytana  
vargp - zmienna lokalna, niewspoldzielona, dane na które wskazuje też są niewspółdzielone  
cnt - zmienna globalna, współdzielona, wyścig (linijka 7, nie ma kontroli dostępu)  
argc - zmienna lokalna, niewspółdzielone, używana tylko w main  
argv[0] - jest używane przez strtab, więc współdzielone, ale nie jest źródłem wyścigu, bo jest jedynie czytane

## Zadanie 2
```text
Zadanie 2. Podaj definicję sekcji krytycznej [ 2 , 6.2]. Następnie wymień i uzasadnij założenia jakie musi
spełniać rozwiązanie problemu sekcji krytycznej. Czemu w programach przestrzeni użytkownika do jej
implementacji nie możemy używać wyłączania przerwań (ang. interrupt disable)? Odwołując się do Prawa
Amdahla powiedz czemu programistom powinno zależeć na tym, by sekcje krytyczne były możliwie jak
najkrótsze – określa się to również mianem blokowania drobnoziarnistego (ang. fine-grained locking).
```

### Następnie wymień i uzasadnij założenia jakie musi spełniać rozwiązanie problemu sekcji krytyczne
1. wzajemne wykluczenie - tylko jeden proces moze wykonywac dana sekcje krytyczna
2. postep - jeśli żaden proces nie wykonuje sekcji krytycznej, a niektóre procesy chcą do niej wejść, to tylko te procesy które nie wykonują "remainder section" decydują kto wejdzie do sekcji krytycznej, a podjęcie tej decyzji nie może trwać w nieskończoność
3. granica czasu oczekiwania - istnieje limit ile razy inne procesy mogą wejść do sekcji krytycznej, po tym jak wysłaliśmy request żeby wejść do sekcji

### Czemu w programach przestrzeni użytkownika do jej implementacji nie możemy używać wyłączania przerwań (ang. interrupt disable)? 
możemy w środowisku jednoprocesorowym, w wieloprocesorowym jest opóźnienie w tym jak prędko inne procesory dostaną request o zatrzymanie, więc nie zadziała.

```text
Unfortunately, this solution is not as feasible in a multiprocessor environ-
ment. Disabling interrupts on a multiprocessor can be time consuming, since
the message is passed to all the processors. This message passing delays entry
into each critical section, and system efficiency decreases. Also consider the
effect on a system’s clock if the clock is kept updated by interrupts.
```

### Odwołując się do Prawa Amdahla powiedz czemu programistom powinno zależeć na tym, by sekcje krytyczne były możliwie jak najkrótsze – określa się to również mianem blokowania drobnoziarnistego (ang. fine-grained locking).
Sekcja krytyczna, to kod którego nie da się zrównoleglić, a chcemy móc zrównoleglić jak najwięcej kodu.


## Zadanie 3
```text
Zadanie 3. Podaj w pseudokodzie semantykę instrukcji atomowej compare-and-swap i przy jej pomocy
zaimplementuj blokadę wirującą (ang. spin lock) [3 , 28.7]. Niech typ «spin_t» będzie równoważny «int».
Podaj ciało procedur «void lock(spin_t *)» i «void unlock(spin_t *)».  
Czemu blokada wirująca nie jest sprawiedliwa (ang. fair ) [3 , 28.8]? 
Uruchamiamy n identycznych wątków. Kolejno każdy z nich wchodzi do sekcji krytycznej, po czym zostaje wywłaszczony przez jądro. Ile czasu zajmie wszystkim wątkom jednokrotne przejście przez sekcję krytyczną – algorytm planisty to round-robin, kwant czasu wynosi 1ms
```
```c
spin_t CompareAndSwap(spin_t *ptr, spin_t expected, spin_t new) {
  spin_t original = *ptr;
  if (original == expected)
      *ptr = new;
  return original;
}

typedef int spin_t;

void lock(spin_t *spin) {
    while (CompareAndSwap(spin, 0, 1) == 1); // spin
}
void unlock(spin_t *spin) {
    *spin = 0;
}
```
### Czemu blokada wirująca nie jest sprawiedliwa (ang. fair ) [3 , 28.8]?
Nie możęmy zapewnić, że wątek czekający na wejscie do sekcji krytycznej kiedykolwiek do niej wejdzie, zależy to od schedulera, któremu kolejnemu wątkowi da wejść do sekcji krytycznej, po tym jak poprzedni wątek z niej wyszedł

### Uruchamiamy n identycznych wątków. Kolejno każdy z nich wchodzi do sekcji krytycznej, po czym zostaje wywłaszczony przez jądro. Ile czasu zajmie wszystkim wątkom jednokrotne przejście przez sekcję krytyczną – algorytm planisty to round-robin, kwant czasu wynosi 1ms

1. wątek wykonuje sekcje krytyczną przez 1ms
2. wątek dostaje przerwanie
3. scheduling przechodzi cyklicznie przez całą reszte wątków które nic nie robią
4. wątek znów wykonuje sekcje krytyczną przez 1ms

załóżmy że czas wykonania sekcji krytycznej to jakieś k (w milisekundach)  
wtedy więc musimy wykonywac wątki przez k * n (milisekund), aby przejść przez jedną sekcję krytyczną.  
Aby przejść przez wszystkie sekcje krytyczne raz mamy więc k * n * n (milisekund)

## Zadanie 4
```text
Zadanie 4. Wiemy, że aktywne czekanie (ang. busy waiting) nie jest właściwym sposobem oczekiwania
na zwolnienie blokady. Powstaje zatem potrzeba stworzenia blokady usypiającej.  
Czemu oddanie czasu procesora funkcją «yield» [ 3 , 28.13] nie rozwiązuje wszystkich problemów, które mieliśmy z blokadami
wirującymi? 
Zreferuj implementację podaną w [ 3, 28.14]. Czemu jest ona niepoprawna bez użycia funkcji «setpark»?
```

### Czemu oddanie czasu procesora funkcją «yield» [ 3 , 28.13] nie rozwiązuje wszystkich problemów, które mieliśmy z blokadami
Nadal jesteśmy skazani na implementacje schedulera, który może nam zagłodzić któryś wątek oraz nadal musimy przejść przez wszystkie wątki

### Zreferuj implementację podaną w [ 3, 28.14]. Czemu jest ona niepoprawna bez użycia funkcji «setpark»?
ponieważ jeśli wątek zostanie wywłaszczony tuż przed wywołaniem park(), a w tym czasie wątek trzymający blokade się zwolni to uśpiony wątek będzie spał w nieskończoność (bo nie ma kto go obudzić), jest to wakeup/waiting race.  
Gdy użyjemy wcześniej setpark(), to jeśli nastąpi opisana wyżej sytuacja, to park() po prostu od razu powróci zamiast spac.


## Zadanie 5
```text
Zadanie 5. Podaj cztery warunki konieczne do zaistnienia zakleszczenia.  
Na podstawie [3 , 32.3] wyjaśnij w jaki sposób można przeciwdziałać zakleszczeniom (ang. deadlock prevention)?  
Narzędzie lockdep, stosowane w jądrze Linux i Mimiker, buduje graf skierowany, w którym wierzchołkami są klasy blokad.  
Jak lockdep wykrywa, że może wystąpić zakleszczenie? Z jakimi scenariuszami sobie nie radzi?
```

### Podaj cztery warunki konieczne do zaistnienia zakleszczenia.  
```text
• Mutual exclusion: Threads claim exclusive control of resources that
they require (e.g., a thread grabs a lock).
• Hold-and-wait: Threads hold resources allocated to them (e.g., locks
that they have already acquired) while waiting for additional re-
sources (e.g., locks that they wish to acquire).
• No preemption: Resources (e.g., locks) cannot be forcibly removed
from threads that are holding them.
• Circular wait: There exists a circular chain of threads such that each
thread holds one or more resources (e.g., locks) that are being re-
quested by the next thread in the chain
```
### Na podstawie [3 , 32.3] wyjaśnij w jaki sposób można przeciwdziałać zakleszczeniom (ang. deadlock prevention)?  
Zapobiec któremukolwiek z warunków.  
Żeby zapobiec **circular wait** wystarczy, że zawsze będziemy pobierać blokady (acquire locks) w tej samej kolejności.  
Czyli nasz wadliwy kod:
```c
Thread 1: Thread 2:
pthread_mutex_lock(L1); pthread_mutex_lock(L2);
pthread_mutex_lock(L2); pthread_mutex_lock(L1);
```
zamienimy na:
```c
Thread 1: Thread 2:
pthread_mutex_lock(L1); pthread_mutex_lock(L1);
pthread_mutex_lock(L2); pthread_mutex_lock(L2);
```

**Hold-and-wait** wystarczy pobrać wszystkie blokady na raz:
```c
 pthread_mutex_lock(prevention); // begin acquisition
2 pthread_mutex_lock(L1);
3 pthread_mutex_lock(L2);
4 ...
5 pthread_mutex_unlock(prevention); // end
```
Minusy to zmniejszenie współbieżności, bo pobieramy od razu locki a nie gdy są potrzebne


**No preemption**
```c
1 top:
2 pthread_mutex_lock(L1);
3 if (pthread_mutex_trylock(L2) != 0) {
4 pthread_mutex_unlock(L1);
5 goto top;
6 }
```
w tej metodzie nie blokujemy w kółko tylko próbujemy pobrać mutex, jak sie nie udało to zwalniamy poprzednio pobrane mutexy, więc któryś z wątków wreszcie zrobi progress.  
Istnieje jednak szansa, że oba wątki będą wykonywać się dosłownie równolegle i wzajemne blokowanie wystąpi za każdym razem, nazywamy to livelockiem.  
Żeby obejść ten problem można dodać np. małe losowe opoznienie do wątku, aby za którymś razem się udało.

**Mutual Exclusion**
Propozycja jest taka by w ogóle nie używać locków, tylko samych instrukcji atomowych, co wymaga silnego wsparcia hardware'owego.
np.:
```text
1 int CompareAndSwap(int *address, int expected, int new) {
2   if (*address == expected) {
3       *address = new;
4       return 1; // success
5   }
6   return 0; // failure
7 }
1 void AtomicIncrement(int *value, int amount) {
2   do {
3       int old = *value;
4   } while (CompareAndSwap(value, old, old + amount) == 0);
5 }
```
Próbujemy wbić się w moment, gdy żaden inny wątek w międzyczasie nie podmienił nam wartośći.

**Deadlock Avoidance via Scheduling**
Autor jako rozwiązanie uniknięcia zakleszczenia podaje mądre planowanie, które wie jakie procesory powinny wykonywać jakie wątki, tak aby nie doszło do deadlocka.
![./assets/deadlock_scheduling.png]()


### Narzędzie lockdep, stosowane w jądrze Linux i Mimiker, buduje graf skierowany, w którym wierzchołkami są klasy blokad. Jak lockdep wykrywa, że może wystąpić zakleszczenie? Z jakimi scenariuszami sobie nie radzi?
Dla każdej klasy blokady L lockdep posiada dwie listy.  
before list: lista blokad przejmowanych przed L  
after list: lista blokad przejmowanych w czasie gdy L jest przejęte 

Gdy wątek chce przejąć blokade L, to lockdep sprawdza czy któraś z blokad przyjętych przez wątek jest na after list, jeśli tak to została naruszona kolejność i potencjalnie może nastąpić deadlock.  
Z tego co rozumiem idea jest taka, że jeśli za jednym razem w czasie gdy blokada L była przejęta, została przejęta jakaś blokada L2, to nie powinna wystąpić sytuacja, że kolejnym razem gdy przejmujemy L, to L2 już jest przejęte.


Ponieważ lockdep korzysta z klas blokad, to nie bierze od uwage sytuacji gdy ten sam obiekt może mieć różne locking rules i podaje taki przykład:
```c
struct node_data {
    ...
    mutex_t lock;
};
struct tree_node {
    ...
    struct node_data *data;
    mutex_t lock;
};
void lock_tree_node_and_storage(struct tree_node *A) {
    if (is_root(A)) {
        mutex_lock(A->lock);
        mutex_lock(A->data->lock);
    } else {
        mutex_lock(A->data->lock);
        mutex_lock(A->lock);
    }
}
```
## Zadanie 6
```text
Zadanie 6. Poniżej znajduje się propozycja2 programowego rozwiązania problemu wzajemnego wykluczania
(ang. mutual exclusion) dla dwóch procesów. Znajdź kontrprzykład, w którym to rozwiązanie zawodzi.
1 shared boolean blocked [2] = { false, false };
2 shared int turn = 0;
3
4 void P (int id) {
5   while (true) {
6       blocked[id] = true;
7       while (turn != id) {
8           while (blocked[1 - id])
9               continue;
10          turn = id;
11      }
12 /* put code to execute in critical section here */
13      blocked[id] = false;
14  }
15 }
16
17 void main() { parbegin (P(0), P(1)); }
```

Pierwszy wykonuje się turn = 0, no chyba że wcześniej id = 1.
Co jeśli najpierw wykona się id = 1 aż do linii 10, a potem wywłaszczy się id = 0, który dojdzie do sekcji krytycznej?  
Po wznowieniu id = 1 też dojdzie do sekcji krytycznej, więc oba wątki się w nim znajdą.



## Zadanie 7
```text
Zadanie 7. Algorytm Petersona3 rozwiązuje programowo problem wzajemnego wykluczania. Zreferuj poniższą
wersję implementacji tego algorytmu dla dwóch procesów. Uzasadnij jego poprawność.
1 shared boolean blocked [2] = { false, false };
2 shared int turn = 0;
3
4 void P (int id) {
5   while (true) {
6       blocked[id] = true;
7       turn = 1 - id;
8       while (blocked[1 - id] && turn == (1 - id))
9           continue;
10      /* put code to execute in critical section here */
11      blocked[id] = false;
12  }
13 }
14
15 void main() { parbegin (P(0), P(1)); }
```

Jeśli oba procesy chcą wejść do sekcji krytycznej, to wejdzie ten którego jest kolej (turn).  
Jeśli jeden proces chce wejść, to zrobi to ponieważ drugi nie jest blocked.  
Mamy więc i mutual exclusion i postęp


## Zadanie 8
```text
Zadanie 8. Poniżej podano błędną implementację semafora zliczającego przy pomocy semaforów binar-
nych. Wartość «count» może być ujemna – wartość bezwzględna oznacza wtedy liczbę uśpionych procesów.
Znajdź kontrprzykład i zaprezentuj wszystkie warunki niezbędne do jego odtworzenia.
```

Binary semaphore: semaphore whose value is always 0 or 1
no więc delay nie bardzo może być semaforem binarnym.
Co jeśli przyjdą po sobie dwaj konsumenci i wywołają P?
Oba będą czekać w [ while (s == 0) wait(); s--; ].
Jeśli więc przyjdzie producent i coś wyprodukuje, to jeden z wątków uśpionych który



## Zadanie 9


A checks condition, it's not satisfied
B satisfies condition, wake A up
A goes to sleep/blocks



