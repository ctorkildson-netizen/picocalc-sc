# sc for the PicoCalc

[sc](https://github.com/n-t-roff/sc), the Unix spreadsheet calculator
(version 7.16), running on the ClockworkPi PicoCalc with an RP2350 module
(Pico 2 or Pico 2 W). It starts from the PicoCalc boot menu and returns to
it when you quit.

- 64 columns by 32 rows in the 5×10 font, or 40 columns in the 8×10 font
- sc's formulas, functions, formats, ranges, colour and help, unchanged
- **CSV**: a name ending in `.csv` opens and saves as comma-separated values
- ready-made date, time, money and percent formats in `/sc/scrc.txt`
- the clock and time zone left by settime, VirtualT or the FTP server, for
  `@now` and the date functions
- running out of memory offers to save the sheet first, and says so

**GUIDE.TXT** walks through a first sheet in ten minutes, **SC.TXT** is the
reference, and **TESTING.TXT** the alpha checklist.

## Install

Copy `sc.uf2` to the card's `pico2-apps` folder, and `scrc.txt` (and the
samples, if you like) to a folder called `sc` at the top of the card.
sc reads its settings from `/sc/.scrc` or, failing that, `/sc/scrc.txt`.

## Building

The PicoCalc build needs the Pico SDK 2.x and the submodules:

```
git clone --recursive https://github.com/ctorkildson-netizen/picocalc-sc
cd picocalc-sc && mkdir build && cd build
cmake -G Ninja .. && ninja          # PICO_SDK_PATH set
```

`Makefile.host` builds the same code on Linux with a scripted screen and
keyboard (`port/be_host.c`), for testing without a PicoCalc:

```
make -f Makefile.host
SCKEYS='=5\n\D=A0*2\n' build-host/sc     # types the keys, prints the screen
```

## Layout

| | |
|---|---|
| `sc/` | sc 7.16 (public domain). Changes are marked "PicoCalc:" in comments |
| `port/` | the PicoCalc layer: a small curses (`curses.c`), the LCD and keyboard (`be_pico.c`), CSV (`csv.c`), startup (`main_pico.c`), the Unix calls the PicoCalc can't make (`os_pico.c`) |
| `modules/` | Blair Leduc's picocalc-text-starter drivers and picocalc-shell header, as submodules pinned to releases |
| `samples/` | example sheets, and size tests in `samples/size` |

## Changes to sc itself

- CSV load and save (`readfile`, `writefile`)
- strings containing `"` are saved as `\"`, so they load again (sc 7.16 wrote
  them unescaped and could not read its own file back)
- the help pages are reflowed to the screen width
- no crash on quit when there is no temporary file to merge the history in
- an emergency memory reserve, freed before the out-of-memory save

## Licences

sc is public domain ([sc/LICENSE](sc/LICENSE)). The PicoCalc port is
BSD-2-Clause ([LICENSE](LICENSE)). Blair Leduc's drivers are MIT; see
[THIRD-PARTY.md](THIRD-PARTY.md).

## Credits

sc: James Gosling, Mark Weiser, Bruce Israel, R. Bond, Alan Silverstein,
Chuck Martin and many others since 1982; the 7.16 fork maintained by
n-t-roff. PicoCalc drivers and shell: Blair Leduc. Port by Chris Torkildson,
written with Claude (Anthropic) and tested on a PicoCalc.
