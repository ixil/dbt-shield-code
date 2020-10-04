#ifndef STEPPER_H
#define STEPPER_H

// #include "stepper_pins.h"

#include <SPI.h>
#include <TMCStepper.h>
// #include "planner.h"
// #include "enum.h"

#if TMC2130STEPPER_VERSION < 0x020201
  #error "Update TMC2130Stepper library to 2.2.1 or newer."
#endif

  // Use internal reference voltage for current calculations. This is the default.
  // Following values from Trinamic's spreadsheet with values for a NEMA17 (42BYGHW609)
  // https://www.trinamic.com/products/integrated-circuits/details/tmc2130/
TMC2130Stepper(uint16_t pinCS, float RS = default_RS, int8_t link_index = -1);
TMC2130Stepper(uint16_t pinCS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);
TMC2130Stepper(uint16_t pinCS, float RS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK, int8_t link_index = -1);

TMC2130Stepper STEPPER0(STEP0_CS, STEP0_SI, STEP0_SO, STEP0_SCK);
TMC2130Stepper STEPPER1(STEP1_CS, STEP1_SI, STEP1_SO, STEP1_SCK);

  void tmc2130_init(TMC2130Stepper &st, const uint16_t mA, const uint16_t microsteps, 
      const uint32_t thrs, const float spmm) {
    st.begin();
    st.setCurrent(mA, R_SENSE, HOLD_MULTIPLIER);
    st.microsteps(microsteps);
    st.blank_time(24);
    st.off_time(5); // Only enables the driver if used with stealthChop
    st.interpolate(INTERPOLATE);
    st.power_down_delay(128); // ~2s until driver lowers to hold current
    st.hysteresis_start(3);
    st.hysteresis_end(2);
    // #if ENABLED(STEALTHCHOP)
      st.stealth_freq(1); // f_pwm = 2/683 f_clk
      st.stealth_autoscale(1);
      st.stealth_gradient(5);
      st.stealth_amplitude(255);
      st.stealthChop(1);
      // #if ENABLED(HYBRID_THRESHOLD)
        st.stealth_max_speed(12650000UL*microsteps/(256*thrs*spmm));
      // #endif
    // #endif
    st.GSTAT(); // Clear GSTAT
  }

  // #define _TMC2130_INIT(ST, SPMM) tmc2130_init(stepper##ST, ST##_CURRENT, ST##_MICROSTEPS, ST##_HYBRID_THRESHOLD, SPMM)

  // void tmc2130_init_to_defaults() {
  //     _TMC2130_INIT( X, planner.axis_steps_per_mm[X_AXIS]);
  //     { constexpr int extruder = 1; _TMC2130_INIT(E1, planner.axis_steps_per_mm[E_AXIS_N]); }
  //     stepperX.sgt(X_HOMING_SENSITIVITY);
  // }

#endif /* STEPPER_H */
