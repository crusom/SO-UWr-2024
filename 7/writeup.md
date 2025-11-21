
## Zadanie 1
```text
Zadanie 1. Na podstawie [3, 49.1] wyjaśnij słuchaczom różnicę między odwzorowaniami plików w pamięć (ang. memory-mapped files) i odwzorowaniami pamięci anonimowej (ang. anonymous mappings).  
Jaką zawartością wypełniana jest pamięć wirtualna należąca do tychże odwzorowań?  
Czym różni się odwzorowanie prywatne od dzielonego?  
Czy pamięć obiektów odwzorowanych prywatnie może być współdzielona? Czemu można tworzyć odwzorowania plików urządzeń blokowych w pamięć, a znakowych nie?
Wskazówka: Jądro może udostępniać pamięć karty graficznej albo partycję dysku jako urządzenie blokowe
```

### Zadanie 1. Na podstawie [3, 49.1] wyjaśnij słuchaczom różnicę między odwzorowaniami plików w pamięć (ang. memory-mapped files) i odwzorowaniami pamięci anonimowej (ang. anonymous mappings). Jaką zawartością wypełniana jest pamięć wirtualna należąca do tychże odwzorowań? 
Odwzorowanie pliku w pamiec - mapuje plik na dysku do pamieci wirtualnej. Operacje na pamięci przekładają się na odczyt z dysku  
Odwzorowanie pamięci anonimowej - mapowanie niepodparte plikiem, daje nam po prostu nowe strony w pamięci wirtualnej, zainicjalizowane zerami

### Czym różni się odwzorowanie prywatne od dzielonego?  
Zmiany w odwzorowaniu dzielonym przekładają się na zmiany w pliku na dysku.  
W odwzorowaniu prywatnym nie.

### Czy pamięć obiektów odwzorowanych prywatnie może być współdzielona? 
tak, na przyklad biblioteki wspoldzielone (so) załadowane do pamięci fizycznej mogą być podmapowane przez wiele procesów w pgd (page global directory).

### Czemu można tworzyć odwzorowania plików urządzeń blokowych w pamięć, a znakowych nie?
Ponieważ urządzenia znakowe są sekwencyjne, nie znamy ich rozmiaru.

## Zadanie 2
```text
Zadanie 2. Na podstawie opisu do [ 3, tabeli 49 –1] podaj scenariusze użycia prywatnych i dzielonych odwzorowań plików w pamięć albo pamięci anonimowej.  
Pokaż jak je utworzyć z użyciem wywołania mmap(2). Co się dzieje z odwzorowaniami po wywołaniu fork(2)?  
Czy wywołanie execve(2) tworzy odwzorowania prywatne czy dzielone? 
W jaki sposób jądro systemu automatycznie zwiększa rozmiar stosu do ustalonego limitu? Kiedy jądro wyśle sygnał SIGBUS do procesu posiadającego odwzorowanie pliku w pamięć [ 3, §49.4.3]
```

### Na podstawie opisu do [ 3, tabeli 49 –1] podaj scenariusze użycia prywatnych i dzielonych odwzorowań plików w pamięć albo pamięci anonimowej.  Pokaż jak je utworzyć z użyciem wywołania mmap(2).  
```c
void *mmap(void *addr, size_t len, int prot, int flags,
   int fildes, off_t off);
```

prywatne-pamięć przydaje się gdy chcemy zainicjalizowac pamiec zawartoscia pliku, np. gdy loader ładuje elfa do pamięci  
```c
struct stat stat;
int fd = Open(argv[1], O_RDONLY, 0);
fstat(fd, &stat);
mmap(NULL, stat.st_size, SOME_PROTECTION, MAP_PRIVATE,fd,0)
```
prywatne-anonim przydaje sie do alokacji, na przyklad dla malloca
```c
mmap(NULL, SOME_SIZE, SOME_PROTECTION, MAP_PRIVATE | MAP_ANON,-1,0)
```
dzielone-pamiec gdy chcemy czytac i/lub modyfikowac plik, gdzie potrzebujemy duzo skoków. łatwiej jest nam uzywac mmapa niz read/write. Również komunikacja międzyprocesowa (IPC)  
```c
struct stat stat;
int fd = Open(argv[1], O_RDONLY, 0);
fstat(fd, &stat);
mmap(NULL, stat.st_size, SOME_PROTECTION, MAP_SHARED,fd,0)
```
dzielone-anonim do komunikacji miedzyprocesowej (IPC)
```c
mmap(NULL, SOME_SIZE, SOME_PROTECTION, MAP_ANON | MAP_SHARED,fd,0)
```

### Co się dzieje z odwzorowaniami po wywołaniu fork(2)?  
Nowy proces dostaje dokładnie to samo mapowanie, a strony są oznaczone przez Copy-on-write.  
Jak podają slajdy:
```text
 Create exact copies of current mm_struct,vm_area_struct, and page tables.
 Flag each page in both processes as read-only
 Flag each vm_area_struct in both processes as private COW
```

### Czy wywołanie execve(2) tworzy odwzorowania prywatne czy dzielone? 
Odwzorowania są prywatne.
![](/assets/slajd11.png)

### W jaki sposób jądro systemu automatycznie zwiększa rozmiar stosu do ustalonego limitu? 
Gdy mapuje stos korzystamy z flagi MAP_GROWSDOWN, która:
```text
MAP_GROWSDOWN
      This flag is used for stacks.  It indicates to the kernel
      virtual memory system that the mapping should extend
      downward in memory.  The return address is one page lower
      than the memory area that is actually created in the
      process's virtual address space.  Touching an address in
      the "guard" page below the mapping will cause the mapping
      to grow by a page.  This growth can be repeated until the
      mapping grows to within a page of the high end of the next
      lower mapping, at which point touching the "guard" page
      will result in a SIGSEGV signal.
```
W skrócie - strona za stosem to GUARD PAGE, gdy jej dotkniemy to kernel przydzieli stosowi więcej pamięci

### Kiedy jądro wyśle sygnał SIGBUS do procesu posiadającego odwzorowanie pliku w pamięć [ 3, §49.4.3]

![](/assets/49_4.png)

Jesli mamy segment pamieci, w którym mamy strony niepodmapowane zadnym plikiem, tzn. zaladowany plik jest krotszy niz wielkosc naszej pamieci, to dostep do tej pamieci nieodpowiadajacej plikowi spowoduje SIGBUS.  
SIGBUS informuje ze zrobilismy dostep do pamieci, ktora nie odpowiada zadnemu plikowi.

## Zadanie 3
```text
Zadanie 3. Przy pomocy polecenia «cat /proc/$(pgrep Xorg)/status | egrep ’Vm|Rss’» wyświetl zużycie pamięci procesu wykonującego kod X-serwera.  
Na podstawie podręcznika proc(5) wyjaśnij znaczenie poszczególnych pól.  
Przypomnij jaka jest różnica między zbiorem roboczym i rezydentnym procesu. 
Napisz krótki skrypt (np. w języku Python lub awk(1)), który wyznaczy sumę «VmSize» i osobno sumę «VmRSS» wszystkich procesów. Czemu ta druga wartość nie pokrywa się z rozmiarem używanej pamięci raportowanym przez polecenie «vmstat -s»?
```

```sh
[crusom@crusomcarbon ~]$ cat /proc/$(pgrep Xorg)/status | egrep 'Vm|Rss'
egrep: warning: egrep is obsolescent; using grep -E
VmPeak:  1020744 kB
VmSize:   810048 kB
VmLck:         0 kB
VmPin:         0 kB
VmHWM:    116668 kB
VmRSS:     82760 kB
RssAnon:           34400 kB
RssFile:           47372 kB
RssShmem:            988 kB
VmData:    83880 kB
VmStk:       132 kB
VmExe:      1764 kB
VmLib:    117424 kB
VmPTE:       496 kB
VmSwap:        0 kB
```


### Na podstawie podręcznika proc(5) wyjaśnij znaczenie poszczególnych pól.  
nie proc tylko proc_pid_status(5)

VmPeak - Peak virtual memory size.  

VmSize - Virtual memory size. Suma VmLib, VmExe, VmData, VmStk

VmLck  - Locked memory size (see mlock(2)).

VmPin  - Pinned memory size (since Linux 3.2).  These are pages that can't be moved because something needs to directly access physical memory.

VmHWM  - Peak resident set size ("high water mark").

VmRSS  - Resident set size.  Note that the value here is the sum of RssAnon, RssFile, and RssShmem.

RssAnon - Size of resident anonymous memory

RssFile - Size of resident file mappings.

RssShmem - Size of resident shared memory (includes System V shared memory, mappings from tmpfs(5), and shared anonymous mappings

VmData VmStk VmExe - Size of data, stack, and text segments.  

VmLib  - Shared library code size.

VmPTE  - Page table entries size (since Linux 2.6.10).

VmSwap - Swapped-out virtual memory size by anonymous private pages; shmem swap usage is not included (since Linux 2.6.34). 


### Przypomnij jaka jest różnica między zbiorem roboczym i rezydentnym procesu. 
Zbiór roboczy podaje ilość stron z których proces korzystał w danym przedziale czasowym  
Zbiór rezydentny to ilość stron procesu, która jest trzymana w RAMie (pamieci fizycznej!). Tak więc nie wlicza się w to swap space ani files system

### Napisz krótki skrypt (np. w języku Python lub awk(1)), który wyznaczy sumę «VmSize» i osobno sumę «VmRSS» wszystkich procesów. Czemu ta druga wartość nie pokrywa się z rozmiarem używanej pamięci raportowanym przez polecenie «vmstat -s»?

```sh
#!/bin/bash

vmsize_sum=0
vmrss_sum=0
for pid in $(ps -e | awk '{print $1}'); do
    f="/proc/$pid/status"
    if [ ! -f $f  ]; then continue; fi
    vmsize=$(grep -i "VmSize" $f | awk '{print $2}')
    vmrss=$(grep -i "VmRSS" $f | awk '{print $2}')
    vmsize_sum=$((vmsize_sum + vmsize))
    vmrss_sum=$((vmrss_sum + vmrss))
done

echo "Suma VmSize: $vmsize_sum kB"
echo "Suma VmRSS: $vmrss_sum kB"
```

```sh
[crusom@crusomcarbon 7]$ ./vm.sh && vmstat -s
Suma VmSize: 265972932 kB
Suma VmRSS: 17681800 kB
     16135548 K total memory
      8633416 K used memory
      8694260 K active memory
      5415992 K inactive memory
       837700 K free memory
       361156 K buffer memory
      7487460 K swap cache
     16777212 K total swap
         3080 K used swap
     16774132 K free swap
      5658104 non-nice user cpu ticks
          115 nice user cpu ticks
      2436633 system cpu ticks
     64236873 idle cpu ticks
       269362 IO-wait cpu ticks
       303900 IRQ cpu ticks
       304143 softirq cpu ticks
            0 stolen cpu ticks
            0 non-nice guest cpu ticks
            0 nice guest cpu ticks
     30958737 K paged in
     35970925 K paged out
            0 pages swapped in
            1 pages swapped out
    293874936 interrupts
    822829712 CPU context switches
   1731870298 boot time
       836455 forks
```

vmstat czyta z /proc/meminfo i daje tylko oszacowania

<https://www.man7.org/linux/man-pages/man5/proc_meminfo.5.html>

i tak na przykład active: 
```text
Active %lu
     Memory that has been used more recently and usually
     not reclaimed unless absolutely necessary.
```

Powód tak dużej rozbieżności wynika prawdopodobnie z tego, że sumując RSS liczymy wielokrotnie RssFile i RssShmem czyli pamięć współdzieloną, a vmstat tego nie robi (za te dane odpowiada kernel).


## Zadanie 4
```text
Zadanie 4. Na podstawie slajdów do wykładu opisz algorytm obsługi błędu stronicowania w systemie Linux.
Jakie informacje musi dostarczyć procesor, żeby można było wykonać procedurę obsługi błędu stronicowania?  
Do czego służą struktury jądra «mm_struct::pgd» i «mm_struct::mmap» zdefiniowane w pliku include/linux/mm_types.h?  
Kiedy jądro wyśle procesowi sygnał SIGSEGV z kodem «SEGV_MAPERR» lub «SEGV_ACCERR»?  
W jakiej sytuacji wystąpi pomniejsza usterka strony (ang. minor page fault) lub poważna usterka strony (ang. major page fault)? Jaką rolę pełni w systemie bufor stron (ang. page cache)?
```
![](/assets/slajd3.png)

1. Jeśli strona nie istnieje, to kernel wysyła seg fault
2. Jeśli strona istnieje, ale nie jest załadowana, to jest ładowana
3. Jeśli zrobiliśmy z pamięcią coś, na co nie pozwala jego protection, to kernel wysyła seg fault


### Jakie informacje musi dostarczyć procesor, żeby można było wykonać procedurę obsługi błędu stronicowania?  
Error code:

![](/assets/intel_error_code.png)

rejestr cr2 zawiera linear address gdzie wystapil page fault

oprócz tego zawsze gdy wywoływany jest wektor wyjątków no to otrzymujemy na stosie:
```text
If a stack switch does occur, the processor does the following:
----------- snip -------------
3. Pushes the temporarily saved SS, ESP, EFLAGS, CS, and EIP values for the interrupted procedure’s stack onto
the new stack
```

### Do czego służą struktury jądra «mm_struct::pgd» i «mm_struct::mmap» zdefiniowane w pliku include/linux/mm_types.h?

PGD jest najwyższym page table w hierarchii, służy on do tego żeby dla danej strony pamięci wirtualnej znaleźć odpowiadającą mu ramke w pamięci fizycznej

mmap wskazuje na liste dwukierunkową "aren" (czyli segmentów) procesu.  
Areny są opisane przez vm_area_struct

!!!!! Od linuxa v6.1 mm_struct nie zawiera juz struct vm_area_struct \*mmap, ponieważ ta funkcjonalnosc jest juz uzywana przez maple_tree
<https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=763ecb035029f500d7e6dc99acd1ad299b7726a1>

### Kiedy jądro wyśle procesowi sygnał SIGSEGV z kodem «SEGV_MAPERR» lub «SEGV_ACCERR»? 
SEGV_MAPERR gdy strona nie jest w ogóle podmapowana (punkt 1 naszego algorytmu)
SEGV_ACCERR gdy dotykamy strony a nie mamy do tego uprawnien (punkt 3 naszego algorytmu)


### W jakiej sytuacji wystąpi pomniejsza usterka strony (ang. minor page fault) lub poważna usterka strony (ang. major page fault)? 
<https://en.wikipedia.org/wiki/Page_fault>

Major page fault wymaga skopiowania strony z dysku, występuje gdy strona nie jest w pamięci z powodu na przykład mechanizmu swapowania  .
Minor page fault wymaga jedynie oznaczenia strony w PTE (page table entry). Może wystąpić na przykład, gdy pamięć jest dzielona i dana strona została już załadowana przez jakiś inny proces.

### Jaką rolę pełni w systemie bufor stron (ang. page cache)?
<https://github.com/firmianay/Life-long-Learner/blob/master/linux-kernel-development/chapter-16.md>

Bufor stron trzyma strony odpowiadające blokom na dysku.  
Mechanizm ten przydaje się, bo dostęp do dysku jest bardzo wolny, a niektóre bloki są często czytane, np. root directory "/".  
Gdy kernel chce przeczytać plik, to najpierw sprawdza page cache. Jeśli strona z blokiem znajduje sie w page cache'u to mamy cache hit, w przeciwnym przypadku mamy cache miss.


## Zadanie 5
```text
Zadanie 5. Chcemy rozszerzyć algorytm z poprzedniego zadania o obsługę kopiowania przy zapisie (ang. copy on write).  
W przestrzeni adresowej procesu utworzono odwzorowania prywatne segmentów pliku wykonywalnego ELF.  
Rozważmy kilkustronicowy segment danych D przechowujący sekcję «.data».  
Wiele procesów wykonuje ten sam program, zatem każdy może zmodyfikować dowolną stronę w swoim segmencie D.
Co jądro przechowuje w strukturze «vm_area_struct» opisującej segment D, a w szczególności w polach «vm_prot» i «vm_flags»?  
Jak jądro zmodyfikuje «mm_struct::pgd» w trakcie pierwszego odczytu ze strony p należącej do D, a jak w trakcie późniejszego pierwszego zapisu do p?  
Co jądro musi zrobić z tablicą stron procesu, który zawołał fork(2)?
```

### Co jądro przechowuje w strukturze «vm_area_struct» opisującej segment D, a w szczególności w polach «vm_prot» i «vm_flags»?  
<https://elixir.bootlin.com/linux/v6.0-rc1/source/arch/x86/include/asm/pgtable_types.h#L176>
vm_proto definiuje ochrone pamięci

```text
#define pgprot_val(x)		((x).pgprot)
#define __pgprot(x)		((pgprot_t) { (x) } )
#define __pg(x)			__pgprot(x)

#define PAGE_NONE	     __pg(   0|   0|   0|___A|   0|   0|   0|___G)
#define PAGE_SHARED	     __pg(__PP|__RW|_USR|___A|__NX|   0|   0|   0)
#define PAGE_SHARED_EXEC     __pg(__PP|__RW|_USR|___A|   0|   0|   0|   0)
#define PAGE_COPY_NOEXEC     __pg(__PP|   0|_USR|___A|__NX|   0|   0|   0)
#define PAGE_COPY_EXEC	     __pg(__PP|   0|_USR|___A|   0|   0|   0|   0)
#define PAGE_COPY	     __pg(__PP|   0|_USR|___A|__NX|   0|   0|   0)
#define PAGE_READONLY	     __pg(__PP|   0|_USR|___A|__NX|   0|   0|   0)
#define PAGE_READONLY_EXEC   __pg(__PP|   0|_USR|___A|   0|   0|   0|   0)
```


<https://elixir.bootlin.com/linux/v6.0-rc1/source/include/linux/mm.h#L260>
vm_flags daje nam szczegóły co do mapowania, takie jak czy strona jest dzielona, czy rosnie w dol (dla stosu), czy jest mergeable itd.
```text
/*
 * vm_flags in vm_area_struct, see mm_types.h.
 * When changing, update also include/trace/events/mmflags.h
 */
#define VM_NONE		0x00000000

#define VM_READ		0x00000001	/* currently active flags */
#define VM_WRITE	0x00000002
#define VM_EXEC		0x00000004
#define VM_SHARED	0x00000008

/* mprotect() hardcodes VM_MAYREAD >> 4 == VM_READ, and so for r/w/x bits. */
#define VM_MAYREAD	0x00000010	/* limits for mprotect() etc */
#define VM_MAYWRITE	0x00000020
#define VM_MAYEXEC	0x00000040
#define VM_MAYSHARE	0x00000080

#define VM_GROWSDOWN	0x00000100	/* general info on the segment */
#define VM_UFFD_MISSING	0x00000200	/* missing pages tracking */
#define VM_PFNMAP	0x00000400	/* Page-ranges managed without "struct page", just pure PFN */
#define VM_UFFD_WP	0x00001000	/* wrprotect pages tracking */

#define VM_LOCKED	0x00002000
#define VM_IO           0x00004000	/* Memory mapped I/O or similar */

					/* Used by sys_madvise() */
#define VM_SEQ_READ	0x00008000	/* App will access data sequentially */
#define VM_RAND_READ	0x00010000	/* App will not benefit from clustered reads */

#define VM_DONTCOPY	0x00020000      /* Do not copy this vma on fork */
#define VM_DONTEXPAND	0x00040000	/* Cannot expand with mremap() */
#define VM_LOCKONFAULT	0x00080000	/* Lock the pages covered when they are faulted in */
#define VM_ACCOUNT	0x00100000	/* Is a VM accounted object */
#define VM_NORESERVE	0x00200000	/* should the VM suppress accounting */
#define VM_HUGETLB	0x00400000	/* Huge TLB Page VM */
#define VM_SYNC		0x00800000	/* Synchronous page faults */
#define VM_ARCH_1	0x01000000	/* Architecture-specific flag */
#define VM_WIPEONFORK	0x02000000	/* Wipe VMA contents in child. */
#define VM_DONTDUMP	0x04000000	/* Do not include in the core dump */

#ifdef CONFIG_MEM_SOFT_DIRTY
# define VM_SOFTDIRTY	0x08000000	/* Not soft dirty clean area */
#else
# define VM_SOFTDIRTY	0
#endif

#define VM_MIXEDMAP	0x10000000	/* Can contain "struct page" and pure PFN pages */
#define VM_HUGEPAGE	0x20000000	/* MADV_HUGEPAGE marked this vma */
#define VM_NOHUGEPAGE	0x40000000	/* MADV_NOHUGEPAGE marked this vma */
#define VM_MERGEABLE	0x80000000	/* KSM may merge identical pages */
```

### Jak jądro zmodyfikuje «mm_struct::pgd» w trakcie pierwszego odczytu ze strony p należącej do D, a jak w trakcie późniejszego pierwszego zapisu do p?
![](/assets/intel_52.png)

W trakcie pierwszego odczytu, jeśli nie ma page entry, no to jądro ładuje strone do pamięci i ustawia page entry.  
W trakcie pierwszego zapisu, skoro korzystamy z COW, to jądro musi stworzyć nową stronę w pamięci i zaktualizować page entry, aby na nią wskazywał.  
Nowa strona musi być oznaczona do zapisu.

### Co jądro musi zrobić z tablicą stron procesu, który zawołał fork(2)?
Nowy proces dostaje kopie pgd, najprawdopodobniej strony mają ustawiony bit R/W, żeby wiedzieć kiedy wywołać page fault.


## Zadanie 6
```text
Zadanie 6. Wiemy, że jądro używa stronicowania na żądanie (ang. demand paging) dla wszystkich odwzorowań.  
Rozważmy program, który utworzył prywatne odwzorowanie pliku w pamięć. 
Czy mamy gwarancję, że program nie zobaczy modyfikacji zawartości pliku, które zostaną wprowadzone po utworzeniu tego odwzorowania?  
Próba otworzenia open(2) pliku wykonywalnego do zapisu, kiedy ten plik jest załadowany i wykonywany w jakimś procesie, zawiedzie z błędem «ETXTBSY».  
Podobnie, nie możemy załadować do przestrzeni adresowej execve(2) pliku, który jest otwarty do zapisu.  
Co złego mogłoby się stać, gdyby system operacyjny pozwolił modyfikować plik wykonywalny, który jest uruchomiony?
```

### Czy mamy gwarancję, że program nie zobaczy modyfikacji zawartości pliku, które zostaną wprowadzone po utworzeniu tego odwzorowania?  
Zróbmy test

plik test.c:
```c
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
int main() {
  struct stat stat;
  int fd = open("asdf", O_RDONLY, 0);
  fstat(fd, &stat);
  char *buf = (char*)mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE,fd,0);
  sleep(5);
  write(1, buf, stat.st_size);
}
```

plik overwrite.c:
```c
#include <unistd.h>
#include <fcntl.h>
int main() {
  int fd = open("asdf", O_RDWR, 0);
  char s[] = "bbbbbbbbbbbbbbbb";
  for (int i = 0; i < (4086*3)/16; i++)
    write(fd, s, 16);
  close(fd);
```

Zróbmy test
```sh
[crusom@crusomcarbon tmp]$ python -c "print('0' * 4096*3)" > asdf
[crusom@crusomcarbon tmp]$ ./a.out &
[2] 948294
[crusom@crusomcarbon tmp]$ ./overwrite
[crusom@crusomcarbon tmp]$ bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
```

Jak widzimy, mimo że mmap został wykonany zanim plik został nadpisany, to kto faktycznie czytamy z pliku, to czytamy z już zmodyfikowanego pliku.  
Wynika to ze stronnicowania na żądanie. Jądra czyta wczytuje strone, dopiero gdy rzeczywiście ją dotykamy.

### Co złego mogłoby się stać, gdyby system operacyjny pozwolił modyfikować plik wykonywalny, który jest uruchomiony?
Z pewnością zepsułby się stan naszego procesu, bo wykonywalibyśmy nieplanowane instrukcje.  
Atakujący może celowo nadpisać program, aby wykonać złośliwy kod (memory injection)


## Zadanie 7
Zbadajmy czas wykonania dla jednowątkowego programu:
```text
8.52user 0.28system 0:08.83elapsed 99%CPU (0avgtext+0avgdata 525380maxresident)k
0inputs+0outputs (0major+131143minor)pagefaults 0swaps
```
```sh
sudo perf record ./forksort
```
```sh
Samples: 35K of event 'cycles:P', Event count (approx.): 33303232029
Overhead  Command   Shared Object      Symbol
  65.07%  forksort  forksort           [.] Partition                                                                                                                                          ◆
   9.34%  forksort  libc.so.6          [.] random                                                                                                                                             ▒
   8.77%  forksort  forksort           [.] InsertionSort                                                                                                                                      ▒
   5.54%  forksort  forksort           [.] SwapElem                                                                                                                                           ▒
   1.36%  forksort  libc.so.6          [.] random_r                                                                                                                                           ▒
   1.14%  forksort  forksort           [.] main                                                                                                                                               ▒
   0.76%  forksort  forksort           [.] QuickSort                                                                                                                                          ▒
   0.65%  forksort  libc.so.6          [.] 0x000000000016c60d                                                                                                                                 ▒
   0.53%  forksort  libc.so.6          [.] 0x000000000016c6c0                                                                                                                                 ▒
```
Najwiecej cykli zajmuje Partition, InsertionSort i SwapElem, wszystkie są wywoływane przez QuickSort, który możemy zrównoleglić

```text
W przypadku zrównoleglania, Prawo Amdahla mówi, że jeżeli P jest proporcją programu, który może podlegać zrównolegleniu (np. korzyści z wykonywania równoległego) i ( 1 − P ) jest proporcją części, która nie może zostać zrównoleglona (pozostaje w przetwarzaniu szeregowym), wówczas maksymalne przyspieszenie jakie może być uzyskane przy użyciu N procesorów jest równe:
1 / ((1-P) + (P/N))
```

Nasze P to 65.07% + 8.77% + 5.54% + 0.76% = 80.14% = 0.80  
N to liczba procesorów, u mnie to jest N=4 dla rdzeni fizycznych i N=8 dla wirtualnych.  
Podejrzewam że interesują nas rdzenie fizyczne, bo prawo to było stworzone przed hyper-threadingiem.

Przyspieszenie wynosi 1 / (0.20 + 0.2) = 2.5

```sh
env time ./forksort 2> forksort_multithread.txt
```
```text
12.15user 0.85system 0:03.49elapsed 372%CPU (0avgtext+0avgdata 525460maxresident)k
0inputs+0outputs (0major+255070minor)pagefaults 0swaps
```

Porównajmy czasy wykonania 8.83/3.49 = 2.53

Przyspieszenie wynosiło 2.53, co jest bliskie prawu amdahla.

Możemy też zauważyć, że zużycie procesora wynosiło 372%CPU, ponieważ korzystaliśmy z 4 rdzeni, a nie tylko jednego.
