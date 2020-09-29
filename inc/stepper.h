#ifndef STEPPER_H
#define STEPPER_H

// #include "stepper_pins.h"

//
// TMC2130 Driver objects and inits
// Taken and modified from https://github.com/Naesstrom/Marlin/blob/master/stepper_indirection.cpp
//

#if HAS_DRIVER(TMC2130)

  #include <SPI.h>
  #include <TMC2130Stepper.h>
  // #include "planner.h"
  // #include "enum.h"

  #if TMC2130STEPPER_VERSION < 0x020201
    #error "Update TMC2130Stepper library to 2.2.1 or newer."
  #endif

#define _TMC2130_DEFINE(ST) TMC2130Stepper stepper##ST(ST##_ENABLE_PIN, ST##_DIR_PIN, ST##_STEP_PIN, ST##_CS_PIN)

  // Stepper objects of TMC2130 steppers used
  #if AXIS_DRIVER_TYPE(X, TMC2130)
    _TMC2130_DEFINE(X);
  #endif
  #if AXIS_DRIVER_TYPE(X2, TMC2130)
    _TMC2130_DEFINE(X2);
  #endif
  #if AXIS_DRIVER_TYPE(Y, TMC2130)
    _TMC2130_DEFINE(Y);
  #endif
  #if AXIS_DRIVER_TYPE(Y2, TMC2130)
    _TMC2130_DEFINE(Y2);
  #endif
  #if AXIS_DRIVER_TYPE(Z, TMC2130)
    _TMC2130_DEFINE(Z);
  #endif
  #if AXIS_DRIVER_TYPE(Z2, TMC2130)
    _TMC2130_DEFINE(Z2);
  #endif
  #if AXIS_DRIVER_TYPE(E0, TMC2130)
    _TMC2130_DEFINE(E0);
  #endif
  #if AXIS_DRIVER_TYPE(E1, TMC2130)
    _TMC2130_DEFINE(E1);
  #endif
  #if AXIS_DRIVER_TYPE(E2, TMC2130)
    _TMC2130_DEFINE(E2);
  #endif
  #if AXIS_DRIVER_TYPE(E3, TMC2130)
    _TMC2130_DEFINE(E3);
  #endif
  #if AXIS_DRIVER_TYPE(E4, TMC2130)
    _TMC2130_DEFINE(E4);
  #endif

  // Use internal reference voltage for current calculations. This is the default.
  // Following values from Trinamic's spreadsheet with values for a NEMA17 (42BYGHW609)
  // https://www.trinamic.com/products/integrated-circuits/details/tmc2130/
  void tmc2130_init(TMC2130Stepper &st, const uint16_t mA, const uint16_t microsteps, const uint32_t thrs, const float spmm) {
    #if DISABLED(STEALTHCHOP) || DISABLED(HYBRID_THRESHOLD)
      UNUSED(thrs);
      UNUSED(spmm);
    #endif
    st.begin();
    st.setCurrent(mA, R_SENSE, HOLD_MULTIPLIER);
    st.microsteps(microsteps);
    st.blank_time(24);
    st.off_time(5); // Only enables the driver if used with stealthChop
    st.interpolate(INTERPOLATE);
    st.power_down_delay(128); // ~2s until driver lowers to hold current
    st.hysteresis_start(3);
    st.hysteresis_end(2);
    #if ENABLED(STEALTHCHOP)
      st.stealth_freq(1); // f_pwm = 2/683 f_clk
      st.stealth_autoscale(1);
      st.stealth_gradient(5);
      st.stealth_amplitude(255);
      st.stealthChop(1);
      #if ENABLED(HYBRID_THRESHOLD)
        st.stealth_max_speed(12650000UL*microsteps/(256*thrs*spmm));
      #endif
    #endif
    st.GSTAT(); // Clear GSTAT
  }

  #define _TMC2130_INIT(ST, SPMM) tmc2130_init(stepper##ST, ST##_CURRENT, ST##_MICROSTEPS, ST##_HYBRID_THRESHOLD, SPMM)

  void tmc2130_init_to_defaults() {
    #if AXIS_DRIVER_TYPE(X, TMC2130)
      _TMC2130_INIT( X, planner.axis_steps_per_mm[X_AXIS]);
    #endif
    #if AXIS_DRIVER_TYPE(X2, TMC2130)
      _TMC2130_INIT(X2, planner.axis_steps_per_mm[X_AXIS]);
    #endif
    #if AXIS_DRIVER_TYPE(Y, TMC2130)
      _TMC2130_INIT( Y, planner.axis_steps_per_mm[Y_AXIS]);
    #endif
    #if AXIS_DRIVER_TYPE(Y2, TMC2130)
      _TMC2130_INIT(Y2, planner.axis_steps_per_mm[Y_AXIS]);
    #endif
    #if AXIS_DRIVER_TYPE(Z, TMC2130)
      _TMC2130_INIT( Z, planner.axis_steps_per_mm[Z_AXIS]);
    #endif
    #if AXIS_DRIVER_TYPE(Z2, TMC2130)
      _TMC2130_INIT(Z2, planner.axis_steps_per_mm[Z_AXIS]);
    #endif
    #if AXIS_DRIVER_TYPE(E0, TMC2130)
      _TMC2130_INIT(E0, planner.axis_steps_per_mm[E_AXIS]);
    #endif
    #if AXIS_DRIVER_TYPE(E1, TMC2130)
      { constexpr int extruder = 1; _TMC2130_INIT(E1, planner.axis_steps_per_mm[E_AXIS_N]); }
    #endif
    #if AXIS_DRIVER_TYPE(E2, TMC2130)
      { constexpr int extruder = 2; _TMC2130_INIT(E2, planner.axis_steps_per_mm[E_AXIS_N]); }
    #endif
    #if AXIS_DRIVER_TYPE(E3, TMC2130)
      { constexpr int extruder = 3; _TMC2130_INIT(E3, planner.axis_steps_per_mm[E_AXIS_N]); }
    #endif
    #if AXIS_DRIVER_TYPE(E4, TMC2130)
      { constexpr int extruder = 4; _TMC2130_INIT(E4, planner.axis_steps_per_mm[E_AXIS_N]); }
    #endif

    #if ENABLED(SENSORLESS_HOMING)
      #define TMC_INIT_SGT(P,Q) stepper##Q.sgt(P##_HOMING_SENSITIVITY);
      #if X_SENSORLESS
        #if AXIS_DRIVER_TYPE(X, TMC2130)
          stepperX.sgt(X_HOMING_SENSITIVITY);
        #endif
        #if AXIS_DRIVER_TYPE(X2, TMC2130)
          stepperX2.sgt(X_HOMING_SENSITIVITY);
        #endif
      #endif
      #if Y_SENSORLESS
        #if AXIS_DRIVER_TYPE(Y, TMC2130)
          stepperY.sgt(Y_HOMING_SENSITIVITY);
        #endif
        #if AXIS_DRIVER_TYPE(Y2, TMC2130)
          stepperY2.sgt(Y_HOMING_SENSITIVITY);
        #endif
      #endif
      #if Z_SENSORLESS
        #if AXIS_DRIVER_TYPE(Z, TMC2130)
          stepperZ.sgt(Z_HOMING_SENSITIVITY);
        #endif
        #if AXIS_DRIVER_TYPE(Z2, TMC2130)
          stepperZ2.sgt(Z_HOMING_SENSITIVITY);
        #endif
      #endif
    #endif
  }
#endif // TMC2130

#endif /* STEPPER_H */
