
## zadanie 1
```text
Zadanie 1. W bieżącej wersji biblioteki «libcsapp» znajdują się pliki «terminal.h» i «terminal.c».  
Przeczytaj [2 , 62.1 i 62.2], a następnie zreferuj działanie procedury «tty_curpos» odczytującej pozycję kursora terminala.  
Do czego służy kod sterujący «CPR» opisany w CSI sequences1?  
Posiłkując się tty_ioctl(4) wytłumacz semantykę rozkazów «TCGETS» i «TCSETSW», wykorzystywanych odpowiednio przez tcgetattr(3) i tcsetattr(3), oraz «TIOCINQ» i «TIOCSTI».  
Na podstawie termios(4) wyjaśnij jak flagi «ECHO», «ICANON», «CREAD» wpływają na działanie sterownika terminala.
```
### Do czego służy kod sterujący «CPR» opisany w CSI sequences1?
```text
Reports the cursor position (CPR) by transmitting ESC[n;mR, where n is the row and m is the column.
```

### Posiłkując się tty\_ioctl(4) wytłumacz semantykę rozkazów «TCGETS» i «TCSETSW», wykorzystywanych odpowiednio przez tcgetattr(3) i tcsetattr(3), oraz «TIOCINQ» i «TIOCSTI».  

```text
TCGETS Equivalent to tcgetattr(fd, argp).
Get the current serial port settings.
```

```text
TCSETSW Equivalent to tcsetattr(fd, TCSADRAIN, argp).

TCSADRAIN
      the change occurs after all output written to fd has been
      transmitted.  This option should be used when changing
      parameters that affect output.
```

```text
FIONREAD
      Get the number of bytes in the input buffer.

TIOCINQ
      Same as FIONREAD.
```

```text
TIOCSTI Insert the given byte in the input queue.
```

### Na podstawie termios(4) wyjaśnij jak flagi «ECHO», «ICANON», «CREAD» wpływają na działanie sterownika terminala.

```text
If ECHO is set, input characters	are echoed back	to the	terminal.   If
ECHO is not set,	input characters are not echoed.
```

```text
If  ICANON  is  set, canonical processing is enabled.  This enables the
erase and kill edit functions, and the  assembly	 of  input  characters
into  lines  delimited  by NL, EOF, and EOL, as described in "Canonical Mode Input Processing".
```

```text
If CREAD	is set,	the receiver is	enabled.  Otherwise, no	 character  is
received.   Not	all hardware supports this bit.	 In fact, this flag is
pretty silly and	if it were not part of the  termios  specification  it
would be	omitted.
```

## Zadanie 2
```text
Zadanie 2. Na podstawie [ 1 , 19.2] wyjaśnij działanie programu script(1).  
Nagraj interaktywną sesję z powłoką «dash» przy pomocy polecenia «script -T timing -c dash».  
Wykonaj kilka poleceń i zakończ powłokę przy pomocy polecenia «exit 42», po czym odtwórz sesję przy pomocy polecenia «scriptreplay -t timing».  
Następnie uruchom polecenie powyższe polecenie przy pomocy «strace -f -e read,write -o script.log» i na podstawie zawartości pliku «script.log» pokaż jak «script» używa pseudoterminala do komunikacji z programami działającymi pod kontrolą powłoki «dash».  
Pokaż, że sterownik terminala przepisuje znaki zgodnie z flagami «ICRNL» i «ONLCR» opisanymi w termios(4).
```

```text
ioctl(1, TCGETS, {c_iflag=ICRNL|IXON, c_oflag=NL0|CR0|TAB0|BS0|VT0|FF0|OPOST|ONLCR, c_cflag=B38400|CS8|CREAD, c_lflag=ISIG|ICANON|ECHO|ECHOE|ECHOK|IEXTEN|ECHOCTL|ECHOKE, ...}) = 0
ioctl(1, TCSETS, {c_iflag=IXON,       c_oflag=NL0|CR0|TAB0|BS0|VT0|FF0|ONLCR,       c_cflag=B38400|CS8|CREAD, c_lflag=ISIG|            ECHOE|ECHOK|       ECHOCTL|ECHOKE, ...}) = 0
```

Wyłącza icanon, echo, iexten, opost i icrnl
icanon - tryb kanoniczny
echo - echo
iexten-  	     IEXTEN	 enable DISCARD and LNEXT 
opost -oznajmia czy interpretowac c_oflag

ready i write'y:
```sh
248060 read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\340_\2\0\0\0\0\0"..., 832) = 832
248060 read(3, "0.007878 2\n2.652390 1\n0.043096 1"..., 4096) = 309
248060 read(4, "Script started on 2024-10-27 21:"..., 4096) = 239
248060 write(1, "$ ", 2)                = 2
248060 write(1, "l", 1)                 = 1
248060 write(1, "s", 1)                 = 1
248060 write(1, "\10 \10", 3)           = 3
248060 write(1, "\10 \10", 3)           = 3
248060 write(1, "c", 1)                 = 1
248060 write(1, "a", 1)                 = 1
248060 write(1, "t", 1)                 = 1
248060 write(1, " ", 1)                 = 1
248060 write(1, "\10 \10\10 \10\10 \10\10 \10", 12) = 12
248060 write(1, "e", 1)                 = 1
248060 write(1, "c", 1)                 = 1
248060 write(1, "h", 1)                 = 1
248060 write(1, "o", 1)                 = 1
248060 write(1, " ", 1)                 = 1
248060 write(1, "'", 1)                 = 1
248060 write(1, "a", 1)                 = 1
248060 write(1, "'", 1)                 = 1
248060 write(1, "\r\n", 2)              = 2
248060 write(1, "a\r\n$ ", 5)           = 5
248060 write(1, "e", 1)                 = 1
248060 write(1, "x", 1)                 = 1
248060 write(1, "i", 1)                 = 1
248060 write(1, "t", 1)                 = 1
248060 write(1, " ", 1)                 = 1
248060 write(1, "4", 1)                 = 1
248060 write(1, "2", 1)                 = 1
248060 read(3, "", 4096)                = 0
248060 write(1, "\r\n", 2)              = 2
248060 write(1, "\n", 1)                = 1
248060 +++ exited with 0 +++
```

```text
The script process acts as a proxy for the user, taking input entered at the terminal and writing it to the
pseudoterminal master, and reading output from the pseudoterminal master and writing it to the user’s terminal.
```
![](assets/heh.png)


## Zadanie 3
```text
Zadanie 3. Uruchom potok (ang. pipeline) «ps -ef | grep sh | wc -l > cnt» w powłoce utwo-
rzonej przy pomocy polecenia «strace -o pipeline.log -f dash». Na podstawie zawartości pliku
«pipeline.log» opisz jak powłoka realizuje funkcje łączenia procesów rurami (ang. pipe) i wykonuje
przekierowanie standardowego wyjścia do pliku. W szczególności wskaż które procesy i w jakiej kolejno-
ści będą wołały następujące wywołania systemowe: openat(2) z flagą «O_CREAT» (realizuje creat(2)),
dup2(2), pipe(2), close(2), clone(2) (realizuje fork(2)) i execve(2). Zwróć szczególną uwagę na to
kiedy powłoka tworzy rury i kiedy są zamykane ich poszczególne końce.
```
```sh
86905 - dash
86911 - ps
86912 - grep
86913 - wc

# Start the shell process (dash), which initializes the pipeline.
86905 execve("/usr/bin/dash", ["dash"], 0x7ffcaab9fee8 /* 41 vars */) = 0

# Create a pipe with file descriptors 3 (read end) and 4 (write end) to connect `ps` output to `grep`.
86905 pipe2([3, 4], 0) = 0

# Fork a new process (86911) to execute the `ps -ef` command.
86905 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7c0243f1de50) = 86911
# Child process 86911 (`ps`):
    # Close the read end of the pipe as it will only write to `grep`.
    86911 close(3) = 0
    # Duplicate FD 4 to FD 1 (stdout), redirecting output to the pipe connected to `grep`.
    86911 dup2(4, 1) <unfinished ...>
    # Close FD 4 after redirecting stdout; all output now goes to `grep`.
    86911 close(4) <unfinished ...>
    # Execute the `ps -ef` command, listing all processes.
    86911 execve("/usr/bin/ps", ["ps", "-ef"], 0x5c5416bfbc20 /* 41 vars */ <unfinished ...>

# Parent process closes the write end of the pipe, as it will be used by `ps`.
86905 close(4) = 0

# Create another pipe (FD 4 for reading, FD 5 for writing) to connect `grep` output to `wc`.
86905 pipe2([4, 5], 0) = 0

# Fork another process (86912) to execute `grep sh`.
86905 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7c0243f1de50) = 86912
# Child process 86912 (`grep`):
    # Close FD 4, as `grep` only reads input from the pipe.
    86912 close(4) = 0
    # Redirect stdin (FD 0) to FD 3, so it reads from `ps`'s output pipe.
    86912 dup2(3, 0) = 0
    # Close FD 3 after redirecting it to stdin; `grep` now reads only from `ps`.
    86912 close(3) <unfinished ...>
    # Duplicate FD 5 to FD 1 (stdout), redirecting output to the `wc` pipe.
    86912 dup2(5, 1) <unfinished ...>
    # Close FD 5 after redirecting stdout; output now goes to `wc`.
    86912 close(5) = 0
    # Execute the `grep sh` command, filtering for lines containing "sh".
    86912 execve("/usr/bin/grep", ["grep", "sh"], 0x5c5416bfbc20 /* 41 vars */ <unfinished ...>

# Parent process closes FD 5 (write end of the second pipe) as it will be used by `wc`.
86905 close(5 <unfinished ...>

# Fork a new process to handle the `wc -l` command.
86905 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD <unfinished ...>
# Child process 86913 (`wc`):
    # Redirect stdin to FD 4 so `wc` can read from `grep` output.
    86913 dup2(4, 0) = 0
    # Close FD 4 after redirecting stdin.
    86913 close(4) = 0
    # Open the file "cnt" for writing, creating it if it doesn’t exist and truncating it if it does; assigns it FD 3.
    86913 openat(AT_FDCWD, "cnt", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
    # Close its stdout (FD 1) to prepare redirecting output to the file `cnt`.
    86913 close(1) = 0
    # Duplicate FD 3 to FD 1 (stdout), so output is redirected to "cnt".
    86913 dup2(3, 1) = 1
    # Close FD 3 after redirecting stdout to it; output now goes to file "cnt".
    86913 close(3) = 0
    # Execute the `wc -l` command, counting lines from `grep` and saving the result in "cnt".
    86913 execve("/usr/bin/wc", ["wc", "-l"], 0x5c5416bfbc38 /* 41 vars */ <unfinished ...>

# Parent shell process performs cleanup, closing any remaining unnecessary FDs.
86905 close(-1 <unfinished ...>
```

## Zadanie 4
```text
Zadanie 4. Przyjrzyjmy się raz jeszcze plikowi «pipeline.log» z poprzedniego zadania. Zauważ, że wszyst-
kie procesy należące do potoku muszą zostać umieszczone w jednej grupie procesów. Wskaż kiedy powłoka
tworzy nową grupę procesów i jak umieszcza tam procesy realizujące potok. Przeczytaj [ 2, 34.2] i wyja-
śnij czemu dla każdego podprocesu wywołanie setpgid(2) jest robione zarówno w procesie powłoki jak
i w procesie potomnym? Kiedy powłoka ustala grupę pierwszoplanową przy pomocy ioctl(2) (realizuje
tcsetpgrp(3))? Na jakiej podstawie powłoka wyznacza kod wyjścia potoku?
```

```sh
  1 145046 execve("/usr/bin/dash", ["dash"], 0x7ffec3e0bfa8 /* 41 vars */) = 0
  2 145046 setpgid(0, 145046)               = 0
  3 145046 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x73b39187de50) = 145093
  4 145046 setpgid(145093, 145093)          = 0
  5 145093 setpgid(0, 145093)               = 0
  6 145046 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x73b39187de50) = 145094
  7 145046 setpgid(145094, 145093)          = 0
  8 145094 setpgid(0, 145093)               = 0
  9 145046 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD <unfinished ...>
 10 145093 execve("/usr/bin/ps", ["ps", "-ef"], 0x5fb29fc65c20 /* 41 vars */ <unfinished ...>
 11 145046 <... clone resumed>, child_tidptr=0x73b39187de50) = 145095
 12 145046 setpgid(145095, 145093)          = 0
 13 145095 setpgid(0, 145093)               = 0
 14 145094 execve("/usr/bin/grep", ["grep", "sh"], 0x5fb29fc65c20 /* 41 vars */ <unfinished ...>
 15 145093 <... execve resumed>)            = 0
 16 145094 <... execve resumed>)            = 0
 17 145095 execve("/usr/bin/wc", ["wc", "-l"], 0x5fb29fc65c38 /* 41 vars */) = 0
```

### Przeczytaj [ 2, 34.2] i wyjaśnij czemu dla każdego podprocesu wywołanie setpgid(2) jest robione zarówno w procesie powłoki jak i w procesie potomnym?
```text
However, because the scheduling of the parent and child is indeterminate after a fork() (Section 24.4), we can’t rely on the parent changing
the child’s process group ID before the child does an exec(); nor can we rely on the
child changing its process group ID before the parent tries to send any job-control
signals to it. (Dependence on either one of these behaviors would result in a race con-
dition.) Therefore, job-control shells are programmed so that the parent and the
child process both call setpgid() to change the child’s process group ID to the same
value immediately after a fork(), and the parent ignores any occurrence of the
EACCES error on the setpgid() call. In other words, in a job-control shell, we’ll find
code something like that shown in Listing 34-1
```

### Kiedy powłoka ustala grupę pierwszoplanową przy pomocy ioctl(2) (realizuje tcsetpgrp(3))? Na jakiej podstawie powłoka wyznacza kod wyjścia potoku?
Na początku powłoka ustawia grupe pierwszoplanowa na siebie
```sh
147228 openat(AT_FDCWD, "/dev/tty", O_RDWR) = 3
147228 fcntl(3, F_DUPFD, 10)            = 10
147228 close(3)                         = 0
147228 fcntl(10, F_SETFD, FD_CLOEXEC)   = 0
147228 ioctl(10, TIOCGPGRP, [147225])   = 0
147228 getpgrp()                        = 147225
ioctl(10, TIOCSPGRP, [147228])   = 0
```

Po wywołaniu komendy tworzony jest nowy proces (pozniej stanie sie p), który staje się grupą pierwszoplanową.  
Reszcie procesów w pipe'a shell przyporządkowuje te samą grupę pierwszoplanową, czyli pierwszy proces.

Kod wyjścia zależy od kodu wyjścia ostatniego procesu.


## Zadanie 5. 
```text
Zadanie 5. Czemu nie można czytać i modyfikować katalogów przy pomocy wywołań read(2) i write(2)?
Jakim wywołaniem systemowym można wczytać rekord katalogu (ang. directory entry )? 
Dlaczego zawartość katalogu nie jest posortowana? Wyświetl metadane katalogu głównego «/» przy pomocy polecenia «stat»,
a następnie wyjaśnij z czego wynika podana liczba dowiązań (ang. hard link)?
```

## Zadanie 5. Czemu nie można czytać i modyfikować katalogów przy pomocy wywołań read(2) i write(2)?
Bo zwracają rekordy a nie raw bajty.

### Jakim wywołaniem systemowym można wczytać rekord katalogu (ang. directory entry )? 
readdir()

### Dlaczego zawartość katalogu nie jest posortowana?
Myślę, że łatwiej posortować dynamicznie output readdir(), niż grzebać w filesystemie, nalezy jak najmniej czasu spedzac nad io

### Wyświetl metadane katalogu głównego «/» przy pomocy polecenia «stat», a następnie wyjaśnij z czego wynika podana liczba dowiązań (ang. hard link)?
```sh
[crusom@crusomcarbon ~]$ stat /
  File: /
  Size: 4096            Blocks: 8          IO Block: 4096   directory
Device: 254,2   Inode: 2           Links: 18
Access: (0755/drwxr-xr-x)  Uid: (    0/    root)   Gid: (    0/    root)
Access: 2024-10-29 21:39:27.486898484 +0100
Modify: 2013-01-28 04:54:18.000000000 +0100
Change: 2024-07-17 20:17:41.250009250 +0200
 Birth: 2023-10-20 13:38:56.000000000 +0200
```

### a następnie wyjaśnij z czego wynika podana liczba dowiązań (ang. hard link)?
".", ".." i ".." w podkatalogach

## Zadanie 6.
```text
Zadanie 6. Intencją autora poniższego kodu było użycie plików jako blokad międzyprocesowych. Istnienie
pliku o podanej nazwie w systemie plików oznacza, że blokada została założona. Brak tegoż pliku, że blokadę
można założyć. Niestety w poniższym kodzie jest błąd TOCTTOU2, który opisano również w [4 , 39.17].
Zlokalizuj w poniższym kodzie wyścig i napraw go! Opowiedz jakie zagrożenia niesie ze sobą taki błąd.
1 #include "csapp.h"
2
3 bool f_lock(const char *path) {
4 if (access(path, F_OK) == 0)
5 return false;
6 (void)Open(path, O_CREAT|O_WRONLY, 0700);
7 return true;
8 }
9
10 void f_unlock(const char *path) {
11 Unlink(path);
12 }
```

```c
1 #include "csapp.h"
2
3 bool f_lock(const char *path) {
    return Open(path, O_CREAT|O_EXCL|O_WRONLY, 0700) != -1;
8 }
9
10 void f_unlock(const char *path) {
11 Unlink(path);
12 }
```


## Zadanie 7
```text
Zadanie 7. Program «leaky» symuluje aplikację, która posiada dostęp do danych wrażliwych. Pod deskryp-
torem pliku o nieustalonym numerze kryje się otwarty plik «mypasswd». W wyniku normalnego działania
«leaky» uruchamia zewnętrzny program «innocent» dostarczony przez złośliwego użytkownika.
Uzupełnij kod programu «innocent», aby przeszukał otwarte deskryptory plików, a następnie przepisał
zawartość otwartych plików do pliku «/tmp/hacker». Zauważ, że pliki zwykłe posiadają kursor. Do pliku
wyjściowego należy wpisać również numer deskryptora pliku i ścieżkę do pliku, tak jak na poniższym wydruku:
1 File descriptor 826 is ’/home/cahir/lista_4/mypasswd’ file!
2 cahir:...:0:0:Krystian Baclawski:/home/cahir:/bin/bash
Żeby odnaleźć nazwę pliku należy wykorzystać zawartość katalogu «/proc/self/fd» opisaną w procfs(5).
Potrzebujesz odczytać plik docelowy odpowiedniego dowiązania symbolicznego przy pomocy readlink(2).
Następnie napraw program «leaky» – zakładamy, że nie może on zamknąć pliku z wrażliwymi danymi.
Wykorzystaj fcntl(2) do ustawienia odpowiedniej flagi deskryptora wymienionej w open(2).
Zainstaluj pakiet «john» (John The Ripper3). Następnie złam hasło znajdujące się pliku, który wyciekł w wy-
niku podatności pozostawionej przez programistę, który nie przeczytał uważnie podręcznika do execve(2).
Wskazówka: Procedura «dprintf» drukuje korzystając z deskryptora pliku, a nie struktury «FILE»
```

```sh
[crusom@crusomcarbon so21_lista_4]$ john <(echo "tQkrCdv1bf6aU")
----- snip ------
Proceeding with wordlist:/usr/share/john/password.lst, rules:Wordlist
Proceeding with incremental:ASCII
Warning: MaxLen = 13 is too large for the current hash type, reduced to 8
cirilla          (?)
1g 0:00:00:03 DONE 3/3 (2024-10-30 19:53) 0.2770g/s 2580Kp/s 2580Kc/s 2580KC/s cine143..aprume!
Use the "--show" option to display all of the cracked passwords reliably
Session completed
```


## Zadanie 8
```text
Zadanie 8. Uruchom program «mkholes», a następnie odczytaj metadane pliku «holes.bin» przy pomocy
polecenia stat(1). Wszystkie pola struktury «stat» są opisane w stat(2). Oblicz faktyczną objętość pliku
na podstawie liczby używanych bloków «st_blocks» i rozmiaru pojedynczego bloku «st_blksize» systemu
pliku. Czemu liczba używanych bloków jest mniejsza od tej wynikającej z objętości pliku z pola «st_size»?
Czemu jest większa od liczby faktycznie używanych bloków zgłaszanych przez «mkholes»? Wyjaśnij to
zjawisko na podstawie [1, 3.6]
```

```text
[crusom@crusomcarbon so21_lista_4]$ stat holes.bin
  File: holes.bin
  Size: 33550336        Blocks: 1112       IO Block: 4096   regular file
Device: 254,2   Inode: 11840711    Links: 1
Access: (0644/-rw-r--r--)  Uid: ( 1000/  crusom)   Gid: ( 1000/  crusom)
Access: 2024-10-30 19:56:00.422926389 +0100
Modify: 2024-10-30 19:54:53.659595394 +0100
Change: 2024-10-30 19:54:53.659595394 +0100
 Birth: 2024-10-30 19:54:50.246262181 +0100
```


<https://elixir.bootlin.com/linux/v6.11.5/source/include/uapi/asm-generic/stat.h#L24>
```text
struct stat {
	unsigned long	st_dev;		/* Device.  */
	unsigned long	st_ino;		/* File serial number.  */
	unsigned int	st_mode;	/* File mode.  */
	unsigned int	st_nlink;	/* Link count.  */
	unsigned int	st_uid;		/* User ID of the file's owner.  */
	unsigned int	st_gid;		/* Group ID of the file's group. */
	unsigned long	st_rdev;	/* Device number, if device.  */
	unsigned long	__pad1;
	long		st_size;	/* Size of file, in bytes.  */
	int		st_blksize;	/* Optimal block size for I/O.  */
	int		__pad2;
	long		st_blocks;	/* Number 512-byte blocks allocated. */
	long		st_atime;	/* Time of last access.  */
	unsigned long	st_atime_nsec;
	long		st_mtime;	/* Time of last modification.  */
	unsigned long	st_mtime_nsec;
	long		st_ctime;	/* Time of last status change.  */
	unsigned long	st_ctime_nsec;
	unsigned int	__unused4;
	unsigned int	__unused5;
};
```
Sektory majo po 512 bajtów, można sprawdzić zawsze `sudo fdisk -l`
