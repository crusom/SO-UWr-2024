#define _GNU_SOURCE
#include "csapp.h"
#include <sys/stat.h>
#include <fcntl.h>
//#include <unistd.h>
//#include <sys/syscall.h>
//#include <dirent.h>

#define DIRBUFSZ 256

#define ERROR_LOG(format, ...) do { fprintf(stderr,"ERROR: " format, ##__VA_ARGS__); } while(0)
#define ERROR_LOG_AND_EXIT(format, ...) do { fprintf(stderr,"ERROR: " format, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)

static void print_mode(mode_t m) {
  char t;

  if (S_ISDIR(m))
    t = 'd';
  else if (S_ISCHR(m))
    t = 'c';
  else if (S_ISBLK(m))
    t = 'b';
  else if (S_ISREG(m))
    t = '-';
  else if (S_ISFIFO(m))
    t = 'f';
  else if (S_ISLNK(m))
    t = 'l';
  else if (S_ISSOCK(m))
    t = 's';
  else
    t = '?';

  char ur = (m & S_IRUSR) ? 'r' : '-';
  char uw = (m & S_IWUSR) ? 'w' : '-';
  char ux = (m & S_IXUSR) ? 'x' : '-';
  char gr = (m & S_IRGRP) ? 'r' : '-';
  char gw = (m & S_IWGRP) ? 'w' : '-';
  char gx = (m & S_IXGRP) ? 'x' : '-';
  char or = (m & S_IROTH) ? 'r' : '-';
  char ow = (m & S_IWOTH) ? 'w' : '-';
  char ox = (m & S_IXOTH) ? 'x' : '-';

  /* TODO: Fix code to report set-uid/set-gid/sticky bit as 'ls' does. */
  ux = (m & S_ISUID) ? ((m & S_IXUSR) ? 's' : 'S') : ux;
  ux = (m & S_ISGID) ? 's' : gx;
  ux = (m & S_ISVTX) ? 't' : ox;

  printf("%c%c%c%c%c%c%c%c%c%c", t, ur, uw, ux, gr, gw, gx, or, ow, ox);
}

static void print_uid(uid_t uid) {
  struct passwd *pw = getpwuid(uid);
  if (pw)
    printf(" %10s", pw->pw_name);
  else
    printf(" %10d", uid);
}

static void print_gid(gid_t gid) {
  struct group *gr = getgrgid(gid);
  if (gr)
    printf(" %10s", gr->gr_name);
  else
    printf(" %10d", gid);
}

static void file_info(int dirfd, const char *name) {
  struct stat sb[1];

  /* TODO: Read file metadata. */
  if(fstatat(dirfd,name,sb,AT_SYMLINK_NOFOLLOW) == -1) {
    ERROR_LOG("%s\n", strerror(errno));
    goto err;
  }

  print_mode(sb->st_mode);
  printf("%4ld", sb->st_nlink);
  print_uid(sb->st_uid);
  print_gid(sb->st_gid);

  /* TODO: For devices: print major/minor pair; for other files: size. */
  if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode)) {
    unsigned int maj = major(sb->st_dev);
    unsigned int min = minor(sb->st_dev);
    printf("%9u,%3u", maj, min);
  } 
  else {
    printf("%12lu ", sb->st_size);
  }

  char *now = ctime(&sb->st_mtime);
  now[strlen(now) - 1] = '\0';
  printf("%26s", now);


  if (S_ISLNK(sb->st_mode)) {
  /* TODO: Read where symlink points to and print '-> destination' string. */
    char path[PATH_MAX];
    ssize_t nbytes;
    if ((nbytes = readlinkat(dirfd,name,path,sizeof(path)-1)) == -1) {
      ERROR_LOG("%s\n", strerror(errno));
      goto err;
    }
    printf("  %s -> %.*s", name, (int) nbytes, path);
  }
  else
    printf("  %s", name);
err:
  putchar('\n');
}

int main(int argc, char *argv[]) {
  if (!argv[1])
    argv[1] = ".";

  int dirfd = Open(argv[1], O_RDONLY | O_DIRECTORY, 0);
  char buf[DIRBUFSZ];
  int n;
  while ((n = Getdents(dirfd, (void *)buf, DIRBUFSZ))) {
    struct linux_dirent *d;
    /* TODO: Iterate over directory entries and call file_info on them. */
    for (size_t bpos = 0; bpos < n;) {
      d = (struct linux_dirent *) (buf + bpos);
      file_info(dirfd, d->d_name);
      bpos += d->d_reclen;
    }
  }

  Close(dirfd);
  return EXIT_SUCCESS;
}
