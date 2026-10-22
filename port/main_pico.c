/* main_pico.c -- start sc on the PicoCalc.
 *
 * Files without a path are kept in /sc on the SD card.  /sc/sc.cfg may
 * hold "font=large" (8x10 font, 40 columns), "beep=off", "keys=show"
 * (each key's raw code) and "mem=show" (free memory), both at the
 * right end of the top line.  sc's own settings go in /sc/.scrc, as on Unix.
 * Quitting sc (q) returns to the PicoCalc boot menu.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "drivers/lcd.h"
#include "drivers/keyboard.h"
#include "drivers/southbridge.h"
#include "drivers/audio.h"
#include "drivers/fat32.h"
#include "pico/aon_timer.h"
#include "hardware/watchdog.h"
#include <time.h>
#include <sys/time.h>
#include "picocalc_shell.h"
#include "sc_pico.h"

struct sc_settings sc_settings = { .wide = false, .beep = true };

int sc_main(int argc, char **argv);

static void read_cfg(void)
{
	FILE *f = fopen(SC_DIR "/sc.cfg", "r");
	char line[80];
	if (!f) return;
	while (fgets(line, sizeof line, f)) {
		char *p = line;
		while (*p == ' ' || *p == '\t') p++;
		if (!strncmp(p, "font", 4) && strstr(p, "large")) sc_settings.wide = true;
		if (!strncmp(p, "beep", 4) && strstr(p, "off")) sc_settings.beep = false;
		if (!strncmp(p, "keys", 4) && strstr(p, "show")) sc_settings.showkeys = true;
		if (!strncmp(p, "mem", 3) && strstr(p, "show")) sc_settings.showmem = true;
	}
	fclose(f);
}

#define SC_STACK (48 * 1024)
static uint64_t sc_stack[SC_STACK / 8];

static void __attribute__((noreturn)) run_sc(void)
{
	char *argv[] = { "sc", NULL };
	sc_main(1, argv);
	exit(0);
}

/* the time zone, from the files settime and VirtualT keep it in */
static void read_tz(void)
{
	static const char *files[] = { "/settime.cfg", "/virtualt/virtualt.cfg" };
	static char tz[64] = "PST8PDT,M3.2.0,M11.1.0";
	char line[100];
	for (unsigned i = 0; i < sizeof files / sizeof files[0]; i++) {
		FILE *f = fopen(files[i], "r");
		if (!f) continue;
		bool found = false;
		while (fgets(line, sizeof line, f)) {
			char *p = line;
			while (*p == ' ' || *p == '\t') p++;
			if (strncmp(p, "tz", 2)) continue;
			p += 2;
			while (*p == ' ' || *p == '\t') p++;
			if (*p++ != '=') continue;
			while (*p == ' ' || *p == '\t') p++;
			p[strcspn(p, " \t\r\n#")] = 0;
			if (*p) { strncpy(tz, p, sizeof tz - 1); found = true; }
			break;
		}
		fclose(f);
		if (found) break;
	}
	setenv("TZ", tz, 1);
	tzset();
}

/* @now and the date functions need the clock: keep the time an earlier
   program (settime, VirtualT, the FTP server) left in the always-on timer */
static void read_clock(void)
{
	struct timespec ts;
	if (aon_timer_is_running() && aon_timer_get_time(&ts) && ts.tv_sec > 1600000000) {
		struct timeval tv = { .tv_sec = ts.tv_sec, .tv_usec = ts.tv_nsec / 1000 };
		settimeofday(&tv, NULL);
	}
}

/* sc ends with exit(); if anything gets past to_menu, or the program
   faults, go back to the menu rather than sit on a frozen screen */
void __attribute__((noreturn)) _exit(int status)
{
	(void)status;
	picocalc_shell_exit();
	watchdog_reboot(0, 0, 0);
	for (;;) tight_loop_contents();
}

void isr_hardfault(void)
{
	_exit(1);
}

static void to_menu(void)
{
	fflush(NULL);
	picocalc_shell_exit();		/* no card: restart instead */
	watchdog_reboot(0, 0, 0);
	for (;;) tight_loop_contents();
}

int main(void)
{
	sb_init();
	lcd_init();
	fat32_init();
	keyboard_init();
	keyboard_set_background_poll(true);
	audio_init();

	fat32_file_t d;
	if (fat32_dir_create(&d, SC_DIR) == FAT32_OK) fat32_close(&d);
	setenv("HOME", SC_DIR, 1);
	setenv("TERM", "picocalc", 1);
	read_cfg();
	read_tz();
	read_clock();

	atexit(to_menu);

	/* sc keeps big buffers on the stack and the RP2350's own stack is
	   4 KB of scratch RAM, so run it on a stack of its own */
	uint32_t top = (uint32_t)&sc_stack[sizeof sc_stack / sizeof sc_stack[0]];
	__asm volatile ("mov sp, %0\n\tblx %1\n" : : "r" (top), "r" (run_sc) : "memory");
	for (;;) ;
}
