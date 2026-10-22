/* port.c -- PicoCalc-port helpers used by sc's own code (host and device).
 *
 * Running out of memory: sc's fatal() offers to save the sheet before it
 * stops, but on the PicoCalc opening the file needs memory too.  A block
 * kept back at startup is freed first so the save can work, and the
 * screen then says what happened before sc exits. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curses.h"

#define RESERVE (24 * 1024)
static void *reserve;

void port_init(void) { if (!reserve) reserve = malloc(RESERVE); }

void port_release_reserve(void) { free(reserve); reserve = NULL; }

/* after sc's die-save: say what happened and wait for a key */
void port_fatal_note(const char *why, const char *savename)
{
	char line[128];
	clear();
	move(2, 0);
	addstr("sc has to stop: ");
	addstr(why);
	move(4, 0);
	snprintf(line, sizeof line, "If you answered y, the sheet was saved as %s in %s:",
	    savename, getenv("HOME") ? getenv("HOME") : ".");
	addstr(line);
	move(5, 0);
	addstr("open it with G.");
	move(7, 0);
	if (strstr(why, "memory"))
		addstr("This sheet is too big for the PicoCalc's memory.");
	move(9, 0);
	addstr("Press any key.");
	refresh();
	(void)getch();
}
