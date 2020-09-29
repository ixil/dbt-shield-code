
#include "../inc/pin_mappings.h"
#include <TMPStepper.h

void updateTemperature() {}
void readDisplayInputs() {}
void writeDisplayOut() {}

void idle() {}

void kill() {}

void stop() {}

void setup() { 

    setup_killpin();

    setup_powerhold();

#if HAS_STEPPER_RESET
    disableStepperDrivers();
#endif

    MYSERIAL0.begin(BAUDRATE);
    SERIAL_PROTOCOLLNPGM("start");
    SERIAL_ECHO_START();

    // Prepare communication for TMC drivers
#if HAS_DRIVER(TMC2130)
    tmc_init_cs_pins();
#endif

    // // Check startup - does nothing if bootloader sets MCUSR to 0
    // byte mcu = MCUSR;
    // if (mcu &  1) SERIAL_ECHOLNPGM(MSG_POWERUP);
    // if (mcu &  2) SERIAL_ECHOLNPGM(MSG_EXTERNAL_RESET);
    // if (mcu &  4) SERIAL_ECHOLNPGM(MSG_BROWNOUT_RESET);
    // if (mcu &  8) SERIAL_ECHOLNPGM(MSG_WATCHDOG_RESET);
    // if (mcu & 32) SERIAL_ECHOLNPGM(MSG_SOFTWARE_RESET);
    // MCUSR = 0;

    // SERIAL_ECHOPGM(MSG_MARLIN);
    // SERIAL_CHAR(' ');
    // SERIAL_ECHOLNPGM(SHORT_BUILD_VERSION);
    // SERIAL_EOL();

    // #if defined(STRING_DISTRIBUTION_DATE) && defined(STRING_CONFIG_H_AUTHOR)
    //   SERIAL_ECHO_START();
    //   SERIAL_ECHOPGM(MSG_CONFIGURATION_VER);
    //   SERIAL_ECHOPGM(STRING_DISTRIBUTION_DATE);
    //   SERIAL_ECHOLNPGM(MSG_AUTHOR STRING_CONFIG_H_AUTHOR);
    //   SERIAL_ECHO_START();
    //   SERIAL_ECHOLNPGM("Compiled: " __DATE__);
    // #endif

    // SERIAL_ECHO_START();
    // SERIAL_ECHOPAIR(MSG_FREE_MEMORY, freeMemory());
    // SERIAL_ECHOLNPAIR(MSG_PLANNER_BUFFER_BYTES, int(sizeof(block_t))*(BLOCK_BUFFER_SIZE));

    // Send "ok" after commands by default
    // for (int8_t i = 0; i < BUFSIZE; i++) send_ok[i] = true;

    // Load data from EEPROM if available (or use defaults)
    // This also updates variables in the planner, elsewhere
    // (void)settings.load();


    thermalManager.init();    // Initialize temperature loop

    job_timer.init();   // Initial setup of print job timer

    stepper.init();           // Init stepper. This enables interrupts!

    lcd_init();
    lcd_reset_status();

#if ENABLED(SHOW_BOOTSCREEN)
    lcd_bootscreen();
#endif

#if ENABLED(USE_WATCHDOG)
    watchdog_init();
#endif
}

void loop() {

    display.event_handler();
    idle();
}
