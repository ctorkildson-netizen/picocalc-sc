/* csv.c -- CSV files for sc.
 *
 * readfile() and writefile() call these when the file name ends in .csv
 * (any case).  Reading puts each field in a cell, starting at A0: a field
 * that is entirely a number becomes a number, anything else a
 * left-aligned label.  Fields may be quoted ("a, b", "say ""hi""") and
 * quoted fields may run over several lines (the line breaks become
 * spaces, since a cell shows one line).  Writing saves the values
 * that cells show (not their formulas), quoting where it has to.
 */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include "compat.h"
#include "sc.h"

int csv_name(const char *fname)
{
	size_t n = strlen(fname);
	if (n < 4) return 0;
	const char *e = fname + n - 4;
	return e[0] == '.' && tolower((unsigned char)e[1]) == 'c' &&
	    tolower((unsigned char)e[2]) == 's' &&
	    tolower((unsigned char)e[3]) == 'v';
}

static void put_field(int row, int col, char *s, int quoted)
{
	char *end;
	struct ent *p;

	if (!quoted) {			/* trim unquoted fields */
		while (*s == ' ' || *s == '\t') s++;
		end = s + strlen(s);
		while (end > s && (end[-1] == ' ' || end[-1] == '\t')) *--end = 0;
	}
	if (!*s) return;
	if (col > ABSMAXCOLS - 1) return;
	checkbounds(&row, &col);
	p = lookat(row, col);

	double d = strtod(s, &end);
	while (*end == ' ' || *end == '\t') end++;
	if (end != s && *end == 0)
		let(p, new_const(O_CONST, d));
	else
		label(p, s, -1);
}

/* returns the number of rows read */
int csv_read(FILE *f)
{
	char field[FBUFLEN];
	size_t n = 0;
	int row = 0, col = 0, quoted = 0, inq = 0, any = 0, c;

	for (;;) {
		c = getc(f);
		if (inq) {
			if (c == EOF) { inq = 0; continue; }
			if (c == '"') {
				int d = getc(f);
				if (d == '"') { if (n < sizeof field - 1) field[n++] = '"'; }
				else { inq = 0; ungetc(d, f); }
			} else if (c != '\r' && n < sizeof field - 1)
				field[n++] = c == '\n' ? ' ' : (char)c;	/* one line per cell */
			continue;
		}
		if (c == '"' && n == 0) { inq = quoted = 1; continue; }
		if (c == ',' || c == '\n' || c == EOF) {
			field[n] = 0;
			if (n || quoted) { put_field(row, col, field, quoted); any = 1; }
			n = 0; quoted = 0;
			if (c == ',') { col++; any = 1; continue; }
			if (c == '\n' || any) row++;
			col = 0; any = 0;
			if (c == EOF) break;
			continue;
		}
		if (c == '\r') continue;
		if (n < sizeof field - 1) field[n++] = (char)c;
	}
	return row;
}

static void put_csv(FILE *f, const char *s)
{
	int q = !*s ? 0 : (strpbrk(s, ",\"\n\r") != NULL ||
	    s[0] == ' ' || s[strlen(s) - 1] == ' ');
	if (!q) { fputs(s, f); return; }
	putc('"', f);
	for (; *s; s++) {
		if (*s == '"') putc('"', f);
		putc(*s, f);
	}
	putc('"', f);
}

void csv_write(FILE *f, int r0, int c0, int rn, int cn)
{
	char num[64];

	for (int r = r0; r <= rn; r++) {
		for (int c = c0; c <= cn; c++) {
			struct ent *p = *ATBL(tbl, r, c);
			if (c > c0) putc(',', f);
			if (!p) continue;
			if (p->flags & IS_VALID) {
				snprintf(num, sizeof num, "%.15g", p->v);
				fputs(num, f);
			} else if (p->label)
				put_csv(f, p->label);
		}
		putc('\n', f);
	}
}

/* Append s to buf at *len as an sc string constant: "...", with each "
 * written as \" (which the lexer turns back into "). */
void quote_str(char *buf, size_t size, ssize_t *len, const char *s)
{
	size_t n = (size_t)*len;
	if (n + 1 < size) buf[n++] = '"';
	for (; s && *s && n + 3 < size; s++) {
		if (*s == '"') buf[n++] = '\\';
		buf[n++] = *s;
	}
	if (n + 1 < size) buf[n++] = '"';
	buf[n] = 0;
	*len = (ssize_t)n;
}
