## Zadanie 1
```text
Zadanie 1. W każdym z poniższych przypadków zakładamy, że początkowa tożsamość naszego procesu  
to: ruid=1000, euid=0, suid=0. Jak zmieni się tożsamość procesu po wywołaniu następujących funkcji:  
(a) setuid(2000), (b) setreuid(-1, 2000), (c) seteuid(2000), (d) setresuid(-1, 2000, 3000).  
Odpowiedź uzasadnij posługując się podręcznikami systemowymi setuid(2), setreuid(2), setresuid(2).  
Czy proces z tożsamością ruid=0, euid=1000, suid=1000 jest uprzywilejowany? Odpowiedź uzasadnij  
```

a) poniewaz euid to root, to moze ustawic swoje uid na jakikolwiek proces, wiec syscall sie powiedzie  
b) ruid zostaje taki sam, a euid zmieni sie na 2000  
c) euid zostanie ustawiony na 2000  
d) reuid zostanie ten sam (1000), euid na 2000, saved na 3000

### Czy proces z tożsamością ruid=0, euid=1000, suid=1000 jest uprzywilejowany? Odpowiedź uzasadnij  
W trakcie sprawdzania uprawnień brany jest pod uwagę tylko effective id.  
Proces moze ustawic swoj euid na ruid i stac sie rootem, ale aktualnie jest uzytkownikiem.

## Zadanie 2
```text
Zadanie 2. Jaką rolę pełnią bity uprawnień «rwx» dla katalogów w systemach uniksowych? Opisz znaczenie bitów «set-gid» i «sticky» dla katalogów.  
Napisz w pseudokodzie i zreferuj procedurę «bool my_access(struct stat *sb, int mode)».  
Pierwszy i drugi argument opisano odpowiednio w stat(2) i access(2).  
Dla procesu o tożsamości zadanej przez getuid(2) i getgroups(2) procedura «my_access» sprawdza czy proces ma upoważniony dostęp «mode» do pliku o metadanych wczytanych do «sb».  
Wskazówka: Rozważ uprawnienia katalogu «/usr/local» i «/tmp».
```

### Jaką rolę pełnią bity uprawnień «rwx» dla katalogów w systemach uniksowych? Opisz znaczenie bitów «set-gid» i «sticky» dla katalogów.
R - mozna czytac entries (getdirents)  
W - mozna modyfikowac zawartosc katalogu (creat, unlink, rename)  
X - mozna czytac metadane pliku i ich zawartosc (stat)  
sticky - plik moze usunac tylko jego wlasciciel (a nie czlonek grupy)  

setgid -  
```text
Setting the setgid permission on a directory causes files and subdirectories created within to inherit its group ownership,  
rather than the primary group of the file-creating process. Created subdirectories also inherit the setgid bit.  
The policy is only applied during creation and, thus, only prospectively.  
Directories and files existing when the setgid bit is applied are unaffected, as are directories and files moved into the directory on which the bit is set. 
```

#### Napisz w pseudokodzie i zreferuj procedurę «bool my_access(struct stat *sb, int mode)».  
```c
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
/*
  The check is done using the calling process's real UID and GID, rather than the effective IDs as is done when actually attempting an operation (e.g., open(2)) on the file.
  Similarly, for the root user, the check uses the set of permitted capabilities rather than the set of effective capabilities;
  and for non-root users, the check uses an empty set of capabilities.

  This allows set-user-ID programs and capability-endowed programs to easily determine the invoking user's authority.
  In other words, access() does not answer the "can I read/write/execute  this  file?"  question.
  It answers a slightly different question: "(assuming I'm a setuid binary) can the user who invoked me read/write/execute this file?",
  which gives setuser-ID programs the possibility to prevent malicious users from causing them to read files which users shouldn't be able to read.
*/

// #define R_OK  4   /* Test for read permission.  */
// #define W_OK  2   /* Test for write permission.  */
// #define X_OK  1   /* Test for execute permission.  */
// #define F_OK  0   /* Test for existence.  */

bool my_access(struct stat *sb, int mode) {
  int file_mode = sb->st_mode;
  uid_t ruid = getuid();
  // we're the owner.
  if (sb->st_uid == ruid) {
    // Now check if the requested mode is ok
    int file_r  = (file_mode & S_IRUSR);
    int file_w  = (file_mode & S_IWUSR);
    int file_x  = (file_mode & S_IXUSR);

    if ((mode & R_OK) && !file_r) return false;
    if ((mode & W_OK) && !file_w) return false;
    if ((mode & X_OK) && !file_x) return false;
    // if we got to this point i guess we're fine
    return true;
  }
  // second case, we may be in supplementary group, which owns the file.
  // (code from getgroups(3) example)
  gid_t rgid = getgid();

  long ngroups_max = sysconf(_SC_NGROUPS_MAX) + 1;
  gid_t *group = (gid_t *)malloc(ngroups_max *sizeof(gid_t));

  int ngroups = getgroups(ngroups_max, group);
  for (int i = 0; i < ngroups; i++) {
    if (rgid == group[i])
      return true;
  }
  return false; // nope
```


## Zadanie 3
```text
Zadanie 3. Właścicielem pliku programu su(1) jest «root», a plik ma ustawiony bit «set-uid».  
Jaką tożsamość będzie miał na początku proces wykonujący «su», jeśli przed execve(2) było euid=1000?  
Zreferuj działanie uproszczonej wersji programu su1 zakładając, że wszystkie wywołania systemowe kończą  
się bez błędów, a użytkownik zdołał się uwierzytelnić. Skoncentruj się na funkcjach czytających bazę danych  
użytkowników, odczytujących i sprawdzających hasło, oraz zmieniających tożsamość procesu.
```

### Jaką tożsamość będzie miał na początku proces wykonujący «su», jeśli przed execve(2) było euid=1000?  
Proces otrzyma euid roota.


### Zreferuj działanie uproszczonej wersji programu su1 zakładając, że wszystkie wywołania systemowe kończą się bez błędów, a użytkownik zdołał się uwierzytelnić. Skoncentruj się na funkcjach czytających bazę danych  użytkowników, odczytujących i sprawdzających hasło, oraz zmieniających tożsamość procesu.

```c
int
main(int argc, char *argv[])
{
	char *usr, *pass;
	char *shell, *envshell, *term;
	struct passwd *pw;
	char *newargv[3];
	uid_t uid;

	ARGBEGIN {
	case 'l':
		lflag = 1;
		break;
	case 'p':
		pflag = 1;
		break;
	default:
		usage();
	} ARGEND;

	if (argc > 1)
		usage();
	usr = argc > 0 ? argv[0] : "root";

	errno = 0;
	// The  getpwnam()  function returns a pointer to a structure containing the broken-out fields of 
    // the record in the password database (e.g., the local password file /etc/passwd, NIS, and LDAP) that matches the username name.
    
    // struct passwd {
    //   char   *pw_name;       /* username */
    //   char   *pw_passwd;     /* user password */
    //   uid_t   pw_uid;        /* user ID */
    //   gid_t   pw_gid;        /* group ID */
    //   char   *pw_gecos;      /* user information */
    //   char   *pw_dir;        /* home directory */
    //   char   *pw_shell;      /* shell program */
    // };

    pw = getpwnam(usr);
	if (!pw) {
		if (errno)
			eprintf("getpwnam: %s:", usr);
		else
			eprintf("who are you?\n");
	}
    // check the real user id (the process that called execve).
    // If it's root then there's no need for authentication
	uid = getuid();
	if (uid) {
		pass = getpass("Password: ");
		if (!pass)
			eprintf("getpass:");
		if (pw_check(pw, pass) <= 0)
			exit(1);
	}

    // The  initgroups()  function  initializes  the group access list by reading the group database /etc/group and using all groups of which user is a member
	if (initgroups(usr, pw->pw_gid) < 0)
		eprintf("initgroups:");
    // set our gid to the user we want to be
	if (setgid(pw->pw_gid) < 0)
		eprintf("setgid:");
    // set our uid to the user we want to be
	if (setuid(pw->pw_uid) < 0)
		eprintf("setuid:");

	shell = pw->pw_shell[0] == '\0' ? "/bin/sh" : pw->pw_shell;
    //  Start the shell as a login shell with an environment similar to a real login.
	if (lflag) {
		term = getenv("TERM");
		clearenv();
		setenv("HOME", pw->pw_dir, 1);
		setenv("SHELL", shell, 1);
		setenv("USER", pw->pw_name, 1);
		setenv("LOGNAME", pw->pw_name, 1);
		setenv("TERM", term ? term : "linux", 1);
		if (chdir(pw->pw_dir) < 0)
			eprintf("chdir %s:", pw->pw_dir);
		newargv[0] = shell;
        // Make bash act as if it had been invoked as a login shell (see INVOCATION below).
		newargv[1] = "-l";
		newargv[2] = NULL;
	} else {
        // Create a pseudo-terminal for the session.
		if (pflag) {
			envshell = getenv("SHELL");
			if (envshell && envshell[0] != '\0')
				shell = envshell;
		} else {
			setenv("HOME", pw->pw_dir, 1);
			setenv("SHELL", shell, 1);
			if (strcmp(pw->pw_name, "root") != 0) {
				setenv("USER", pw->pw_name, 1);
				setenv("LOGNAME", pw->pw_name, 1);
			}
		}
		newargv[0] = shell;
		newargv[1] = NULL;
	}
	execve(shell, newargv, environ);
	weprintf("execve %s:", shell);
	return (errno == ENOENT) ? 127 : 126;
}
```

## Zadanie 4
```text
Zadanie 4. Na podstawie §38.2 i §38.3 wyjaśnij czemu programy uprzywilejowane należy projektować w taki
sposób, by operowały z najmniejszym możliwym zestawem upoważnień (ang. the least privilege). 
Zreferuj wytyczne dotyczące projektowania takich programów. Zapoznaj się z §39.1 i wytłumacz czemu standardowy
zestaw funkcji systemu uniksowego do implementacji programów uprzywilejowanych jest niewystarczający.
Jak starają się to naprawić zdolności (ang. capabilities)? Dla nieuprzywilejowanego procesu posiadającego
zdolności «CAP_DAC_READ_SEARCH» i «CAP_KILL» jądro pomija sprawdzanie upoważnień do wykonywania
pewnych akcji – wymień je. Kiedy proces użytkownika może wysłać sygnał do innego procesu
```

### Na podstawie §38.2 i §38.3 wyjaśnij czemu programy uprzywilejowane należy projektować w taki sposób, by operowały z najmniejszym możliwym zestawem upoważnień (ang. the least privilege). 
Jesli nasz system zostalby przejety przez atakujacego, to niechcielibysmy zeby byl on w stanie wyrzadzic szkody.  
Na przyklad, jesli nasz web server ma podatnosc i ktos ja z sukcesem zeksploituje, to oczekujemy ze sam server bedzie uruchomiony z jak najmniejszymi uprawnieniami, przez co atakujacy nie wyrzadzi szkod.  

Powinnismy wychodzic z trybu uprzywilejowanego, gdy wykonamy potrzebne akcje.
Jesli tryb uprzywilejowany nie bedzie nam wiecej potrzebny, to powinnismy sie go calkowicie pozbyc.
W przypadku roota wystarczy uzyc setuid.  
Setuid jest jednak dosc quirky, bo jesli nie wywoluje go root to 
```text
If  the  process does not have appropriate privileges, but uid is equal to the real user ID or the saved set-user-ID, 
setuid() shall set the effective user ID to uid; the real user ID and saved set-user-ID shall remain unchanged.
``` 
Mozna uzyc setreuid, jak czytamy:
```text
If  the  real  user ID is being set (ruid is not -1), or the effective user ID is being set to 
a value not equal to the real user ID, then the saved set-user-ID of the current process shall be set equal to the new effective user ID.
```

Przed wywolaniem execa wystarczy, ze ustawimy effective uid i real uid, bo 
```text
   The effective user ID of the process is copied to the saved set-
   user-ID; similarly, the effective group ID is copied to the saved
   set-group-ID.  This copying takes place after any effective ID
   changes that occur because of the set-user-ID and set-group-ID
   mode bits.

   The process's real UID and real GID, as well as its supplementary
   group IDs, are unchanged by a call to execve().
```

Powinnismy tez pamietac zeby zamykac file desciptory, bo pozostaja otwarte, nawet po dropnieciu uprawnien.

### Zapoznaj się z §39.1 i wytłumacz czemu standardowy zestaw funkcji systemu uniksowego do implementacji programów uprzywilejowanych jest niewystarczający. Jak starają się to naprawić zdolności (ang. capabilities)? 
Podzial na procesy uprzywielejowane i nie, sprawiaja ze proces uprzywilejowany moze wszystko.  
Czesto jednak chcemy, zeby proces mogl wykonac tylko jedna uprzywilejowana akcje, na przyklad ping, aby wysylal i odbieral pakiety, ale niekoniecznie bypassowal checki odczytu plikow.  
Capabilities pozwalaja nam na bardziej granualne zarzadzanie akcjami uprzywilejowanymi.


### Dla nieuprzywilejowanego procesu posiadającego zdolności «CAP_DAC_READ_SEARCH» i «CAP_KILL» jądro pomija sprawdzanie upoważnień do wykonywania pewnych akcji – wymień je. 
Quick man page lookup
```text
CAP_DAC_READ_SEARCH
      •  Bypass file read permission checks and directory read and execute permission checks;
      •  invoke open_by_handle_at(2);
      •  use the linkat(2) AT_EMPTY_PATH flag to create a link to a file referred to by a file descriptor.
```

```text
CAP_KILL
      Bypass permission checks for sending signals (see kill(2)).  This includes use of the ioctl(2) KDSIGACCEPT operation.
```


### Kiedy proces użytkownika może wysłać sygnał do innego procesu?
Jesli ruid lub euid procesu wysylajacego jest rowne reuid/suid procesu do ktorego wysylamy.
Zrodlo slajd 9ty wykladu 6b.

## Zadanie 5
```text
Zadanie 5. Jakie zadania pełni procedura exit(3) z biblioteki standardowej?  
Opisz problemy z buforowaniem plików, które mogą wystąpić dla strumieni biblioteki stdio(3) w przypadku użycia wywołań fork(2), execve(2) i _exit(2).  
Jak zapobiec tym problemom? Jaka jest domyślna strategia buforowania strumienia związanego z (a) plikiem terminala (b) plikiem zwykłym (c) standardowym wyjściem błędów «stderr».  
Piszesz program który używa biblioteki «stdio». Działanie programu da się przerwać sygnałem «SIGINT».  
Ma on wtedy opróżnić wszystkie bufory otwartych strumieni i dopiero wtedy wyjść.  
Zaproponuj rozwiązanie pamiętając, że w procedurach obsługi sygnału nie wolno korzystać z funkcji, które nie są wielobieżne.
```

### Jakie zadania pełni procedura exit(3) z biblioteki standardowej?  
Jak mowi exit(3):
```text
All  functions  registered  with  atexit(3)  and  on_exit(3)  are  called,  in the reverse order of their registration.

All open stdio(3) streams are flushed and closed.  Files created by tmpfile(3) are removed.
```

Destruktory sa tez wywolywane.
Mozemy to latwo przetestowac.
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
__attribute__((destructor)) void asdf() {
  printf("asdf\n");
}
int main () { _exit(1);}
```
Destuktor nie jest wywolywany. Jesli jednak wywolamy zamiast \_exit, exit z stdlib, to wszystko dziala elegancko.


### Opisz problemy z buforowaniem plików, które mogą wystąpić dla strumieni biblioteki stdio(3) w przypadku użycia wywołań fork(2), execve(2) i \_exit(2).  
Musimy pamietac, aby zflushowac dane przed wywolaniem forka, poniewaz nowy proces dostanie kopie bufora.

Execve podmienia nasza przestrzen, ale pozostaja otwarte deskryptory.  
Do tego musimy pamietac tez o flushowaniu buforow, bo utracimy nasze dane.  

\_exit() to syscall, wiec po nasz program chamsko po prostu zakoncza dzialanie, bez zadnego flushowania buforow.

### Jak zapobiec tym problemom? Jaka jest domyślna strategia buforowania strumienia związanego z (a) plikiem terminala (b) plikiem zwykłym (c) standardowym wyjściem błędów «stderr».  
a) bufor jest flushowany, po napotkaniu znaku nowej linii "\n"
b) bufor jest flushowany, po zapelnieniu go calego
c) brak buforowania

Problemom mozna zapobiec pamietajac o flushowaniu, lub ustawiajac odpowiedni sposob buforowania za pomoca setvbuf.

### Piszesz program który używa biblioteki «stdio». Działanie programu da się przerwać sygnałem «SIGINT». Ma on wtedy opróżnić wszystkie bufory otwartych strumieni i dopiero wtedy wyjść.  Zaproponuj rozwiązanie pamiętając, że w procedurach obsługi sygnału nie wolno korzystać z funkcji, które nie są wielobieżne.
ummmmmmmmmmmmm
```c
void handle_sigint(int sig) {
    write(1, stdout->_IO_buf_base, stdout->_IO_buf_end - stdout->_IO_buf_base);
    _exit(0);
}
```
pobite gary

## Zadanie 6
```text
Zadanie 6 (2). Program «writeperf» służy do testowania wydajności operacji zapisu do pliku. Nasz
microbenchmark2 wczytuje z linii poleceń opcje i argumenty opisane dalej. Na standardowe wyjście drukuje
t trójkątów (opcja «-t») prostokątnych o boku złożonym z l znaków gwiazdki «*» (opcja «-l»). Jeśli
standardowe wyjście zostało przekierowane do pliku oraz została podana opcja «-s», to przed zakończeniem
programu bufory pliku zostaną zsynchronizowane z dyskiem wywołaniem fsync(2).
Program realizuje pięć wariantów zapisu do pliku:
• Każdą linię trójkąta zapisuje osobno wywołaniem write(2) (argument «write»).
• Używa strumienia biblioteki stdio bez buforowania (argument «fwrite»), z buforowaniem liniami
(argument «fwrite-line») i buforowaniem pełnym (argument «fwrite-full»).
• Wykorzystuje wywołanie systemowe writev(2) do zapisania do «IOV_MAX» linii na raz.
Twoim zadaniem jest odpowiednie skonfigurowanie bufora strumienia «stdout» z użyciem procedury
setvbuf(3) oraz zaimplementowanie metody zapisu z użyciem «writev».
Przy pomocy skryptu powłoki «writeperf.sh» porównaj wydajność wymienionych wcześniej metod zapisu.
Uzasadnij przedstawione wyniki. Miej na uwadze liczbę wywołań systemowych (należy to zbadać posługując
się narzędziem strace(1) z opcją «-c») oraz liczbę kopii danych wykonanych celem przesłania zawartości
linii do buforów dysku.
```


