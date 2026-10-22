/* be_host.c -- a scripted test backend for the curses shim.
 *
 * Keys come from the environment variable SCKEYS:
 *   \U \D \L \R arrows   \E Esc   \n Enter   \b Backspace   \t Tab
 *   \P snapshot (print the screen now)   \\ a backslash
 *   \cX Ctrl+X
 * When the keys run out the screen is printed and the program exits.
 * Standout cells print in [brackets] only in the snapshot's attribute line
 * when SCATTR=1 is set.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curses.h"

#define R 32
#define C 64

static unsigned char grid[R][C];
static attr_t ga[R][C];
static int crow, ccol;
static const char *keys;

void be_init(int *rows, int *cols)
{
	const char *s = getenv("SCROWS");
	*rows = s ? atoi(s) : R;
	s = getenv("SCCOLS");
	*cols = s ? atoi(s) : C;
	memset(grid, ' ', sizeof grid);
	keys = getenv("SCKEYS");
	if (!keys) keys = "";
}

void be_end(void) {}

void be_put(int row, int col, int ch, unsigned int attr, int fg, int bg)
{
	(void)fg; (void)bg;
	if (row < R && col < C) { grid[row][col] = ch; ga[row][col] = attr; }
}

void be_cursor(int row, int col) { crow = row; ccol = col; }
void be_flush(void) {}
void be_beep(void) {}

static void snapshot(void)
{
	int rows = LINES < R ? LINES : R, cols = COLS < C ? COLS : C;
	bool at = getenv("SCATTR") != NULL;
	printf("+");
	for (int c = 0; c < cols; c++) putchar('-');
	printf("+ cursor %d,%d\n", crow, ccol);
	for (int r = 0; r < rows; r++) {
		putchar('|');
		for (int c = 0; c < cols; c++) putchar(grid[r][c]);
		putchar('|');
		if (at) {
			putchar(' ');
			for (int c = 0; c < cols; c++)
				putchar(ga[r][c] & A_STANDOUT ? '#' : '.');
		}
		putchar('\n');
	}
	printf("+");
	for (int c = 0; c < cols; c++) putchar('-');
	printf("+\n");
	fflush(stdout);
}

int be_getkey(void)
{
	for (;;) {
		if (!*keys) { snapshot(); exit(0); }
		int c = (unsigned char)*keys++;
		if (c != '\\') return c;
		c = *keys++;
		switch (c) {
		case 'U': return KEY_UP;
		case 'D': return KEY_DOWN;
		case 'L': return KEY_LEFT;
		case 'R': return KEY_RIGHT;
		case 'E': return 27;
		case 'n': return '\r';
		case 'b': return KEY_BACKSPACE;
		case 't': return '\t';
		case 'c': return *keys++ & 0x1f;
		case 'P': snapshot(); continue;
		case 0: keys--; continue;
		default: return c;
		}
	}
}

/* sc decides between screen and pipe mode with isatty(); the scripted
 * backend is always a screen, as the PicoCalc is. */
int isatty(int fd) { (void)fd; return 1; }
