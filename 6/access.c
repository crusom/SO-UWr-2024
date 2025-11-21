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
}
int main(){}
