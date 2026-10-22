/* os_pico.h -- included before every sc source file on the PicoCalc:
 * the Unix bits newlib lacks. */
#ifndef OS_PICO_H
#define OS_PICO_H
#include <sys/types.h>
#include <signal.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 256
#endif
#define PATHLEN 256	/* sc keeps several on the stack */
#ifndef SIGWINCH
#define SIGWINCH 28
#endif
#ifndef SIGPIPE
#define SIGPIPE 13
#endif
#ifndef SIGCHLD
#define SIGCHLD 20
#endif
#ifndef SIGQUIT
#define SIGQUIT 3
#endif
#ifndef SIGHUP
#define SIGHUP 1
#endif
#ifndef SIGALRM
#define SIGALRM 14
#endif
#ifndef SIGTSTP
#define SIGTSTP 18
#endif
#endif
