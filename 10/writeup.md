
## Zadanie 1
```text
Zadanie 1. Czym różni się przetwarzanie równoległe (ang. parallel) od przetwarzania współbieżnego
(ang. concurrent)? Czym charakteryzują się procedury wielobieżne (ang. reentrant)? Podaj przykład
procedury w języku C (a) wielobieżnej, ale nie wielowątkowo-bezpiecznej (ang. MT-safe) (b) na odwrót.
Kiedy w jednowątkowym procesie uniksowym może wystąpić współbieżność?
```
### Czym różni się przetwarzanie równoległe (ang. parallel) od przetwarzania współbieżnego (ang. concurrent)?
Przetwarzanie równoległe jest wtedy gdy dzieje się kilka różnych rzeczy (na przykład działa kilka różnych wątków), ale niekonicznie w tym samym czasie (time-slicing itp.).  
Przetwarzanie współbieżne kiedy mamy kilka procesorów i rzeczy dosłownie dzieją się w tym samym czasie (na odrębnych procesorach).  

```text
Concurrency                 Concurrency + parallelism
(Single-Core CPU)           (Multi-Core CPU)
 ___                         ___ ___
|th1|                       |th1|th2|
|   |                       |   |___|
|___|___                    |   |___
    |th2|                   |___|th2|
 ___|___|                    ___|___|
|th1|                       |th1|
|___|___                    |   |___
    |th2|                   |   |th2|
```


### Czym charakteryzują się procedury wielobieżne (ang. reentrant)?  
Procedura wielobieżna to taka procedura, która może zostać przerwana i wywołana jeszcze raz.  
W szczególności są to signal handlery.  

### Podaj przykład procedury w języku C (a) wielobieżnej, ale nie wielowątkowo-bezpiecznej (ang. MT-safe) (b) na odwrót.

a)  
```c
/* We save the global state in a local variable and we restore
   it at the end of the function.  The function is now reentrant
   but it is not thread safe. */

int t;

void swap(int *x, int *y)
{
    int s;
    s = t;
    t = *x;
    *x = *y;
    *y = t;
    t = s;
}
```
b)  
```c
/* We use a thread local variable: the function is now
   thread-safe but still not reentrant (within the
   same thread). */

__thread int t;

void swap(int *x, int *y)
{
    t = *x;
    *x = *y;
    *y = t;
}
```
Źródło:  [](https://stackoverflow.com/a/33445858)
### Kiedy w jednowątkowym procesie uniksowym może wystąpić współbieżność?
Obsługa przerwań jest współbieżna de facto, przerwanie może przyjść w dowolnym momencie i wykonujemy wtedy inny kawałek kodu  


## Zadanie 2
```text
Zadanie 2. Wybierz odpowiedni scenariusz zachowania wątków, w którym konkurują o dostęp do zasobów,
i na tej podstawie precyzyjnie opisz zjawisko zakleszczenia (ang. deadlock), uwięzienia (ang. livelock)
oraz głodzenia (ang. starvation).
```

Deadlock - wątek A czeka na zwolnienie zasobu przez wątek B, ale wątek B czeka na zwolnienie zasobu przez wątek A. 

![](./assets/deadlock.png)

Livelock - jak deadlock, ale oba wątki zmieniają swój stan, tak że nadal się blokują. Przykładem jest sytuacja gdzie 2 osoby spotykają się w wąskim korytarzu i jedna drugiej chce ustąpić w tym samym czasie.  
Starvation - scheduling lub eventy odbywają się w taki sposób, że wątek nigdy nie może się ruszyć

### Zadanie 3
```text
Zadanie 3. W poniższym programie występuje sytuacja wyścigu (ang. race condition) dotycząca dostępów
do współdzielonej zmiennej «tally». Wyznacz jej najmniejszą i największą możliwą wartość.
1 const int n = 50;
2 shared int tally = 0;
3
4 void total() {
5 for (int count = 1; count <= n; count++)
6 tally = tally + 1;
7 }
8
9 void main() { parbegin (total(), total()); }
Dyrektywa «parbegin» rozpoczyna współbieżne wykonanie procesów. Maszyna wykonuje instrukcje arytme-
tyczne wyłącznie na rejestrach – tj. kompilator musi załadować wartość zmiennej «tally» do rejestru, przed
wykonaniem dodawania. Jak zmieni się przedział możliwych wartości zmiennej «tally», gdy wystartujemy
k procesów zamiast dwóch? Odpowiedź uzasadnij pokazując przeplot, który prowadzi do określonego wyniku
```
```asm
; jesli za kazdym razem drugi proces wykona ciag tych 3 instrukcji, przed tym jak pobralismy stara wartosc, to bedziemy miec max, czyli 100
mov rax, [tally]
; jesli za kazdym razem drugi proces wykona ciag tych 3 instrukcji, po tym jak pobralismy stara wartosc, to bedziemy miec minimum czyli 50
add rax, 1
mov [tally], rax
```


### Zadanie 4
```text
Zadanie 4. Podaj procedury wątków POSIX, które pełnią podobną funkcję co fork(2), exit(3), waitpid(2),
atexit(3) oraz abort(3) na procesach.  
Opisz semantykę podanych funkcji i powiedz gdzie się różnią od swoich odpowiedników na procesach.  
Porównaj zachowanie wątków złączalnych (ang. joinable) i odczepionych (ang. detached).  
Zauważ, że w systemie Linux procedura pthread_create odpowiada za utworzenie reprezentacji wątku w przestrzeni użytkownika, 
w tym utworzenie stosu i uruchomienie wątku przy pomocy wywołania clone(2).  
Kto zatem odpowiada za usunięcie segmentu stosu z przestrzeni użytkownika, gdy wątek złączalny albo odczepiony zakończy pracę? 
Pomocne może być zajrzenie do implementacji funkcji pthread_exit, pthread_join i __unmapself
```

