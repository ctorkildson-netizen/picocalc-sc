/* curses.h -- the small part of curses that sc uses, for the PicoCalc.
 *
 * One window (stdscr), a grid of cells with an attribute and a colour
 * pair, redrawn through a backend (port/be_*.c) on refresh().  Only the
 * cells that changed since the last refresh are sent to the backend.
 */
#ifndef PC_CURSES_H
#define PC_CURSES_H

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include "be.h"

typedef unsigned int chtype;
typedef unsigned int attr_t;

typedef struct pc_window {
	int cury, curx;
	attr_t attr;		/* A_STANDOUT etc., no colour bits */
	short pair;		/* current colour pair */
	attr_t bkgd;
	bool scroll, clear;
} WINDOW;

extern WINDOW *stdscr;
extern int LINES, COLS;

#define OK	0
#define ERR	(-1)

#define A_NORMAL	0u
#define A_CHARTEXT	0x000000ffu
#define A_STANDOUT	BE_STANDOUT
#define A_REVERSE	A_STANDOUT
#define A_BOLD		BE_BOLD
#define A_UNDERLINE	BE_UNDERLINE
#define A_COLOR		0x0f000000u
#define A_ATTRIBUTES	0xffffff00u
#define COLOR_PAIR(n)	((attr_t)((n) & 15) << 24)
#define PAIR_NUMBER(a)	((int)(((a) & A_COLOR) >> 24))

#define COLOR_BLACK	0
#define COLOR_RED	1
#define COLOR_GREEN	2
#define COLOR_YELLOW	3
#define COLOR_BLUE	4
#define COLOR_MAGENTA	5
#define COLOR_CYAN	6
#define COLOR_WHITE	7
#define COLORS		8
#define COLOR_PAIRS	16

/* key codes, as in ncurses */
#define KEY_MIN		0401
#define KEY_DOWN	BE_KEY_DOWN
#define KEY_UP	BE_KEY_UP
#define KEY_LEFT	BE_KEY_LEFT
#define KEY_RIGHT	BE_KEY_RIGHT
#define KEY_HOME	BE_KEY_HOME
#define KEY_BACKSPACE	BE_KEY_BACKSPACE
#define KEY_F0	BE_KEY_F0
#define KEY_F(n)	(KEY_F0 + (n))
#define KEY_DC	BE_KEY_DC
#define KEY_IC	BE_KEY_IC
#define KEY_NPAGE	BE_KEY_NPAGE
#define KEY_PPAGE	BE_KEY_PPAGE
#define KEY_ENTER	0527
#define KEY_A1		0534
#define KEY_A3		0535
#define KEY_B2		0536
#define KEY_C1		0537
#define KEY_C3		0540
#define KEY_END	BE_KEY_END
#define KEY_FIND	0552
#define KEY_HELP	0553
#define KEY_SELECT	0601
#define KEY_RESIZE	0632
#define KEY_MAX		0777

WINDOW *initscr(void);
int endwin(void);
int cbreak(void);
int noecho(void);
int nonl(void);
int keypad(WINDOW *, bool);
int scrollok(WINDOW *, bool);
int clearok(WINDOW *, bool);
int reset_prog_mode(void);
int reset_shell_mode(void);
int start_color(void);
bool has_colors(void);
int init_pair(short, short, short);
int wbkgd(WINDOW *, chtype);
void wbkgdset(WINDOW *, chtype);
int wattrset(WINDOW *, int);
int wattr_on(WINDOW *, attr_t, void *);
int wattr_off(WINDOW *, attr_t, void *);
int wattr_get(WINDOW *, attr_t *, short *, void *);
int wcolor_set(WINDOW *, short, void *);
int wmove(WINDOW *, int, int);
int waddch(WINDOW *, chtype);
int waddnstr(WINDOW *, const char *, int);
int wclear(WINDOW *);
int werase(WINDOW *);
int wclrtobot(WINDOW *);
int wclrtoeol(WINDOW *);
int wrefresh(WINDOW *);
chtype winch(WINDOW *);
int wgetch(WINDOW *);
int ungetch(int);
const char *keyname(int);
int printw(const char *, ...);
int beep(void);
int getcurx(WINDOW *);
int getcury(WINDOW *);
int getmaxx(WINDOW *);
int getmaxy(WINDOW *);

#define getyx(w, y, x)	((y) = getcury(w), (x) = getcurx(w))
#define getmaxyx(w, y, x) ((y) = getmaxy(w), (x) = getmaxx(w))

#define move(y, x)		wmove(stdscr, (y), (x))
#define addch(c)		waddch(stdscr, (c))
#define addstr(s)		waddnstr(stdscr, (s), -1)
#define addnstr(s, n)		waddnstr(stdscr, (s), (n))
#define mvaddstr(y, x, s)	(wmove(stdscr, (y), (x)), addstr(s))
#define mvaddch(y, x, c)	(wmove(stdscr, (y), (x)), addch(c))
#define refresh()		wrefresh(stdscr)
#define clear()			wclear(stdscr)
#define erase()			werase(stdscr)
#define clrtoeol()		wclrtoeol(stdscr)
#define clrtobot()		wclrtobot(stdscr)
#define standout()		wattr_on(stdscr, A_STANDOUT, 0)
#define standend()		wattrset(stdscr, A_NORMAL)
#define attron(a)		wattr_on(stdscr, (a), 0)
#define attroff(a)		wattr_off(stdscr, (a), 0)
#define attrset(a)		wattrset(stdscr, (a))
#define color_set(p, o)		wcolor_set(stdscr, (p), (o))
#define getch()			wgetch(stdscr)
#define inch()			winch(stdscr)
#define bkgd(c)			wbkgd(stdscr, (c))
#define bkgdset(c)		wbkgdset(stdscr, (c))
#define attr_get(a, p, o)	wattr_get(stdscr, (a), (p), (o))
#define nl()			OK
#define echo()			OK
#define nocbreak()		OK

#endif
