/* curses.c -- see curses.h. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curses.h"

#define MAXR 32
#define MAXC 64

typedef struct { unsigned char ch; attr_t a; } cell;	/* a: attr | pair */

static WINDOW win;
WINDOW *stdscr = &win;
int LINES, COLS;

static cell scr[MAXR][MAXC];	/* what the program has drawn */
static cell shown[MAXR][MAXC];	/* what the backend shows */
static short pfg[COLOR_PAIRS], pbg[COLOR_PAIRS];
static int pushed[8], npushed;
static bool started;

static void blank_from(int r, int c)
{
	for (; c < COLS; c++) {
		scr[r][c].ch = ' ';
		scr[r][c].a = stdscr->bkgd;
	}
}

WINDOW *initscr(void)
{
	if (!started) {
		be_init(&LINES, &COLS);
		if (LINES > MAXR) LINES = MAXR;
		if (COLS > MAXC) COLS = MAXC;
		for (int i = 0; i < COLOR_PAIRS; i++) {
			pfg[i] = COLOR_WHITE;
			pbg[i] = COLOR_BLACK;
		}
		started = true;
	}
	win.cury = win.curx = 0;
	win.attr = 0;
	win.pair = 0;
	for (int r = 0; r < LINES; r++) blank_from(r, 0);
	win.clear = true;
	return stdscr;
}

int endwin(void) { be_end(); return OK; }
int cbreak(void) { return OK; }
int noecho(void) { return OK; }
int nonl(void) { return OK; }
int keypad(WINDOW *w, bool b) { (void)w; (void)b; return OK; }
int scrollok(WINDOW *w, bool b) { w->scroll = b; return OK; }
int clearok(WINDOW *w, bool b) { w->clear = b; return OK; }
int reset_prog_mode(void) { return OK; }
int reset_shell_mode(void) { return OK; }
int start_color(void) { return OK; }
bool has_colors(void) { return true; }
int beep(void) { be_beep(); return OK; }

int init_pair(short p, short fg, short bg)
{
	if (p < 0 || p >= COLOR_PAIRS) return ERR;
	pfg[p] = fg;
	pbg[p] = bg;
	return OK;
}

int wbkgd(WINDOW *w, chtype c) { w->bkgd = c & A_ATTRIBUTES; return OK; }
void wbkgdset(WINDOW *w, chtype c) { w->bkgd = c & A_ATTRIBUTES; }

int wattrset(WINDOW *w, int a)
{
	w->attr = (attr_t)a & ~(A_COLOR | A_CHARTEXT);
	if ((attr_t)a & A_COLOR) w->pair = PAIR_NUMBER((attr_t)a);
	return OK;
}
int wattr_on(WINDOW *w, attr_t a, void *o)
{
	(void)o;
	w->attr |= a & ~(A_COLOR | A_CHARTEXT);
	if (a & A_COLOR) w->pair = PAIR_NUMBER(a);
	return OK;
}
int wattr_off(WINDOW *w, attr_t a, void *o)
{
	(void)o;
	w->attr &= ~a;
	return OK;
}
int wattr_get(WINDOW *w, attr_t *a, short *p, void *o)
{
	(void)o;
	if (a) *a = w->attr | COLOR_PAIR(w->pair);
	if (p) *p = w->pair;
	return OK;
}
int wcolor_set(WINDOW *w, short p, void *o)
{
	(void)o;
	if (p < 0 || p >= COLOR_PAIRS) return ERR;
	w->pair = p;
	return OK;
}

int wmove(WINDOW *w, int y, int x)
{
	if (y < 0 || y >= LINES || x < 0 || x >= COLS) return ERR;
	w->cury = y;
	w->curx = x;
	return OK;
}

static void scroll_up(void)
{
	memmove(scr[0], scr[1], sizeof(scr[0]) * (LINES - 1));
	blank_from(LINES - 1, 0);
}

int waddch(WINDOW *w, chtype c)
{
	int ch = c & A_CHARTEXT;
	attr_t a = (c & A_ATTRIBUTES & ~A_COLOR) | w->attr;
	int pair = (c & A_COLOR) ? PAIR_NUMBER(c) : w->pair;

	if (ch == '\n') {
		blank_from(w->cury, w->curx);
		w->curx = 0;
		if (w->cury < LINES - 1) w->cury++;
		else if (w->scroll) scroll_up();
		return OK;
	}
	if (ch == '\r') { w->curx = 0; return OK; }
	if (ch == '\b') { if (w->curx > 0) w->curx--; return OK; }
	if (ch == '\t') {
		do waddch(w, ' ' | (c & A_ATTRIBUTES)); while (w->curx & 7);
		return OK;
	}
	if (ch < ' ' || ch == 0x7f) {	/* control characters as ^X */
		waddch(w, '^' | (c & A_ATTRIBUTES));
		return waddch(w, (ch == 0x7f ? '?' : ch + '@') | (c & A_ATTRIBUTES));
	}
	scr[w->cury][w->curx].ch = (unsigned char)ch;
	scr[w->cury][w->curx].a = a | COLOR_PAIR(pair);
	if (++w->curx >= COLS) {
		w->curx = 0;
		if (w->cury < LINES - 1) w->cury++;
		else if (w->scroll) scroll_up();
		else { w->curx = COLS - 1; return ERR; }
	}
	return OK;
}

int waddnstr(WINDOW *w, const char *s, int n)
{
	if (!s) return ERR;
	for (; *s && n != 0; s++, n--)
		waddch(w, (unsigned char)*s);
	return OK;
}

int printw(const char *fmt, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof buf, fmt, ap);
	va_end(ap);
	return waddnstr(stdscr, buf, -1);
}

int werase(WINDOW *w)
{
	for (int r = 0; r < LINES; r++) blank_from(r, 0);
	w->cury = w->curx = 0;
	return OK;
}
int wclear(WINDOW *w) { werase(w); w->clear = true; return OK; }

int wclrtoeol(WINDOW *w) { blank_from(w->cury, w->curx); return OK; }
int wclrtobot(WINDOW *w)
{
	blank_from(w->cury, w->curx);
	for (int r = w->cury + 1; r < LINES; r++) blank_from(r, 0);
	return OK;
}

chtype winch(WINDOW *w)
{
	cell *c = &scr[w->cury][w->curx];
	return c->ch | c->a;
}

int wrefresh(WINDOW *w)
{
	for (int r = 0; r < LINES; r++)
		for (int c = 0; c < COLS; c++) {
			cell *s = &scr[r][c], *o = &shown[r][c];
			if (!w->clear && s->ch == o->ch && s->a == o->a)
				continue;
			int p = PAIR_NUMBER(s->a);
			be_put(r, c, s->ch, s->a & ~A_COLOR, pfg[p], pbg[p]);
			*o = *s;
		}
	w->clear = false;
	be_cursor(w->cury, w->curx);
	be_flush();
	return OK;
}

int wgetch(WINDOW *w)
{
	if (npushed) return pushed[--npushed];
	wrefresh(w);
	return be_getkey();
}

int ungetch(int c)
{
	if (npushed >= (int)(sizeof pushed / sizeof pushed[0])) return ERR;
	pushed[npushed++] = c;
	return OK;
}

const char *keyname(int c)
{
	static char buf[16];
	switch (c) {
	case KEY_DOWN: return "KEY_DOWN";
	case KEY_UP: return "KEY_UP";
	case KEY_LEFT: return "KEY_LEFT";
	case KEY_RIGHT: return "KEY_RIGHT";
	case KEY_HOME: return "KEY_HOME";
	case KEY_END: return "KEY_END";
	case KEY_BACKSPACE: return "KEY_BACKSPACE";
	case KEY_DC: return "KEY_DC";
	case KEY_IC: return "KEY_IC";
	case KEY_NPAGE: return "KEY_NPAGE";
	case KEY_PPAGE: return "KEY_PPAGE";
	case KEY_ENTER: return "KEY_ENTER";
	}
	if (c >= KEY_F0 && c < KEY_F0 + 64)
		snprintf(buf, sizeof buf, "KEY_F(%d)", c - KEY_F0);
	else if (c < ' ')
		snprintf(buf, sizeof buf, "^%c", c + '@');
	else if (c < 0x7f)
		snprintf(buf, sizeof buf, "%c", c);
	else
		snprintf(buf, sizeof buf, "\\%03o", c);
	return buf;
}

int getcurx(WINDOW *w) { return w->curx; }
int getcury(WINDOW *w) { return w->cury; }
int getmaxx(WINDOW *w) { (void)w; return COLS; }
int getmaxy(WINDOW *w) { (void)w; return LINES; }
