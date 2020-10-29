
#include "../inc/pin_mappings.h"
#include "TMPStepper.h"

void updateTemperature() {}
void readDisplayInputs() {}
void writeDisplayOut() {}

void idle() {}

void kill() {}

void stop() {}

void setup() {

    setup_killpin();
    setup_pins()

    setup_powerhold();

    disableStepperDrivers();

    MYSERIAL0.begin(BAUDRATE);
    SERIAL_PROTOCOLLNPGM("start");
    SERIAL_ECHO_START();

    // Prepare communication for TMC drivers
    tmc_init_cs_pins();

    thermalManager.init();    // Initialize temperature loop

    job_timer.init();   // Initial setup of print job timer

    steppers.init();           // Init stepper. This enables interrupts!

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
