/* be.h -- what a screen/keyboard backend supplies to the curses shim.
 * Kept apart from curses.h so a backend can include the PicoCalc driver
 * headers, whose KEY_ names clash with curses's. */
#ifndef PC_BE_H
#define PC_BE_H

/* attribute bits passed to be_put (the same values as curses.h's A_*) */
#define BE_STANDOUT	0x00010000u
#define BE_BOLD		0x00020000u
#define BE_UNDERLINE	0x00040000u

/* key codes be_getkey returns beyond plain characters (curses's values) */
#define BE_KEY_DOWN	0402
#define BE_KEY_UP	0403
#define BE_KEY_LEFT	0404
#define BE_KEY_RIGHT	0405
#define BE_KEY_HOME	0406
#define BE_KEY_BACKSPACE 0407
#define BE_KEY_F0	0410
#define BE_KEY_DC	0512
#define BE_KEY_IC	0513
#define BE_KEY_NPAGE	0522
#define BE_KEY_PPAGE	0523
#define BE_KEY_END	0550

void be_init(int *rows, int *cols);
void be_end(void);
void be_put(int row, int col, int ch, unsigned int attr, int fg, int bg);
void be_cursor(int row, int col);
void be_flush(void);
int be_getkey(void);		/* blocks; returns a char or a BE_KEY_ code */
void be_beep(void);

#endif
