# sc, the spreadsheet calculator, for the ClockworkPi PicoCalc (RP2350).
# sc 7.16 (public domain) in sc/, the PicoCalc layer in port/, and Blair
# Leduc's drivers and shell header as pinned submodules in modules/.
cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(PICO_BOARD pico2_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
project(picocalc-sc C CXX ASM)
pico_sdk_init()

set(DRV modules/picocalc-text-starter/drivers)
set(SC_SRC abbrev cmds color crypt format frame gram help interp lex pipe
    range sc screen sort version vi vmtbl xmalloc compat)
list(TRANSFORM SC_SRC PREPEND sc/)
list(TRANSFORM SC_SRC APPEND .c)

add_executable(picocalc-sc
    ${SC_SRC}
    port/curses.c port/csv.c port/port.c port/be_pico.c port/main_pico.c port/os_pico.c
    ${DRV}/audio.c ${DRV}/clib.c ${DRV}/fat32.c ${DRV}/font-5x10.c ${DRV}/font-8x10.c
    ${DRV}/keyboard.c ${DRV}/lcd.c ${DRV}/sdcard.c ${DRV}/southbridge.c
)
# sc's own code: old C, quiet its warnings; its main() is called by ours
set_source_files_properties(${SC_SRC} port/csv.c PROPERTIES COMPILE_OPTIONS "-w;-include;${CMAKE_CURRENT_LIST_DIR}/port/os_pico.h")
set_source_files_properties(sc/sc.c PROPERTIES COMPILE_DEFINITIONS "main=sc_main")
# PICO_MALLOC_PANIC=0: let malloc return NULL when memory runs out, so sc's
# own out-of-memory handling (offer to save, then a message) can run; the
# SDK's default panics instead.
target_compile_definitions(picocalc-sc PRIVATE
    HAVE_STDBOOL_H HAVE_ISFINITE HAVE_ATTR_T HAVE_ATTR_GET FBUFLEN=1024
    PICO_MALLOC_PANIC=0 SIGVOID NONOTIMEOUT LIBDIR=\"/sc\" PICOCALC_APP_NAME=\"sc\")
target_include_directories(picocalc-sc PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/port ${CMAKE_CURRENT_LIST_DIR}/sc ${CMAKE_CURRENT_LIST_DIR}/port/fake
    ${CMAKE_CURRENT_LIST_DIR}/modules/picocalc-text-starter
    ${CMAKE_CURRENT_LIST_DIR}/modules/picocalc-text-starter/drivers
    ${CMAKE_CURRENT_LIST_DIR}/modules/picocalc-shell/include)
# files named without a path go in /sc
target_link_options(picocalc-sc PRIVATE -Wl,--wrap=_open -Wl,--wrap=_stat -Wl,--wrap=_unlink -Wl,--wrap=rename)
pico_set_program_name(picocalc-sc "sc")
pico_enable_stdio_uart(picocalc-sc 0)
pico_enable_stdio_usb(picocalc-sc 0)
target_link_libraries(picocalc-sc pico_stdlib hardware_gpio hardware_i2c hardware_spi
    hardware_pio hardware_clocks hardware_watchdog pico_multicore pico_aon_timer)
pico_generate_pio_header(picocalc-sc ${CMAKE_CURRENT_LIST_DIR}/${DRV}/audio.pio)
# sc formats numbers with printf: use newlib's complete printf, not the
# SDK's small one (which prints %f above 1e9 in exponent form, and
# mishandles %g)
pico_set_printf_implementation(picocalc-sc compiler)
pico_add_extra_outputs(picocalc-sc)
