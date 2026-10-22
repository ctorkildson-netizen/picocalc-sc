/* os_pico.c -- the Unix calls sc makes that the PicoCalc can't do.
 *
 * There are no processes, so pipes, shell escapes, external functions
 * and plugins fail cleanly; sc reports the error and carries on.  File
 * names without a path are taken to be in /sc (SC_DIR).
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <pwd.h>
#include "sc_pico.h"

int isatty(int fd) { (void)fd; return 1; }	/* the PicoCalc is a screen */

int pipe(int fd[2]) { (void)fd; errno = ENOSYS; return -1; }
int dup(int fd) { (void)fd; errno = ENOSYS; return -1; }
int dup2(int a, int b) { (void)a; (void)b; errno = ENOSYS; return -1; }
int execl(const char *p, const char *a, ...) { (void)p; (void)a; errno = ENOSYS; return -1; }
int _execve(const char *p, char *const a[], char *const e[]) { (void)p; (void)a; (void)e; errno = ENOSYS; return -1; }
int _fork(void) { errno = ENOSYS; return -1; }
int _wait(int *s) { (void)s; errno = ECHILD; return -1; }
FILE *popen(const char *c, const char *m) { (void)c; (void)m; errno = ENOSYS; return NULL; }
int pclose(FILE *f) { (void)f; return -1; }
struct passwd *getpwnam(const char *n) { (void)n; return NULL; }
mode_t umask(mode_t m) { (void)m; return 0; }
int chown(const char *p, uid_t u, gid_t g) { (void)p; (void)u; (void)g; return 0; }
int utime(const char *p, const struct utimbuf *t) { (void)p; (void)t; return 0; }
long pathconf(const char *p, int n) { (void)p; (void)n; return 255; }

char *getcwd(char *buf, size_t n)
{
	if (!buf || n < sizeof SC_DIR) { errno = ERANGE; return NULL; }
	strcpy(buf, SC_DIR);
	return buf;
}

/* "name" -> "/sc/name"; "/x/name" and "./name" stay as they are
   ("./" is dropped) */
static const char *fix(const char *path, char *buf, size_t n)
{
	if (!path || path[0] == '/') return path;
	if (path[0] == '.' && path[1] == '/') path += 2;
	snprintf(buf, n, "%s/%s", SC_DIR, path);
	return buf;
}

int __real__open(const char *, int, int);
int __wrap__open(const char *path, int flags, int mode)
{
	char b[300];
	return __real__open(fix(path, b, sizeof b), flags, mode);
}

int __real__stat(const char *, struct stat *);
int __wrap__stat(const char *path, struct stat *st)
{
	char b[300];
	return __real__stat(fix(path, b, sizeof b), st);
}

int __real__unlink(const char *);
int __wrap__unlink(const char *path)
{
	char b[300];
	return __real__unlink(fix(path, b, sizeof b));
}

int __real_rename(const char *, const char *);
int __wrap_rename(const char *from, const char *to)
{
	char a[300], b[300];
	return __real_rename(fix(from, a, sizeof a), fix(to, b, sizeof b));
}
