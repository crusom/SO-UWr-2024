
## Zadanie 1
### W nowej powłoce wystartuj «sleep 1000» jako zadanie drugoplanowe i sprawdź, kto jest jego rodzicem. 
```sh
[crusom@crusomcarbon ~]$ ps -o pid,ppid,tty,time,cmd
    PID    PPID TT           TIME CMD
1057319 1057316 pts/0    00:00:00 bash -i
1057449 1057319 pts/0    00:00:00 sleep 1000
1057470 1057319 pts/0    00:00:00 ps -o pid,ppid,tty,time,cmd
```
### Poleceniem «kill» wyślij sygnał «SIGKILL» do uruchomionej wcześniej powłoki i sprawdź, kto stał się nowym rodzicem procesu «sleep»
```sh
[crusom@crusomcarbon ~]$ ps -eo pid,ppid,tty,time,cmd | grep sleep
1057449       1 ?        00:00:00 sleep 1000
1058878  985342 pts/3    00:00:00 grep sleep
```
### Co się dzieje z sesją, która utraci terminal sterujący? 
jest tracona.  
Bash wysyła SIGHUP do grupy procesów oraz do całej sesji  

```sh
[crusom@crusomcarbon ~]$ ps -eo pid,ppid,sid,tty,time,cmd | tail
1094871       2       0 ?        00:00:00 [kworker/0:1-events]
1094948       2       0 ?        00:00:00 [kworker/2:2]
1095031 1071000 1071000 pts/0    00:00:00 xterm -ls -xrm XTerm*selectToClipboard:true -fa LiberationMono-Bold -fs 14 -e bash -i
1095033 1095031 1095033 pts/3    00:00:00 bash -i
1095264 1095033 1095033 pts/3    00:00:00 sleep 1000
1095273 1071000 1071000 pts/0    00:00:00 sudo strace -e trace=signal -p 1095033
1095275 1095273 1095275 pts/4    00:00:00 sudo strace -e trace=signal -p 1095033
1095276 1095275 1095275 pts/4    00:00:00 strace -e trace=signal -p 1095033
1095286 1093657 1093657 pts/5    00:00:00 ps -eo pid,ppid,sid,tty,time,cmd
1095287 1093657 1093657 pts/5    00:00:00 tail
```
Wysyłany jest kill.  

```sh
--- SIGHUP {si_signo=SIGHUP, si_code=SI_KERNEL} ---
--- SIGCONT {si_signo=SIGCONT, si_code=SI_KERNEL} ---
rt_sigreturn({mask=[]})                 = 1
rt_sigprocmask(SIG_BLOCK, NULL, [], 8)  = 0
rt_sigaction(SIGINT, {sa_handler=0x565ed280f0d0, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGHUP, {sa_handler=0x565ed280dbd0, sa_mask=[HUP INT ILL TRAP ABRT BUS FPE USR1 SEGV USR2 PIPE ALRM TERM XCPU XFSZ VTALRM SYS], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGALRM, {sa_handler=0x565ed280dbd0, sa_mask=[HUP INT ILL TRAP ABRT BUS FPE USR1 SEGV USR2 PIPE ALRM TERM XCPU XFSZ VTALRM SYS], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGWINCH, {sa_handler=0x565ed28062a0, sa_mask=[], sa_flags=SA_RESTORER|SA_RESTART, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11350, sa_mask=[], sa_flags=SA_RESTORER|SA_RESTART, sa_restorer=0x74ca90a2d1d0}, 8) = 0
kill(1095033, SIGHUP)                   = 0
--- SIGHUP {si_signo=SIGHUP, si_code=SI_USER, si_pid=1095033, si_uid=1000} ---
rt_sigreturn({mask=[]})                 = 0
rt_sigprocmask(SIG_BLOCK, [HUP INT QUIT ALRM TERM TSTP TTIN TTOU], [], 8) = 0
rt_sigaction(SIGINT, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x565ed280f0d0, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGTERM, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGTERM, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGHUP, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x565ed280dbd0, sa_mask=[HUP INT ILL TRAP ABRT BUS FPE USR1 SEGV USR2 PIPE ALRM TERM XCPU XFSZ VTALRM SYS], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGQUIT, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGQUIT, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGALRM, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x565ed280dbd0, sa_mask=[HUP INT ILL TRAP ABRT BUS FPE USR1 SEGV USR2 PIPE ALRM TERM
XCPU XFSZ VTALRM SYS], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGTSTP, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGTSTP, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGTTOU, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGTTOU, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGTTIN, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGTTIN, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGTTIN, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
rt_sigaction(SIGWINCH, {sa_handler=0x74ca90c11350, sa_mask=[], sa_flags=SA_RESTORER|SA_RESTART, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x565ed28062a0, sa_mask=[], sa_flags=SA_RESTORER|SA_RESTART, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGINT, {sa_handler=0x565ed280f0d0, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGHUP, {sa_handler=0x565ed280dbd0, sa_mask=[HUP INT ILL TRAP ABRT BUS FPE USR1 SEGV USR2 PIPE ALRM TERM XCPU XFSZ VTALRM SYS], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGALRM, {sa_handler=0x565ed280dbd0, sa_mask=[HUP INT ILL TRAP ABRT BUS FPE USR1 SEGV USR2 PIPE ALRM TERM XCPU XFSZ VTALRM SYS], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11340, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
rt_sigaction(SIGWINCH, {sa_handler=0x565ed28062a0, sa_mask=[], sa_flags=SA_RESTORER|SA_RESTART, sa_restorer=0x74ca90a2d1d0}, {sa_handler=0x74ca90c11350, sa_mask=[], sa_flags=SA_RESTORER|SA_RESTART, sa_restorer=0x74ca90a2d1d0}, 8) = 0
kill(-1095264, SIGHUP)                  = 0
rt_sigprocmask(SIG_BLOCK, [CHLD], [], 8) = 0
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_KILLED, si_pid=1095264, si_uid=1000, si_status=SIGHUP, si_utime=0, si_stime=0} ---
rt_sigreturn({mask=[]})                 = 0
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
rt_sigaction(SIGHUP, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0},
{sa_handler=0x565ed280dbd0, sa_mask=[HUP INT ILL TRAP ABRT BUS FPE USR1 SEGV USR2 PIPE ALRM TERM XCPU XFSZ VTALRM SYS], sa_flags=SA_RESTORER, sa_restorer=0x74ca90a2d1d0}, 8) = 0
kill(1095033, SIGHUP)                   = 0
--- SIGHUP {si_signo=SIGHUP, si_code=SI_USER, si_pid=1095033, si_uid=1000} ---
+++ killed by SIGHUP +++
```

Bash otrzymał sighup od kernela, ustawił handlery, po czym wysłał sighup do samego siebie.  
Następie dalej ustawiał handlery na SIGIGN i wysłał SIGHUP dla grupy procesów do której należał sleep, czyli chyba do background job.  
Dostał informacje, że dziecko zostało zabite, po czym ustawił SIGHUP na default action i wysłał SIGHUP do samego siebie, zabijając się w ten sposób.  

## Zadanie 2

### Jak zachowuje się sterownik terminala działającego w trybie kanonicznym?

Terminal input przetwarza linie tekstu.  
Linia kończy się znakiem nowej linii ('\n'), EOF albo EOL.  
EOF, EOL i NL to tak zwane znaki specjalne, które informują terminal że ma coś zrobić. Nie są one przekazywane jako input.  

```text
     EOF     Special character on input and is recognized if the ICANON flag
             is set.  When received, all the bytes waiting to be read are
             immediately passed to the process, without waiting for a newline,
             and the EOF is discarded.  Thus, if there are no bytes waiting
             (that is, the EOF occurred at the beginning of a line), a byte
             count of zero is returned from the read(2), representing an end-
             of-file indication.  If ICANON is set, the EOF character is dis-
             carded when processed."
```

Niektóre znaki specjalne pozwalają edytować nasz tekst np. WERASE wymazuje ostatni wyraz, ERASE wymazuje ostatni znak, a KILL usuwa całą linię.  

Istnieją input modes i output modes, które ustala się poprzez flagi c_iflags i c_oflag.

```text
   Input Modes
     Values of the c_iflag field describe the basic terminal input control,
     and are composed of following masks:

           IGNBRK   /* ignore BREAK condition */
           BRKINT   /* map BREAK to SIGINT */
           IGNPAR   /* ignore (discard) parity errors */
           PARMRK   /* mark parity and framing errors */
           INPCK    /* enable checking of parity errors */
           ISTRIP   /* strip 8th bit off chars */
           INLCR    /* map NL into CR */
           IGNCR    /* ignore CR */
           ICRNL    /* map CR to NL (ala CRMOD) */
           IXON     /* enable output flow control */
           IXOFF    /* enable input flow control */
           IXANY    /* any char will restart after stop */
           IMAXBEL  /* ring bell on input queue full */
```

```text
   Output Modes
     Values of the c_oflag field describe the basic terminal output control,
     and are composed of the following masks:

           OPOST   /* enable following output processing */
           ONLCR   /* map NL to CR-NL (ala CRMOD) */
           OCRNL   /* map CR to NL */
           OXTABS  /* expand tabs to spaces */
           ONOEOT  /* discard EOT's (^D) on output */
           ONOCR   /* do not transmit CRs on column 0 */
           ONLRET  /* on the terminal NL performs the CR function */
```

Oprócz tego są też local modes, które ustalają bardziej ogólne funkcje terminala.  

```text

   Local Modes
     Values of the c_lflag field describe the control of various functions,
     and are composed of the following masks.

           ECHOKE      /* visual erase for line kill */
           ECHOE       /* visually erase chars */
           ECHO        /* enable echoing */
           ECHONL      /* echo NL even if ECHO is off */
           ECHOPRT     /* visual erase mode for hardcopy */
           ECHOCTL     /* echo control chars as ^(Char) */
           ISIG        /* enable signals INTR, QUIT, [D]SUSP */
           ICANON      /* canonicalize input lines */
           ALTWERASE   /* use alternative WERASE algorithm */
           IEXTEN      /* enable DISCARD and LNEXT */
           EXTPROC     /* external processing */
           TOSTOP      /* stop background jobs from output */
           FLUSHO      /* output being flushed (state) */
           NOKERNINFO  /* no kernel output from VSTATUS */
           PENDIN      /* re-echo input buffer at next read */
           NOFLSH      /* don't flush output on signal */
```
### Posługując się rysunkiem [1, 62-1] wytłumacz w jaki sposób przetwarza on znaki (w tym kody sterujące) wchodzące do kolejki wejściowej i kolejki wyjściowej

![18.1 aka 62-1](/assets/queue.png)


### jak konfigurację terminala powinien zmienić program na czas wpisywania hasła przez użytkownika
wyłączć echo.

Demko :333
w pierwszym terminalu wpisuje
```sh
[crusom@crusomcarbon ~]$ tty
/dev/pts/5
[crusom@crusomcarbon ~]$ su
Password:
```
w drugim sprawdzam ustawienia tego ptsa
```sh
[crusom@crusomcarbon ~]$ stty -F /dev/pts/5 -a
speed 38400 baud; rows 49; columns 85; line = 0;
intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>; eol2 = <undef>; swtch = <undef>;
start = ^Q; stop = ^S; susp = ^Z; rprnt = ^R; werase = ^W; lnext = ^V; discard = ^O; min = 1; time = 0;
-parenb -parodd -cmspar cs8 -hupcl -cstopb cread -clocal -crtscts
-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr icrnl ixon -ixoff -iuclc -ixany -imaxbel
-iutf8
opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
isig icanon iexten -echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke -flusho
-extproc
```

jak widać echo jest wylaczone

### Czemu edytory takie jak vi(1) konfigurują sterownik terminala do pracy w trybie niekanonicznym?

Bo definiują inne akcje dla np ctrl+c i inputu nie pobierają jedynie dla linii.

### ciekawostka
jesli uzyjemy stty -F /dev/pts/5 -a, gdy ten pts/5 nie wykonuje zadnego programu, to dostaniemy troche inny output.  

```sh
[crusom@crusomcarbon ~]$ stty -F /dev/pts/5 -a
speed 38400 baud; rows 49; columns 85; line = 0;
intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>; eol2 = <undef>; swtch = <undef>;
start = ^Q; stop = ^S; susp = ^Z; rprnt = ^R; werase = ^W; lnext = <undef>; discard = <undef>;
min = 1; time = 0;
-parenb -parodd -cmspar cs8 -hupcl -cstopb cread -clocal -crtscts
-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl ixon -ixoff -iuclc -ixany -imaxbel
-iutf8
opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
isig -icanon iexten -echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke -flusho
-extproc
```


```sh
[crusom@crusomcarbon ~]$ stty -a
speed 38400 baud; rows 49; columns 85; line = 0;
intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>; eol2 = <undef>;
swtch = <undef>; start = ^Q; stop = ^S; susp = ^Z; rprnt = ^R; werase = ^W;
lnext = ^V; discard = ^O; min = 1; time = 0;
-parenb -parodd -cmspar cs8 -hupcl -cstopb cread -clocal -crtscts
-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr icrnl ixon -ixoff -iuclc
-ixany -imaxbel -iutf8
opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
isig icanon iexten echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl
echoke -flusho -extproc
```

Odpowiedz znajdziemy na <https://unix.stackexchange.com/questions/226448/why-do-the-terminal-attributes-look-different-from-outside-the-terminal>
```text
Bash needs to put the terminal into character-at-a-time mode while it's waiting for you to type in a command line, so that you can edit the command line using emacs or vi-like editing characters. That's the mode you saw when you looked at the terminal's attributes from another terminal in your example.

Just before it runs a program (in your example, stty), bash puts the terminal back into canonical mode, where you have just a few special editing characters available courtesy of the operating system, such as backspace and Control-W, and basically the program gets input only after you type Enter.

When bash regains control, say after the program finishes or is suspended, it will put the terminal into character-at-a-time mode again.
```


## Zadanie 3
```text
Wyświetl konfigurację terminala przy pomocy polecenia «stty -a». Wskaż znaki, które sterownik
terminala: zamienia na sygnały związane z zarządzaniem zadaniami, służą do edycji wiersza. Program
może zostać poinformowany o zmianie rozmiaru okna terminala. W tym celu musi zainstalować procedurę
obsługi sygnału – którego? Jaką procedurą można wczytać nowy rozmiar okna?
```

### Wyświetl konfigurację terminala przy pomocy polecenia «stty -a». Wskaż znaki, które sterownik terminala: zamienia na sygnały związane z zarządzaniem zadaniami, służą do edycji wiersza. Program


```sh
[crusom@crusomcarbon ~]$ stty -a
speed 38400 baud; rows 49; columns 191; line = 0;
intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>; eol2 = <undef>; swtch = <undef>; start = ^Q; stop = ^S; susp = ^Z; rprnt = ^R; werase = ^W; lnext = ^V; discard = ^O;
min = 1; time = 0;
-parenb -parodd -cmspar cs8 -hupcl -cstopb cread -clocal -crtscts
-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr icrnl ixon -ixoff -iuclc -ixany -imaxbel -iutf8
opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
isig icanon iexten echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke -flusho -extproc
```

zarządzanie zadaniami:

- intr (wysyła SIGINT do obecnego process group, dla którego terminal jest controlling)
- quit (jak wyżej, ale wysyła SIGQUIT), 
- susp (jak wyżej, ale wysyła SIGTSTP),

edycja wiersza:

- erase (usuwa znak),
- kill (usuwa linie),
- eof (sygnalizuje koniec pliku i przekazuje input do procesu),
- eol2 (jak wyzej ale inny znak),
- swtch (pojecia nie mam),
- start (wznawia output, jeśli został zawieszony przez stop),
- stop (zawiesa output),
- rprnt (Causes the current input edit line to be retyped),
- werase (usuwa slowo),
- lnext (następny znak będzie brany dosłownie, więc możemy użyć lnext, a potem wklepac jakis special character np.),
- discard (flushuje output)


### Program może zostać poinformowany o zmianie rozmiaru okna terminala. W tym celu musi zainstalować procedurę obsługi sygnału – którego? Jaką procedurą można wczytać nowy rozmiar okna?

Sygnał to SIGWINCH.  
Wczytać rozmiar można przez ioctl z flagą TIOCGWINSZ  
<https://www.man7.org/linux/man-pages/man2/TIOCGWINSZ.2const.html>

```c
static void sig_handler(int sig)
{
  if (SIGWINCH == sig) {
    struct winsize winsz;

    ioctl(0, TIOCGWINSZ, &winsz);
    printf("SIGWINCH raised, window size: %d rows / %d columns\n",
           winsz.ws_row, winsz.ws_col);
  }

/} // sig_handle
```


## Zadanie 4
```text
Zadanie 4. Urządzenie terminala zajmuje się interpretacją znaków i sekwencji sterujących2 przychodzących
od sterownika terminala, oraz przetwarzaniem zdarzeń od użytkownika, które zostają zamienione na znaki
lub sekwencje znaków, a następnie wysłane do sterownika terminala.  
Posługując się poleceniem «echo -e ’sterujacy’; read» zaprezentuj działanie znaków sterujących oraz sekwencji «CSI».  
Uruchom polecenie «cat» i sprawdź jak zachowuje się naciśnięcie klawiszy funkcyjnych i kursora.  
Czemu w tym przypadku zachowanie programu «cat» jest inne niż powłoki poleceń?  
```

```text
Together, a particular triplet of UART driver, line discipline instance and TTY driver may be referred to as a TTY device, or sometimes just TTY.
```

### Posługując się poleceniem «echo -e ’sterujacy’; read» zaprezentuj działanie znaków sterujących oraz sekwencji «CSI».  

<https://notes.burke.libbey.me/ansi-escape-codes/>
ANSI Escape Codes pozwalają nam przesuwać kursor, scrollowac okno, ustawiac tryb graficzny, ale tez ustawiac kolory tekstu, ich tło, ich tryb (italics, bold).

Możemy użyć jakiegos ansi code generatora, jesli nie chce nam sie patrzec ciagle w docsy
<https://ansi.gabebanks.net/>


```sh
[crusom@crusomcarbon ~]$ echo -e "\x1b[32;49m love unix \x1b[0m"; read
 love unix

[crusom@crusomcarbon ~]$
```

If you see \x1b[32;34m, you can read it like this:

\x1b[  # call a function
32;49 # function arguments (32, 49)
m      # function name

SGR to set graphic mode.

Potem używamy \x1b[0m, który resetuje nam wczesniej ustawiony graphic mode na domyslny tryb.

### DEEEMooo :D
odpalamy vima, podłączamy strace i ruszamy kursorem w prawo a potem w lewo
```sh
[crusom@crusomcarbon ~]$ sudo strace -e trace=write -p 1179165
strace: Process 1179165 attached
write(1, "\33[?25l\33[49;70H2\33[1;6H\33[?25h", 27) = 27
write(1, "\33[?25l\33[49;70H1\33[1;5H\33[?25h", 27) = 27
```

ESC[?25l - make cursor invisible
ESC[?25h - make cursor visibla
\33[1;6H - przesuwa nam kursor na 6tą kolumne
\33[1;6H - przesuwa nam kursor spowrotem na 5tą kolumne
\33[49;70H2 - przesuwa nam kursor na 70tą kolumne i 49 wiersz i wstawia 2. (vim nam pokazuje pod jakim wierszem i kolumną jest kursor)
\33[49;70H1 - jak wyżej, ale zmieniamy na 1


### uruchom polecenie «cat» i sprawdź jak zachowuje się naciśnięcie klawiszy funkcyjnych i kursora. Czemu w tym przypadku zachowanie programu «cat» jest inne niż powłoki poleceń?

Dostajemy raw characters.  
Cat próbuje wypisać treść pliku, więc nie chcemy, żeby nam wypisywanie pliku ruszało kursor i psuło nasz bufor konsoli.   
Natomiast cat elegancko obsługuje ansi escape codes.


## Zadanie 5

```
1. Wstrzymaj zadanie pierwszoplanowe «sleep 1000» i przy pomocy wbudowanego polecenia powłoki
«bg» przenieś to zadanie do wykonania w tle. Jaki sygnał został użyty do wstrzymania zadania?
2. Uruchom «find /». W trakcie jego działania naciśnij na przemian kilkukrotnie kombinację klawiszy
«CTRL+S» oraz «CTRL+Q». Czemu program zatrzymuje się i wznawia swoją pracę, skoro sterownik
terminala nie wysyłał do niego żadnych sygnałów?
3. Uruchom w powłoce «bash» polecenie «cat - &». Czemu zadanie zostało od razu wstrzymane? Jaki
sygnał otrzymało? Zakończ to zdanie wbudowanym poleceniem powłoki «kill».
4. Porównaj działanie polecenia «cat /etc/shells &» przed i po zmianie konfiguracji terminala polece-
niem «stty tostop». Jaki efekt ma włączenie flagi «tostop» na zachowanie sterownika terminala?
5. Wykonaj polecenie «stty -echoctl». Wyjaśnij co zmieniło się w konfiguracji terminala i zaprezentuj
na przykładzie programu «cat» pokaż jak zmieniło się przetwarzanie znaków sterujących.
```


1. 
--- SIGTSTP {si_signo=SIGTSTP, si_code=SI_KERNEL} ---
--- stopped by SIGTSTP ---
--- SIGCONT {si_signo=SIGCONT, si_code=SI_USER, si_pid=1182534, si_uid=1000} ---

2. 
Bo to tty wstrzymuje output. ^S to special character, tak samo ^Q

3. --- SIGTTIN {si_signo=SIGTTIN, si_code=SI_KERNEL} ---
```text
SIGTTIN      P1990      Stop    Terminal input for background process
```

<https://www.gnu.org/software/libc/manual/html_node/Job-Control-Signals.html>
```text
A process cannot read from the user’s terminal while it is running as a background job. When any process in a background job tries to read from the terminal, all of the processes in the job are sent a SIGTTIN signal. The default action for this signal is to stop the process. For more information about how this interacts with the terminal driver, see Access to the Controlling Terminal.
```

```text


When a process in a background job tries to read from its controlling terminal, the process group is usually sent a SIGTTIN signal. This normally causes all of the processes in that group to stop (unless they handle the signal and don’t stop themselves). However, if the reading process is ignoring or blocking this signal, then read fails with an EIO error instead.

Similarly, when a process in a background job tries to write to its controlling terminal, the default behavior is to send a SIGTTOU signal to the process group. However, the behavior is modified by the TOSTOP bit of the local modes flags (see Local Modes). If this bit is not set (which is the default), then writing to the controlling terminal is always permitted without sending a signal. Writing is also permitted if the SIGTTOU signal is being ignored or blocked by the writing process.
```
mozemy pozwolic procesowi w tle na output, jesli wylaczymy bit tostop w konfiguracji terminala.  
Nie mozemy natomiast pozwolic procesowi w tle na input.  

4. tostop powoduje wstrzymanie programu, gdy ten probuje cos pisac. Wysylany jest sygnal SIGTTOU  

5. 
```text

       ECHOCTL
              (not in POSIX) If ECHO is also set, terminal special
              characters other than TAB, NL, START, and STOP are echoed
              as ^X, where X is the character with ASCII code 0x40
              greater than the special character.  For example,
              character 0x08 (BS) is echoed as ^H.  [requires
              _BSD_SOURCE or _SVID_SOURCE]
```
nie są echowane spacial characters

## Zadanie 6
```text
Zadanie 6. Procedury setjmp(3) i longjmp(3) z biblioteki standardowej języka C służą do wykonywania nielokalnych skoków.  
Uproszczone odpowiedniki tych procedur znajdują się w pliku «libcsapp/Setjmp.s», a definicja «Jmpbuf» w pliku «include/csapp.h».  
Wyjaśnij co robią te procedury, a następnie przeprowadź uczestników zajęć przez ich kod.  
Dlaczego «Jmpbuf» nie przechowuje wszystkich rejestrów procesora?  
Czemu «Longjmp» zapisuje na stos wartość przed wykonaniem instrukcji «ret»
```

```asm
_JB_RBX = 0
_JB_RBP = 1
_JB_R12 = 2
_JB_R13 = 3
_JB_R14 = 4
_JB_R15 = 5
_JB_RSP = 6
_JB_RIP = 7

        .text

        .globl Setjmp
        .type Setjmp,@function
Setjmp:
    movq    (%rsp),%r11 ; rip was pushed on stack
    movq    %rbx,(_JB_RBX * 8)(%rdi)
    movq    %rbp,(_JB_RBP * 8)(%rdi)
    movq    %r12,(_JB_R12 * 8)(%rdi)
    movq    %r13,(_JB_R13 * 8)(%rdi)
    movq    %r14,(_JB_R14 * 8)(%rdi)
    movq    %r15,(_JB_R15 * 8)(%rdi)
    movq    %rsp,(_JB_RSP * 8)(%rdi)
    movq    %r11,(_JB_RIP * 8)(%rdi)
    xorl    %eax,%eax   ; setjmp returns 0, when initially called
    ret
        .size Setjmp, . - Setjmp

        .globl Longjmp
        .type Longjmp,@function
Longjmp:
    movq    (_JB_RBX * 8)(%rdi),%rbx
    movq    (_JB_RBP * 8)(%rdi),%rbp
    movq    (_JB_R12 * 8)(%rdi),%r12
    movq    (_JB_R13 * 8)(%rdi),%r13
    movq    (_JB_R14 * 8)(%rdi),%r14
    movq    (_JB_R15 * 8)(%rdi),%r15
    movq    (_JB_RSP * 8)(%rdi),%rsp
    movq    (_JB_RIP * 8)(%rdi),%r11
    movl    %esi,%eax ; %esi is val argument
    testl   %eax,%eax ; " If the programmer mistakenly passes the value 0 in val, the "fake" return will instead return 1."
    jnz 1f
    incl    %eax ; if programmer is stupid and entered val as 0, increment it
1:  movq    %r11,(%rsp) ; push %rip on stack
    ret ; pop %rip from stack to %rip
        .size Longjmp, . - Longjmp
```

### Dlaczego «Jmpbuf» nie przechowuje wszystkich rejestrów procesora?
Bo reszta rejestrow jest uzywana w System V AMD64 ABI i caller je zapisuje elegancko  

<https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf>
```text
This subsection discusses usage of each register. Registers %rbp, %rbx and %r12 through
%r15 “belong” to the calling function and the called function is required to preserve their
values. In other words, a called function must preserve these registers’ values for its
caller. Remaining registers “belong” to the called function
```


| Argument Type             | Registers            |
|---------------------------|----------------------|
| Integer/pointer arguments 1-6 | RDI, RSI, RDX, RCX, R8, R9 |
| Floating point arguments 1-8 | XMM0 - XMM7         |
| Excess arguments          | Stack                |
| Static chain pointer      | R10                  |


## Zadanie 7

```text
Zadanie 7. Uzupełnij program «game» tj. prostą grę w szybkie obliczanie sumy dwóch liczb.  
Zadaniem procedury «readnum» jest wczytać od użytkownika liczbę.  
Jeśli w międzyczasie przyjdzie sygnał, to procedura ma natychmiast wrócić podając numer sygnału, który przerwał jej działanie.  
W przeciwnym przypadku zwraca zero i przekazuje wczytaną liczbę przez pamięć pod wskaźnikiem «num_p».  
Twoja implementacja procedury «readnum» musi wczytać cały wiersz w jednym kroku.  
Należy wykorzystać procedury siglongjmp(3), sigsetjmp(3) i alarm(2).  
Pamiętaj, żeby po wczytaniu ciągu znaków zakończyć go znakiem NUL i wyłączyć czasomierz!  
Kiedy Twój program będzie zachowywać się poprawnie zamień procedury nielokalnych skoków na longjmp(3) i setjmp(3). Czemu program przestał działać? 
UWAGA! We FreeBSD i macOS zamiast «longjmp» i «setjmp» należy użyć odpowiednio «_longjmp» i «_setjmp».
```

## Zadanie 8
```text
Zadanie 8. Program «coro» wykonuje trzy współprogramy3 połączone ze sobą w potok bez użycia pipe(2).  
Pierwszy z nich czyta ze standardowego wejścia znaki, kompresuje białe znaki i zlicza słowa.  
Drugi usuwa wszystkie znaki niebędące literami. Trzeci zmienia wielkość liter i drukuje znaki na standardowe wyjście.  
W wyniku wykonania procedury «coro_yield» współprogram przekazuje niezerową liczbę do następnego współprogramu, który otrzyma tę wartość w wyniku powrotu z «coro_yield».  
Efektywnie procedura ta implementuje zmianę kontekstu.  
Taką prymitywną formę wielozadaniowości kooperacyjnej (ang. cooperative multitasking) można zaprogramować za pomocą setjmp(3) i longjmp(3).  
Zaprogramuj procedurę «coro_switch» tak, by wybierała następny współprogram do uruchomienia i przełączała na niego kontekst.  
Jeśli współprogram przekazał wartość parametru «EOF», to należy go usunąć z listy aktywnych współprogramów.  
Program używa listy dwukierunkowej «TAILQ» opisanej w queue(3).  
Zmienna «runqueue» przechowuje listę aktywnych współprogramów, «running» bieżąco wykonywany współprogram, a «dispatcher» kontekst programu, 
do którego należy wrócić, po zakończeniu wykonywania ostatniego aktywnego współprogramu.
```

## some links
<https://www.gaijin.at/en/infos/ascii-ansi-character-table>
<https://nmsl.cs.nthu.edu.tw/wp-content/uploads/2011/09/images_courses_CS5432_2016_18-termio.pdf>
<https://unix.stackexchange.com/questions/18166/what-are-session-leaders-in-ps>

