/* sc_pico.h -- PicoCalc settings shared by main_pico.c and be_pico.c */
#ifndef SC_PICO_H
#define SC_PICO_H
#include <stdbool.h>

#define SC_DIR "/sc"		/* where files without a path go */

struct sc_settings {
	bool wide;		/* the 8x10 font: 40 columns instead of 64 */
	bool beep;
	bool showkeys;
	bool showmem;		/* show free memory in KB, bottom right */		/* show each raw key event, for diagnosing keys */
};
extern struct sc_settings sc_settings;

#endif
