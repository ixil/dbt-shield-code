#ifndef DISPLAY_PINS_H
#define DISPLAY_PINS_H

// SKU MSP3218
#define DISPLAY_DRIVER "ILI9341"


#define DISPLAY_PIN( PIN_NAME, PIN_N ) define DISPLAY_PIN_ ## PIN_NAME (PIN_N)
// #ifndef MISO
// #define MISO 52
// #endif
// #ifndef MOSI
// #define MOSI 51
// #endif
// #ifndef SCK
// #define SCK 52
// #endif
#ifndef DISPL_CS
#define DISPL_CS 34
#endif
#ifndef DISPL_DC
#define DISPL_DC A14
#endif

DISPLAY_PIN(D_DO, MISO)
DISPLAY_PIN(D_DI, MOSI)
DISPLAY_PIN(D_CK, SCK)
DISPLAY_PIN(D_CS, DISPL_CS)

#if DISPLAY_BACKLIGHT_CONTROL
DISPLAY_PIN(D_BACKLIGHT, A13)
#endif DISPLAY_D_BACKLIGHT_CONTROL

#if DISPLAY_TOUCH_SCREEN
// DISPLAY_PIN(T_IRQ, ) NOT CONNECTED
// #ifndef MISO
// #define MISO 52
// #endif
// #ifndef MOSI
// #define MOSI 51
// #endif
// #ifndef SCK
// #define SCK 52
// #endif
#ifndef T_CS
#define T_CS 33
#endif

DISPLAY_PIN(T_DO, MISO)
DISPLAY_PIN(T_DIN, MOSI)
DISPLAY_PIN(T_CS, T_CS)
DISPLAY_PIN(T_CLK, SCK)
DISPLAY_PIN(T_CLK, DC)
#endif

#undef DISPLAY_PIN(,)

#endif /* DISPLAY_PINS_H */
