/* be_pico.c -- the curses shim's backend on the PicoCalc: the LCD through
 * Blair Leduc's picocalc-text-starter drivers, and its keyboard.
 *
 * Keys:  arrows, Home/End, PgUp/PgDn, Del, Ins, Backspace, Tab, Enter and
 * Esc as on a terminal; Ctrl+letter gives the control character;
 * F1-F10 are sc's function keys.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "drivers/lcd.h"
#include "drivers/font.h"
#include "drivers/keyboard.h"
#include "drivers/southbridge.h"
#include "drivers/audio.h"
#include "be.h"
#include "sc_pico.h"
#include <malloc.h>
#include <unistd.h>

extern char __StackLimit;		/* the top of the heap, from the linker */

/* free memory: what the heap can still grow by, plus freed blocks */
static unsigned free_kb(void)
{
	struct mallinfo mi = mallinfo();
	char *brk = sbrk(0);
	return (unsigned)(((&__StackLimit - brk) + mi.fordblks) / 1024);
}

volatile bool user_interrupt;		/* the keyboard driver sets it on Brk */

#define MAXR 32
#define MAXC 64

static unsigned char sh_ch[MAXR][MAXC];	/* what is on the LCD, for the cursor */
static unsigned int sh_a[MAXR][MAXC];
static uint16_t sh_fg[MAXR][MAXC], sh_bg[MAXR][MAXC];
static int rows, cols, cur_r = -1, cur_c = -1;

/* curses colour numbers to RGB565 */
static const uint16_t rgb[8] = {
	0x0000,		/* black */
	0xF800,		/* red */
	0x07E0,		/* green */
	0xFFE0,		/* yellow */
	0x3A7F,		/* blue, lightened so it reads on black */
	0xF81F,		/* magenta */
	0x07FF,		/* cyan */
	0xFFFF,		/* white */
};

static void draw(int r, int c)
{
	uint16_t fg = sh_fg[r][c], bg = sh_bg[r][c];
	if (sh_a[r][c] & BE_STANDOUT) { uint16_t t = fg; fg = bg; bg = t; }
	lcd_set_reverse(false);
	lcd_set_foreground(fg);
	lcd_set_background(bg);
	lcd_set_bold((sh_a[r][c] & BE_BOLD) != 0);
	lcd_set_underscore((sh_a[r][c] & BE_UNDERLINE) != 0);
	lcd_putc(c, r, sh_ch[r][c]);
}

void be_init(int *r, int *c)
{
	lcd_set_font(sc_settings.wide ? &font_8x10 : &font_5x10);
	lcd_enable_cursor(false);	/* we draw our own */
	lcd_clear_screen();
	rows = MAXR;
	cols = lcd_get_columns();
	if (cols > MAXC) cols = MAXC;
	memset(sh_ch, ' ', sizeof sh_ch);
	*r = rows;
	*c = cols;
}

void be_end(void) {}

void be_put(int r, int c, int ch, unsigned int attr, int fg, int bg)
{
	if (r >= rows || c >= cols) return;
	sh_ch[r][c] = (unsigned char)ch;
	sh_a[r][c] = attr;
	sh_fg[r][c] = rgb[fg & 7];
	sh_bg[r][c] = rgb[bg & 7];
	draw(r, c);
	if (r == cur_r && c == cur_c) cur_r = -1;	/* cursor overwritten */
}

/* sc parks the cursor in the bottom-right corner when the cell highlight
   shows where you are; no cursor is drawn there */
void be_cursor(int r, int c)
{
	if (cur_r >= 0) draw(cur_r, cur_c);	/* erase the old one */
	cur_r = -1;
	if (r < 0 || r >= rows || c < 0 || c >= cols) return;
	if (r == rows - 1 && c == cols - 1) return;
	uint16_t col = (sh_a[r][c] & BE_STANDOUT) ? sh_bg[r][c] : sh_fg[r][c];
	lcd_solid_rectangle(col, c * lcd_get_glyph_width(), r * GLYPH_HEIGHT + GLYPH_HEIGHT - 1,
	    lcd_get_glyph_width(), 1);
	cur_r = r;
	cur_c = c;
}

/* keys=show / mem=show: drawn at the right end of the top line after
   every screen update, so sc's own drawing never hides it */
static uint16_t last_ev;
static void overlay(void)
{
	char b[24];
	if (!sc_settings.showkeys && !sc_settings.showmem) return;
	if (sc_settings.showkeys && sc_settings.showmem)
		snprintf(b, sizeof b, " %uK %04X", free_kb(), last_ev);
	else if (sc_settings.showmem)
		snprintf(b, sizeof b, " %uK free", free_kb());
	else
		snprintf(b, sizeof b, " %04X", last_ev);
	lcd_set_reverse(false);
	lcd_set_bold(false);
	lcd_set_underscore(false);
	lcd_set_foreground(0xFFE0);
	lcd_set_background(0x0000);
	lcd_putstr(cols - (int)strlen(b), 0, b);
}

void be_flush(void) { overlay(); }

void be_beep(void)
{
	if (sc_settings.beep) audio_play_sound_blocking(880, 880, 60);
}

int be_getkey(void)
{
	for (;;) {
		uint16_t ev = keyboard_get_event();
		int code = ev & 0xff;
		last_ev = ev;
		overlay();
		bool ctl = (ev & KBD_MASK_CTL) != 0;

		switch (code) {
		case KEY_UP:	return BE_KEY_UP;
		case KEY_DOWN:	return BE_KEY_DOWN;
		case KEY_LEFT:	return BE_KEY_LEFT;
		case KEY_RIGHT:	return BE_KEY_RIGHT;
		case KEY_HOME:	return BE_KEY_HOME;
		case KEY_END:	return BE_KEY_END;
		case KEY_PAGE_UP: return BE_KEY_PPAGE;
		case KEY_PAGE_DOWN: return BE_KEY_NPAGE;
		case KEY_DEL:	return BE_KEY_DC;
		case KEY_INSERT: return BE_KEY_IC;
		case KEY_BACKSPACE: return BE_KEY_BACKSPACE;
		case KEY_ESC:	return 27;
		case KEY_RETURN: case KEY_ENTER: return '\r';
		case KEY_TAB:	return '\t';
		case KEY_BREAK:	return 3;	/* Ctrl+C */
		case KEY_CAPS_LOCK: case KEY_POWER: continue;
		}
		if (code >= KEY_F1 && code <= KEY_F9) return BE_KEY_F0 + code - KEY_F1 + 1;
		if (code == KEY_F10) return BE_KEY_F0 + 10;
		if (ctl && ((code >= 'a' && code <= 'z') || (code >= 'A' && code <= 'Z') ||
		    (code >= '@' && code <= '_')))
			return code & 0x1f;
		if (code >= 1 && code < 0x7f) return code;
	}
}
