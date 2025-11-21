
## zadanie 1
```
Zadanie 1. W systemach uniksowych wszystkie procesy są związane relacją rodzic-dziecko.  
Uruchom polecenie «ps -eo user,pid,ppid,pgid,tid,pri,stat,wchan,cmd».  
Na wydruku zidentyfikuj identyfikator procesu, identyfikator grupy procesów, identyfikator rodzica oraz właściciela procesu.  
Kto jest rodzicem procesu init? Wskaż, które z wyświetlonych zadań są wątkami jądra.  
Jakie jest znaczenie poszczególnych znaków w kolumnie STAT?  
Wyświetl drzewiastą reprezentację hierarchii procesów poleceniem pstree – które z zadań są wątkami?
```


### Kto jest rodzicem procesu init? Wskaż, które z wyświetlonych zadań są wątkami jądra.  
```sh
crusom@crusomcarbon 1]$ ps -eo user,pid,ppid,pgid,tid,pri,stat,wchan,cmd | grep init | head -n1
root           1       0       1       1  19 Ss   -      /sbin/init
```

PID 0 jest procesem który uruchamia kernel.
Rodzicem procesu init jest proces o id 0, czyli Scheduler.  
```text
Process ID Description:
0 The Scheduler
1 The init process
2 kflushd
3 kupdate
4 kpiod
5 kswapd
6 mdrecoveryd
```
Tak naprawde jest to duze uproszczenie i jest to bardziej skomplikowane, a [ten artykuł robi fajny dive in](https://blog.dave.tf/post/linux-pid0/).

```text
PID 0 runs early kernel initialization, then becomes the bootstrap CPU core’s idle task, and plays a minor supporting role in scheduling and power management.
```

Aby znaleźć wątki należące do jądra, należy wyszukać te wątki, których grupa ma id 0 (czyli `root`).  
```sh
ps -eo user,pid,ppid,pgid,tid,pri,stat,wchan,cmd | awk '$4 == 0'
```

### Jakie jest znaczenie poszczególnych znaków w kolumnie STAT?  

Stat informuje nas o stanie procesu, poniżej jest tabela z man page'a ps:
```text
D    uninterruptible sleep (usually IO)
I    Idle kernel thread
R    running or runnable (on run queue)
S    interruptible sleep (waiting for an event to complete)
T    stopped by job control signal
t    stopped by debugger during the tracing
W    paging (not valid since the 2.6.xx kernel)
X    dead (should never be seen)
Z    defunct ("zombie") process, terminated but not reaped by its parent
```

### Wyświetl drzewiastą reprezentację hierarchii procesów poleceniem pstree – które z zadań są wątkami?
pstree oznacza wątki w nawiasach klamrowych.  
Możemy porównać output (częściowy) uruchamiając samo pstree i uruchamiając pstree z flagą -T, która nie ukazuje wątków,
a jedynie procesy.  
Diff nam ukaże coś takiego (to tylko część outputu, dla przykładu)
```
3,21c3,14
<         |-bash---firefox-+-Isolated Servic---30*[{Isolated Servic}]
<         |                |-2*[Isolated Web Co---35*[{Isolated Web Co}]]
<         |                |-12*[Isolated Web Co---27*[{Isolated Web Co}]]
<         |                |-9*[Isolated Web Co---26*[{Isolated Web Co}]]
<         |                |-3*[Isolated Web Co---30*[{Isolated Web Co}]]
<         |                |-Isolated Web Co---38*[{Isolated Web Co}]
<         |                |-2*[Isolated Web Co---28*[{Isolated Web Co}]]
<         |                |-MainThread---3*[{MainThread}]
<         |                |-Privileged Cont---28*[{Privileged Cont}]
<         |                |-RDD Process---7*[{RDD Process}]
<         |                |-Socket Process---5*[{Socket Process}]
<         |                |-Utility Process---7*[{Utility Process}]
<         |                |-Web Content---19*[{Web Content}]
<         |                |-2*[Web Content---18*[{Web Content}]]
<         |                |-WebExtensions---29*[{WebExtensions}]
<         |                |-file:// Content---27*[{file:// Content}]
<         |                `-154*[{firefox}]
<         |-colord---3*[{colord}]
<         |-containerd---12*[{containerd}]
---
>         |-bash---firefox-+-Isolated Servic
>         |                |-29*[Isolated Web Co]
>         |                |-MainThread
>         |                |-Privileged Cont
>         |                |-RDD Process
>         |                |-Socket Process
>         |                |-Utility Process
>         |                |-3*[Web Content]
>         |                |-WebExtensions
>         |                `-file:// Content
>         |-colord
>         |-containerd
```
## zadanie 2
```text
Zadanie 2. Jak jądro systemu reaguje na sytuację kiedy proces staje się sierotą?  
W jaki sposób pogrzebać proces, który wszedł w stan zombie?  
Czemu proces nie może sam siebie pogrzebać?  
Zauważ, że proces może, przy pomocy waitpid(2), czekać na zmianę stanu wyłącznie swoich dzieci.  
Co złego mogłoby się stać, gdyby znieść to ograniczenie?  
Rozważ scenariusze (a) dziecko może czekać na zmianę stanu swojego rodzica (b) wiele procesów oczekuje na zmianę stanu jednego procesu.

Wskazówka: Proces wykonujący w jądrze implementację wywołania systemowego _exit(2) nie może zwolnić stosu jądra, na którym się wykonuje. Kto zatem musi to zrobić?
```

### Jak jądro systemu reaguje na sytuację kiedy proces staje się sierotą?  
Proces osierocony dostaje ppid 1, czyli proces init.

demo:
```c
// some imports, look at 2.c
 int main(void) {
  pid_t pid = fork();
  switch (pid) {
  case -1:
      perror("fork");
      exit(EXIT_FAILURE);
  case 0:
      puts("Child: living my best life :)");
      printf("Child: my parent is %u\n", getppid());
      sleep(2);
      printf("Child: my parent died, now my ppid is %u\n", getppid());
      puts("Child exiting.");
      exit(EXIT_SUCCESS);
  default:
      printf("Child is PID %jd\n", (intmax_t) pid);
      puts("Parent: exiting.");
      exit(EXIT_SUCCESS);
  }
 }
```

```sh
[crusom@crusomcarbon 1]$ ./a.out
Child is PID 319823
Parent: exiting.
Child: living my best life :)
Child: my parent is 319822
[crusom@crusomcarbon 1]$ Child: my parent died, now my ppid is 1
Child exiting.
```

### W jaki sposób pogrzebać proces, który wszedł w stan zombie?  
Rodzic czeka (*wait(2)*) na dziecko zombie, więc jeśli tego nie zrobi (z powodu jakiegoś buga najpewniej), to musimy zabić rodzica.  
Rodzicem dziecka stanie sie wtedy ppid 1, czyli init, który po nim posrząta.

### Czemu proces nie może sam siebie pogrzebać?  
Ponieważ już nie żyje, nie wykonuje żadnego kodu, jego nieposrzątane struktury po prostu sobie wiszą.

### Zauważ, że proces może, przy pomocy waitpid(2), czekać na zmianę stanu wyłącznie swoich dzieci. Co złego mogłoby się stać, gdyby znieść to ograniczenie? Rozważ scenariusze (a) dziecko może czekać na zmianę stanu swojego rodzica (b) wiele procesów oczekuje na zmianę stanu jednego procesu.
a) Jeśli dziecko mogłoby czekać na zmianę stanu rodzica, a rodzic czekałby na zmianę stanu dziecka, to mielibyśmy deadlock.
b) Przede wszystkim byłby to problem bezpieczeństwa, bo jeden proces mógłby wpływać na drugi.  
Wyobraźmy sobie, że wykonanie programu rodzica zależy od tego, jaki status zwróci dziecko.  
Jeśli jakiś inny proces zajmie się pogrzebaniem dziecka, zamiast jego ojca, to psuje to wykonanie programu rodzica.  
Rodzic najpewniej czekałby wtedy wiecznie.

## zadanie 3
```text
Zadanie 3. Do czego służy system plików proc(5) w systemie Linux?  
Dla wybranego przez siebie procesu o identyfikatorze pid wydrukuj zawartość katalogu «/proc/pid».  
Wyświetl plik zawierający argumenty programu oraz zmienne środowiskowe.  
Podaj znaczenie następujących pól pliku «status»: Uid, Gid, Groups, VmPeak, VmSize, VmRSS, Threads, voluntary_ctxt_switches, nonvoluntary_ctxt_switches.
UWAGA! Prowadzący ćwiczenia nie zadowoli się cytowaniem podręcznika systemowego – trzeba wykazać się dociekliwością
```

### Do czego służy system plików proc(5) w systemie Linux? 
Patrząc na długość dokumentu [the /proc filesystem](https://docs.kernel.org/filesystems/proc.html) możemy zauważyć, że służy on do wielu rzeczy.

- /proc/interrupts wskazuje jakie przerwania są aktualnie w użyciu
- /proc/cpuinfo daje szczegółowe informacje o naszym cpu
- /proc/meminfo daje informacje o dystrybucji i zuzyciu pamięci
- /proc/net/dev daje informacje o odebranych i przeslanych pakietach

Jednak najważniejszymi katalogami dla nas, są te numeryczne.  
Numery te odpowiadają PID procesu, a w środku możemy znaleźć szczegółowe informacje o tym procesie.


### Dla wybranego przez siebie procesu o identyfikatorze pid wydrukuj zawartość katalogu «/proc/pid».  
```sh
ls -la /proc/1/
```

### Wyświetl plik zawierający argumenty programu oraz zmienne środowiskowe.  
```sh
cat /proc/$$/cmdline
```

```sh
cat /proc/$$/environ
```

### Podaj znaczenie następujących pól pliku «status»: Uid, Gid, Groups, VmPeak, VmSize, VmRSS, Threads, voluntary\_ctxt\_switches, nonvoluntary\_ctxt\_switches.
```text
UWAGA! Prowadzący ćwiczenia nie zadowoli się cytowaniem podręcznika systemowego – trzeba wykazać się dociekliwością
```

No dobrze, jak nie man page, to [docsy kernela](https://docs.kernel.org/filesystems/proc.html).

| Field       | Content     |
| ----------- | ----------- |
| Uid         | Real, effective, saved set, and file system UIDs       |
| Gid         | Real, effective, saved set, and file system GIDs        |
| Groups      | supplementary group list |
| VmPeak      | peak virtual memory size |
| VmSize      | total program size |
| VmRSS       | size of memory portions. It contains the three following parts (VmRSS = RssAnon + RssFile + RssShmem) |
| Threads     | number of threads |
| voluntary\_ctxt\_switches | number of voluntary context switches |
| nonvoluntary\_ctxt\_switches |  number of non voluntary context switches |

A tak tłumacząc z kernelowego na chłopski:

- Uid - realny i efektywny id uzytkownika to zwykle ta sama wartość. Różnica jest wtedy gdy, wykonujemy program który ma
  ustawiony suid bit, wtedy real user id to nasz id, a effective id to id wlasciciela procesu. Gdy chcemy wykonać jakąś
  nieprzywilejowaną operację, to ustawiamy effective id na real id. Ponieważ straciliśmy nasze uprzywilejowane id w euid
  i ruid, to przydaje sie to Saved user ID, dzięki któremu znów wracamy do trybu uprzywilejowanego. [^uid]
- Gid - podobnie jak z Uid. Czasami musimy należeć do jakiejś grupy, by wykonać jakieś operacje 
- Groups - grupy do jakich należy proces, czy raczej wlasciciel procesu. To może być np. vboxusers, audio, sudo. [Groupy
  są dziedziczone przez dzieci](https://unix.stackexchange.com/questions/469897/where-are-supplemental-groups-coming-from-in-proc-pid-status-are-they-always-i)
- VmPeak - maksymalne zużycie pamięci jakie miało miejsce od startu procesu. Dla przykładu u mnie ff zjadł raz aż `VmPeak: 26404192 kB` pamięci wirtualnej
- VmSize - suma całej zmapowanej pamięci. Nie jest to pamięć fizyczna, tylko wirtualna.
- VmRSS - suma całej pamięci fizycznej używanej przez proces. Nie zawiera pamięci swap, on jest w VmSwap
- Threads - ilość wątków jakie utworzył proces
- voluntary\_ctxt\_switches - ilość context switches, które były dobrowolne, na przykład gdy proces czekał na operacje io, więc nastąpiło przełączenie kontekstu na inny proces
- nonvoluntary\_ctxt\_switches - ilość context switches, które nie były dobrowolne, czyli zostały wymuszone przez scheduler (zwykle, bo skończył się kwant czasu)

## Zadanie 4
```text
Zadanie 4. Znajdź pid procesu X-serwera, a następnie używając polecenia «pmap» wyświetl zawartość jego przestrzeni adresowej.  
Zidentyfikuj w niej poszczególne zasoby pamięciowe – tj. stos, stertę, segmenty programu, pamięć anonimową, pliki odwzorowane w pamięć.  
Należy wyjaśnić znaczenie kolumn wydruku!
```

```sh
sudo pmap $(pgrep Xorg)
```
(właścicielem procesu jest root, więc trzeba użyć sudo)

### stos
```text
00007ffe38e11000    132K rw---   [ stack ]
```
stos jest klasycznie gdzieś daleko i jest do czytania i pisania, ale oczywiście nie do wykonania[^nx]

### sterta
Sterta często jest oznaczona w mapowaniu przez  [heap]`, w tym przypadku jednak nie takiego wpisu.  
Sterta to w zasadzie po prostu anonimowe[^anonymous] mapowanie, które ma flagi rw.

W moim przypadku jest to najpewniej

```sh
000055a7434f0000    248K rw---   [ anon ]
000055a767dc1000  22576K rw---   [ anon ]
```
### segmenty
Segmenty możemy z wywnioskować po nadanych im uprawnieniach
```sh
000055a74329a000    132K r---- Xorg
000055a7432bb000   1764K r-x-- Xorg
000055a743474000    456K r---- Xorg
000055a7434e6000     16K r---- Xorg
000055a7434ea000     24K rw--- Xorg
```

r-x wskazują na .text segment
r---- kod tylko do odczytu czyli pewnie .rodata
rw--- to pewnie .data albo .bss

### pamięć anonimowa, pliki odwzorowane w pamięci
Możemy użyć lsof, żeby sprawdzić jakie pliki są podmapowane przez proces
```sh
sudo lsof -p $(pgrep Xorg)
```
```sh
--- snip ---
Xorg    810 root mem       REG              254,2    391280    3954085 /usr/lib/libgobject-2.0.so.0.8200.1
Xorg    810 root mem       REG              254,2     47064    3979249 /usr/lib/libgudev-1.0.so.0.3.0
Xorg    810 root mem       REG              254,2     79952    3962191 /usr/lib/libevdev.so.2.3.0
Xorg    810 root mem       REG              254,2    351000    3962425 /usr/lib/libinput.so.10.13.0
Xorg    810 root mem       REG              254,2     84128    4086692 /usr/lib/xorg/modules/input/libinput_drv.so
Xorg    810 root mem       REG              254,2     84024    4086290 /usr/lib/dri/libdril_dri.so
Xorg    810 root mem       REG              254,2    331288    3962552 /usr/lib/libEGL_mesa.so.0.0.0
Xorg    810 root mem       REG              254,2   1310728   13107319 /root/.cache/mesa_shader_cache_db/index
Xorg    810 root mem       REG              254,2   1099376    3977593 /usr/lib/libepoxy.so.0.0.0
Xorg    810 root mem       REG              254,2  30737240    3949557 /usr/lib/libicudata.so.75.1
Xorg    810 root mem       REG              254,2     55320    3962414 /usr/lib/libwacom.so.9.0.0
Xorg    810 root mem       REG              254,2     56040    3962481 /usr/lib/libwayland-client.so.0.23.1
Xorg    810 root mem       REG              254,2   2062384    3949577 /usr/lib/libicuuc.so.75.1
Xorg    810 root mem       REG              254,2  22040176    3936357 /usr/lib/libstdc++.so.6.0.33
--- snip ---
```

Z grubsza są to biblioteki.

## zadanie 5

```
Zadanie 5. Używając programu «lsof» wyświetl zasoby plikopodobne podpięte do procesu przeglądarki
«firefox».  
Wyjaśnij znaczenie poszczególnych kolumn wykazu, po czym zidentyfikuj pliki zwykłe, katalogi, urządzenia, gniazda (sieciowe lub domeny uniksowej) i potoki.  
Przekieruj wyjście z programu «lsof», przed i po otwarciu wybranej strony, odpowiednio do plików «before» i «after».  
Czy poleceniem «diff -u before after» jesteś w stanie zidentyfikować nowo utworzone połączenia sieciowe?
```

### Wyjaśnij znaczenie poszczególnych kolumn wykazu

- COMMAND - pierwsze 9 znaków nazwy programu procesu  

- PID - numer id procesu  

- USER - numer/nazwa usera, do którego proces należy

- FD:

```
FD         is the File Descriptor number of the file or:

           cwd  current working directory;
           Lnn  library references (AIX);
           ctty character tty;
           DEL  deleted file;
           err  FD information error (see NAME column);
           fp.  Fileport (Darwin);
           jld  jail directory (FreeBSD);
           ltx  shared library text (code and data);
           Mxx  hex memory-mapped type number xx.
           m86  DOS Merge mapped file;
           mem  memory-mapped file;
           mmap memory-mapped device;
           NOFD for a Linux /proc/<PID>/fd directory that can't be opened --
                the directory path appears in the NAME column, followed by an error
                message;
           pd   parent directory;
           Rnn  unknown pregion number (HP-UX);
           rtd  root directory;
           twd  per task current working directory;
           txt  program text (code and data);
           v86  VP/ix mapped file;
```

po FD jest litera, która oznacza tryb, w którym plik jest otwarty:

- r - tylko do odczytu
- w - tylko do pisania
- u - do odczytu i pisania
- <spacja> jesli tryb jest nieznany, a dalej nie ma znaku blokady
- "-" jestli tryb jest nieznany, a dalej jest znak blokady

Na pliki mogą być nałożone blokady, na przykład exclusive lock albo shared lock.  

- Exclusive lock nie pozwala żadnemu innemu procesowi pisać ani czytać do pliku, dopóki doputy lock nie zostanie zniesiony.  
- Shared lock pozwala innym przeczytac plik i dopiero gdy wszyscy skończą czytać, to proces kontynuuje pisanie do pliku.  

Lockiem może być objęty cały plik, ale również i jego część.  
Poniżej informacja z man page'a.

```text
N for a Solaris NFS lock of unknown type;
r for read lock on part of the file;
R for a read lock on the entire file;
w for a write lock on part of the file;
W for a write lock on the entire file;
u for a read and write lock of any length;
U for a lock of unknown type;
x for an SCO OpenServer Xenix lock on part of the file;
X for an SCO OpenServer Xenix lock on the entire file;
space if there is no lock.
```

- TYPE - rodzaj węzła powiązanego z plikiem. Może to być inode, w kontekście systemu plików, ale też sockety, linki symboliczne, katalogi, wirtualne systemy plików i wiele innych.  

Lista typów jest dość długa, warto sprawdzić lsof(8).

- DEVICE - numer urządzenia. To może być urządzenie blokowe, znakowe, normalne  

```text
Te major number identifies
the device driver and sometimes encodes which peripheral board to
communicate with; the minor number identifies the specific subdevice. Recall
from Figure 4.13 that a disk drive often contains several file systems. Each file
system on the same disk drive would usually have the same major number, but
a different minor number.
```
[^devicenum]

Urządzenia blokowe i znakowe to po prostu pliki, które są abstrakcją dla urządzeń.  
Dane pisane do takiego pliku, lecą potem do odpowiedniego sterownika.  
Urządzenie blokowe jest seekable, działa jak normalny plik, a urządzenie znakowe na to nie pozwala.[^specialfiles]

- SIZE/OFF - rozmiar lub offset pliku.  

Jeśli typ to liczba decymalna to jest to rozmiar, jeśli rozpoczyna się od "0t" albo "0x", to jest to offset.

- NODE - numer węzła, według danego typu.
- NAME - ścieżka do pliku.  

NAME może mieć bardzo wiele typów, więc też warto sprawdzić man page.

### ... po czym zidentyfikuj pliki zwykłe, katalogi, urządzenia, gniazda (sieciowe lub domeny uniksowej) i potoki.

Pliki zwykłe mają typ REG, przykładowe rekordy (jest ich bardzo dużo):

```sh
firefox 1500 crusom DEL       REG                0,1              28668 /memfd:mozilla-ipc
firefox 1500 crusom mem       REG              254,2  43577564  4215382 /usr/lib/firefox/browser/omni.ja
firefox 1500 crusom DEL       REG                0,1               1155 /memfd:mozilla-ipc
firefox 1500 crusom mem       REG              254,2    180152  4077543 /usr/share/mime/mime.cache
firefox 1500 crusom mem       REG              254,2  34409198  4229829 /usr/lib/firefox/omni.ja
firefox 1500 crusom mem       REG              254,2     14312  3964011 /usr/lib/libxcb-present.so.0.0.0
firefox 1500 crusom mem       REG              254,2     61664  4587976 /var/cache/fontconfig/491f45a0a771fef1c10b9b647a97fb82-le64.cache-9
```

Katalogi mają typ DIR, rekordy:
```sh
firefox 1500 crusom cwd       DIR              254,2     53248 10747906 /home/crusom
firefox 1500 crusom rtd       DIR              254,2      4096        2 /
```

- cwd oznacza oczywiście obecny katalog, w którym proces się znajduje.
- rtd to root directory, standardowo jest to /, może być pewnie jednak inaczej, jeśli używamy chroota

Urządzenia (blokowe lub znakowe) mają typ "BLK" lub "CHR" 

Mój proces firefoxa nie korzysta z urządzeń blokowych, korzysta natomiast ze znakowych:
```sh
firefox 1500 crusom   1w      CHR                1,3       0t0        4 /dev/null
--- snip ---
firefox 1500 crusom  15u      CHR            226,128       0t0      406 /dev/dri/renderD128
--- snip ---
firefox 1500 crusom  36u      CHR              226,1       0t0      407 /dev/dri/card1
```

- /dev/null to oczywiście urządzenie blokowe, do którego można wrzucać co chcemy i trafi do w nicość, i z którego można czytać zera.
- /dev/dri `Each GPU detected by DRM is referred to as a DRM device, and a device file /dev/dri/cardX (where X is a sequential number) is created to interface with it.`[^DRM]

Gniazda w komunikacji sieciowej najczęściej są to typy IPv4 i IPv6:

```sh
firefox 1500 crusom 208u     unix 0x00000000587bc5df       0t0    15376 type=STREAM (CONNECTED)
firefox 1500 crusom 209u     IPv4             508810       0t0      TCP crusomcarbon:53358->82.221.107.34.bc.googleusercontent.com:http (ESTABLISHED)
firefox 1500 crusom 210u     unix 0x0000000007b5c15c       0t0     9815 type=STREAM (CONNECTED)
firefox 1500 crusom 211u     unix 0x00000000aa128069       0t0    52172 type=STREAM (CONNECTED)
firefox 1500 crusom 212u     unix 0x000000003af7214a       0t0    64791 type=SEQPACKET (CONNECTED)
firefox 1500 crusom 213u     IPv6             514368       0t0      TCP crusomcarbon:55984->[2001:67c:4e8:f004::9]:https (ESTABLISHED)
```
unixowe:

```sh
firefox 1500 crusom 219u     unix 0x000000005bfd8788       0t0   484730 type=STREAM (CONNECTED)
firefox 1500 crusom 220u     unix 0x00000000704b1572       0t0    13269 type=SEQPACKET (CONNECTED)
```

Potoki mają typ "STR", ale nie znalazłem żadnych otwartych plików z typem STR.


### Przekieruj wyjście z programu «lsof», przed i po otwarciu wybranej strony, odpowiednio do plików «before» i «after». Czy poleceniem «diff -u before after» jesteś w stanie zidentyfikować nowo utworzone połączenia sieciowe?  


```
[crusom@crusomcarbon 1]$ lsof -c firefox > before
[crusom@crusomcarbon 1]$ lsof -c firefox > after
```

```sh
diff -u before after | grep -Ei '(ipv4|ipv6)' | grep +
```

```sh
+firefox 1500 crusom 303u     IPv4             591749       0t0      TCP crusomcarbon:40576->linux.org.pl:https (ESTABLISHED)
+firefox 1500 crusom 379u     IPv6             603457       0t0      TCP crusomcarbon:53600->[2001:67c:4e8:f004::9]:https (ESTABLISHED)
+firefox 1500 crusom 387u     IPv6             603459       0t0      TCP crusomcarbon:53608->[2001:67c:4e8:f004::9]:https (ESTABLISHED)
```

```sh
[crusom@crusomcarbon ~]$ getent ahostsv6 linux.org.pl
2001:41d0:601:1100::ce8 STREAM linux.org.pl
2001:41d0:601:1100::ce8 DGRAM
2001:41d0:601:1100::ce8 RAW
```

Rzeczywiście, udało mi się znaleźć nowo utworzone połączenia.

## zadanie 6
```text
Zadanie 6. Wbudowanym poleceniem powłoki «time» zmierz czas wykonania długo działającego procesu, np. polecenia «find /usr».  
Czemu suma czasów user i sys (a) nie jest równa real (b) może być większa od real?  
Poleceniem «ulimit» nałóż ograniczenie na czas wykonania procesów potomnych powłoki tak, by limit się wyczerpał. Uruchom ponownie wybrany program – który sygnał wysłano do procesu?
```

### Wbudowanym poleceniem powłoki «time» zmierz czas wykonania długo działającego procesu, np. polecenia «find /usr». Czemu suma czasów user i sys (a) nie jest równa real (b) może być większa od real?

a) 

```sh
[crusom@crusomcarbon 1]$ time find /usr > /dev/null
real    0m3.388s
user    0m0.531s
sys     0m1.142s
```

man 1 time
```text
These statistics consist of (i) the elapsed real time between
       invocation and termination, (ii) the user CPU time (the sum of
       the tms_utime and tms_cutime values in a struct tms as returned
       by times(2)), and (iii) the system CPU time (the sum of the
       tms_stime and tms_cstime values in a struct tms as returned by
       times(2)).
```

- real to rzeczywisty czas, który minął między wywołaniem a zakończeniem programu,
- user to suma tms_utime i tms_cutime
- sys to suma tms_stime i tms_cstime

Zajrzyjmy teraz do man 1 times:

- tms_utime czas CPU spędzony na wykonywaniu instrukcji procesu wywołującego
- tms_cutime czas CPU spędzony na wykonywaniu instrukcji przez dzieci procesu wywołującego
- tms_stime czas CPU spędzony na wykonywaniu wewnątrz jądra podczas wykonując zadania w imieniu procesu wywołującego.
- tms_stime czas CPU spędzony na wykonywaniu wewnątrz jądra podczas wykonując zadania w imieniu dzieci procesu wywołującego.

Więc:

- user - czas spędzony przez proces i dzieci na wykonywaniu instrukcji,
- sys - czas spędzony w kernelu, na wykonywaniu zadań dla procesu i jego dzieci,

Ponieważ nasz proces nie zajmował całego czasu procesora, a jedynie jego część, to rzeczywisty czas wykonania programu może być dłuższy, bo zawiera w sobie też inne procesy.

b)

Ponieważ współczesne CPU mają w sobie kilka procesorów, to jeśli wystąpiła by sytuacja, że każdy z procesorów wykonuje nasz proces i jego dzieci, a potem kończy, to nasz czas user+sys będzie kilkukrotnie wyższy, niż real.  
Konkretnie tylukrotnie wyższy ile mamy procesorów.

### Poleceniem «ulimit» nałóż ograniczenie na czas wykonania procesów potomnych powłoki tak, by limit się wyczerpał. Uruchom ponownie wybrany program – który sygnał wysłano do procesu?

Nakładamy limit na 1 sekunde korzystając z
```sh
ulimit -t 1
```
```sh
[crusom@crusomcarbon ~]$ time find / &> /dev/null
Killed

real    0m1.004s
user    0m0.382s
sys     0m0.610s
[crusom@crusomcarbon ~]$ echo $?
137
```

Widzimy napis Killed, a exit code programu to 137.  
Sprawdzamy [liste exit code'ów](https://tldp.org/LDP/abs/html/exitcodes.html) i widzimy, że kod 128+n oznacza `Fatal error signal "n"`.  
Patrzymy na [liste sygnałów](https://faculty.cs.niu.edu/~hutchins/csci480/signals.htm) i rzeczywiście 9 oznacza `SIGKILL`


## zadanie 7
```text
Zadanie 7. Napisz program, który będzie prezentował, że pliki procesu są kopiowane przez referencję w trakcie wywołania fork(2).  
W procesie głównym otwórz plik do odczytu open(2). Czy zamknięcie pliku close(2) w procesie głównym zamyka plik także w dziecku?  
Czy odczyt z pliku read(2) zmienia pozycję kursora lseek(2) w drugim procesie? Wyjaśnij zachowanie swojego programu!  
Przed każdym komunikatem diagnostycznym wypisz pid procesu.  
W drugiej części zadania należy wydrukować bieżącą pozycję kursora pliku przed operacją odczytu z pliku.  
Należy wykorzystać dostarczone funkcje opakowujące uniksowe wywołania systemowe z biblioteki libcsapp.
```

### Napisz program, który będzie prezentował, że pliki procesu są kopiowane przez referencję w trakcie wywołania fork(2).  W procesie głównym otwórz plik do odczytu open(2). Czy zamknięcie pliku close(2) w procesie głównym zamyka plik także w dziecku?  

kod źródłowy 7.c
```c
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "include/csapp.h"

int main(void) {
  int fd = Open("a.txt",0,O_RDONLY);
  char buf[0x100] = {0};


  if (Signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  pid_t pid = Fork();
  switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      printf("[%jd] Sleeping...\n", getpid());
      sleep(2);
      ssize_t r;
      if((r = read(fd,buf,0x10)) == -1)
        printf("[%jd] Read error! %s\n", (intmax_t) getpid(), strerror(errno));
      else {
        buf[r] = 0;
        printf("[%jd] successfuly read()! %s\n",(intmax_t) getpid(), buf);
      }
      printf("[%jd] Exiting.\n", getpid());
      break;
    default:
      Close(fd);
      printf("[%jd] Closed fd\n", getpid());
      printf("[%jd] Exiting.\n", getpid());
      break;
  }
}
```

Otwieramy plik, robimy forka, dziecko chwile czeka, zeby byc pewnym ze rodzic zamknął file descriptor, po czym czyta plik.  

Wykonanie programu:
```sh
[crusom@crusomcarbon 1]$ ./a.out
[183062] Closed fd
[183062] Exiting.
[183063] Sleeping...
[crusom@crusomcarbon 1]$ [183063] successfuly read()! afsfdsafdsafda

[183063] Exiting.
```

Rzeczywiście udało się przeczytać plik.  

Sprawdźmy jakich syscalli używa fork(), użyjmy w tym celu narzędzia strace  
Po otwarciu pliku pojawia się taki syscall

```sh
clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7a3cf526fa10) = 183669
```

Sprawdźmy w clone(2) czy możemy znaleźć jakąś flagę, która zmieni zachowanie programu z file descryptorami.


```text
CLONE_FILES (since Linux 2.0)
      If CLONE_FILES is set, the calling process and the child process share the same file descriptor table.  Any file descriptor created by the  calling  process  or  by  the  child
      process  is  also  valid  in the other process.  Similarly, if one of the processes closes a file descriptor, or changes its associated flags (using the fcntl(2) F_SETFD opera‐
      tion), the other process is also affected.  If a process sharing a file descriptor table calls execve(2), its file descriptor table is duplicated (unshared).

      If CLONE_FILES is not set, the child process inherits a copy of all file descriptors opened in the calling process at the time of the clone call.   Subsequent  operations  that
      open  or  close file descriptors, or change file descriptor flags, performed by either the calling process or the child process do not affect the other process.  Note, however,
      that the duplicated file descriptors in the child refer to the same open file descriptions as the corresponding file descriptors in the calling process,  and  thus  share  file
      offsets and file status flags (see open(2)).
```

W pliku 7_ptrace.c znajduje się kod, który robi to samo co wcześniej, z tym że podłącza ptrace i dodaje flage CLONE_FILES do syscalla clone, gdy jest wołany.

```c
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

#include "include/csapp.h"

#define CLONE_FILES 0x00000400

void monitor_clone(pid_t child) {
  int status;
  struct user_regs_struct regs;

  while (1) {
    wait(&status);
    if (WIFEXITED(status)) break;
    ptrace(PTRACE_GETREGS, child, 0, &regs);

    if (regs.orig_rax == SYS_clone) {
      if (!(regs.rdi & CLONE_FILES)) {
        printf("Dodajemy CLONE_FILES do flag\n");

        regs.rdi |= CLONE_FILES;
        ptrace(PTRACE_SETREGS, child, 0, &regs);
      }
    }
    // continue the process
    ptrace(PTRACE_SYSCALL, child, 0, 0);
  }
}

int main() {
  pid_t child;
  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  child = Fork();
  if (child == 0) {
    long ret = ptrace(PTRACE_TRACEME, 0, 0, 0);
    if (ret == -1) {
      fprintf(stderr, "ptrace(PTRACE_TRACEM) error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    kill(getpid(), SIGSTOP); // Zatrzymujemy proces, czekając na rodzica

    int fd = Open("a.txt",0,O_RDONLY);
    char buf[0x100] = {0};

    pid_t pid = Fork();
    if (pid == 0) {
      printf("[%jd] Sleeping...\n", getpid());
      sleep(1);
      ssize_t r;
      printf("[%jd] Trying to read()\n", getpid());
      if((r = read(fd,buf,0x10)) == -1) {
        printf("[%jd] Error in read()! %s\n",(intmax_t) getpid(), strerror(errno));
      } else {
        buf[r] = 0;
        printf("[%jd] Successful read()! %s\n",(intmax_t) getpid(), buf);
      }
      printf("[%jd] Exiting.\n", getpid());
      exit(EXIT_SUCCESS);
    } else {
      Close(fd);
      printf("[%jd] closed\n", getpid());
      printf("[%jd] Sleeping...\n", getpid());
      sleep(2);
      printf("[%jd] Exiting.\n", getpid());
      exit(EXIT_SUCCESS);
    }

  } else {
    Wait(NULL);
    ptrace(PTRACE_SYSCALL, child, 0, 0);
    monitor_clone(child);
  }
  return 0;
}
```

```sh
[crusom@crusomcarbon 1]$ ./a.out
Dodajemy CLONE_FILES do flag
[192448] Sleeping...
[192447] closed
[192447] Sleeping...
[192448] Trying to read()
[192448] Error in read()! Bad file descriptor
[192448] Exiting.
[192447] Exiting.
```

Jak widać z flagą CLONE_FILES, dziecko i rodzic dzielą te samą tablice deskryptorów.

### Czy odczyt z pliku read(2) zmienia pozycję kursora lseek(2) w drugim procesie? Wyjaśnij zachowanie swojego programu!  

```c
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "include/csapp.h"

int main(void) {
  int fd = 0pen("a.txt", 0, O_RDONLY);
  char buf[0x100] = {0};


  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  pid_t pid = Fork();
  switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      off_t position = lseek(fd, 0, SEEK_CUR);
      printf("[%jd] offset position %jd\n", getpid(), position);
      printf("[%jd] Sleeping\n", getpid());
      sleep(2);
      position = lseek(fd, 0, SEEK_CUR);
      printf("[%jd] offset position %jd\n", getpid(), position);
      printf("[%jd] Exiting.\n", getpid());
      break;
    default:
      printf("[%jd] Sleeping\n", getpid());
      sleep(1);
      printf("[%jd] Read file\n", getpid());
      Read(fd,buf,0x10);
      printf("[%jd] Sleeping\n", getpid());
      sleep(2);
      printf("[%jd] Exiting.\n", getpid());
      break;
  }
}
```

Pozycja kursora zmieniła się też w dziecku!  
Dlaczego? Wyjaśnia to fork(2)  

```text
The child inherits copies of the parent's set of open file descriptors.  
Each file descriptor in the child refers to the same open file description (see open(2)) as the corresponding file descriptor in the parent.  
This means that the two file descriptors share open file status flags, file offset, and signal-driven I/O attributes  (see  the  description  of F_SETOWN and F_SETSIG in fcntl(2)).
```

File descriptor to taki troche pointer do file description.  
Close() sprawia, że ten "pointer" nie jest już dłużej możliwy do użytku, ale file description dalej żyje.  


## Zadanie 8
```text
Rozwiąż problem n hetmanów6 z użyciem fork(2) i waitpid(2). Gdy w i-tym elemencie tablicy «board» przechowywana jest wartość j znaczy to, że pozycja i-tego hetmana na szachownicy to (i, j).  
Mając niekonfliktujące ustawienie pierwszych k − 1 hetmanów po kolei startuj n podprocesów z proponowanym ustawieniem k-tego hetmana.  
Podproces, który wykryje konfliktujące ustawienie hetmanów, ma zakończyć swe działanie. W przeciwnym wypadku zachowuje się jak rodzic dla k + 1 hetmana.  
Podproces, który uzyska prawidłowe ustawienie n hetmanów, ma wydrukować je na standardowe wyjście.  
Procedura «ndselect» wraca wielokrotnie z kolejnymi liczbami z zakresu 0...n − 1.
```

```c
#include <unistd.h>
#include "include/csapp.h"

static int fd;
#define myexit() do { close(fd); exit(EXIT_SUCCESS); } while(0);

static int ndselect(int n) {
  /* TODO: A loop is missing here that spawns processes and waits for them! */
  // ojciec bierze kolejne dzieci
  pid_t children[n];
  for (int i = 0; i < n; i++) {
    pid_t pid = Fork();
    children[i] = pid;
    if (pid == 0)
      return i;
  }
  // ojciec czeka na dzieci, zeby po nich posprzatac
  for (int i = 0; i < n; i++)
    waitpid(children[i], NULL, 0);

  myexit();
}

static int conflict(int x1, int y1, int x2, int y2) {
  return x1 == x2
    || y1 == y2
    || x1 + y1 == x2 + y2
    || x1 - y1 == x2 - y2;
}

static void print_line_sep(int size) {
  for (int i = 0; i < size; ++i)
    dprintf(fd,"+---");
  dprintf(fd,"+\n");
}

static void print_board(int size, int board[size]) {
  for (int i = 0; i < size; ++i) {
    print_line_sep(size);
    for (int j = 0; j < size; ++j)
      dprintf(fd,"|%s", board[i] == j ? " Q " : "   ");
    dprintf(fd,"|\n");
  }
  print_line_sep(size);
  dprintf(fd,"\n");
}


int main(int argc, char **argv) {
  if (argc != 2)
    app_error("Usage: %s [SIZE]", argv[0]);

  int size = atoi(argv[1]);

  if (size < 4 || size > 9)
    app_error("Give board size in range from 4 to 9!");

  fd = Open("het.txt", O_RDWR | O_CREAT, 0644);

  int board[size];
  memset(board,-1,sizeof(board));
  // tworzymy wszystkie mozliwe kombinacje planszy
  for (int i = 0; i < size; i++) {
    int k = ndselect(size);
    board[i] = k;
  }

  // sprawdzamy czy działa
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i == j) continue;
      if (conflict(i, board[i], j, board[j]))
        myexit();
    }
  }

  while(lockf(fd, F_TLOCK, 0)) {puts("locked...\n");};
  print_board(size, board);
  lockf(fd,F_ULOCK, 0);

  close(fd);
  return 0;
}
```
Tworzymy wszystkie możliwe wariacje planszy i sprawdzamy czy nie ma konfliktów.  
Używamy lockf, aby dać procesowi eksluzywny dostęp do pliku, żeby mógł w całości wyprintować plansze.  
Gdy zdejmie blokade, to kolejny proces będzie mógł wyprintować.  
W pliku het.txt znajdują się nasze rozwiązania.  

# Footnotes

[^uid]: <https://stackoverflow.com/questions/32455684/difference-between-real-user-id-effective-user-id-and-saved-user-id>

[^nx]: <https://en.wikipedia.org/wiki/NX_bit>

[^anonymous]: anonimowy znaczy tyle, że mapowanie nie jest powiązane z żadnym plikiem

[^devicenum]: APUE 4.24

[^specialfiles]: <https://unix.stackexchange.com/questions/60034/what-are-character-special-and-block-special-files-in-a-unix-system>
[^DRM]: <https://en.wikipedia.org/wiki/Direct_Rendering_Manager>
