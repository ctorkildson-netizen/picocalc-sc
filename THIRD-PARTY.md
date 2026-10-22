# Third-party material

This project's own code (`port/`, the build files and the documentation) is
under the BSD-2-Clause licence in [`LICENSE`](LICENSE). Everything below came
from elsewhere and keeps its own licence.

## sc 7.16 — public domain

<https://github.com/n-t-roff/sc>, the 7.16 fork of sc, the spreadsheet
calculator first written by James Gosling in 1982. Its notice is in
[`sc/LICENSE`](sc/LICENSE): public domain. `sc/gram.c`, `sc/y.tab.h`,
`sc/experres.h` and `sc/statres.h` are generated from `sc/gram.y` by its own
build and are included so the PicoCalc build needs no yacc.

The changes made here are marked "PicoCalc:" in the source:

| File | Change |
|---|---|
| `cmds.c` | CSV in `readfile()` and `writefile()` |
| `interp.c`, `lex.c` | a `"` in a string is saved as `\"` and read back |
| `help.c` | help pages reflowed to the screen width |
| `vi.c` | history save works without `tmpfile()` |
| `xmalloc.c`, `sc.c` | an emergency memory reserve for the out-of-memory save |
| `sc.h`, `compat.h` | the port's prototypes and header choices |

## picocalc-text-starter — MIT, Blair Leduc

<https://github.com/BlairLeduc/picocalc-text-starter>, a submodule in
`modules/picocalc-text-starter`, used unchanged. The build compiles its
`drivers/` for the LCD, fonts, keyboard, SD card and FAT32, the C library
file calls (`clib.c`), audio, and the keyboard controller (`southbridge.c`).

    MIT License

    Copyright (c) 2025 Blair Leduc

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

## picocalc-shell — `picocalc_shell.h` only, MIT, Blair Leduc

<https://github.com/BlairLeduc/picocalc-shell>, a submodule in
`modules/picocalc-shell`. Only its header `include/picocalc_shell.h` is
used, so sc can hand control back to the shell. The shell itself is
GPL-3.0, but that header is also offered under the MIT licence so that
programs under any licence can include it. Nothing else from the shell is
compiled in.

## Raspberry Pi Pico SDK

<https://github.com/raspberrypi/pico-sdk>, BSD-3-Clause, Raspberry Pi Ltd.
