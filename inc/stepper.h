#ifndef STEPPER_H
#define STEPPER_H

#include "TMCStepper.h"
#include <SPI.h>
#include "stepper_pins.h"

// Settings
enum class ESTOP_TYPE { BRAKE, FREEWHEEL };


// Settings
constexpr ESTOP_TYPE ESTOP_TYPE = ESTOP_TYPE::FREEWHEEL;
#define STEALTHCHOP true
// #define HYBRID_THRESHOLD true
#define MICROSTEPPING 16

// You can define starting values here:
constexpr struct {
  const uint16_t microsteps      = MICROSTEPPING;    // TODO
  const uint8_t blank_time       = 24;   // [16, 24, 36, 54]
  const uint8_t off_time         = 3;    // [1..15]
  const uint8_t hysteresis_start = 1;    // [1..8]
  const int8_t hysteresis_end    = 12;   // [-3..12]
  const float hold_multiplier    = 0.5f; // [0..1]
  const float threshold          = 0.1f; // TODO
  const bool interpolate         = true;
} stepperconfig;

#ifndef STEALTHCHOP
#define STEALTHCHOP false
#endif /* ifndef STEALTHCHOP */
#ifndef HYBRID_THRESHOLD
#define HYBRID_THRESHOLD false
#endif /* ifndef HYBRID_THRESHOLD */


// #if TMC2130STEPPER_VERSION < 0x020201
//   #error "Update TMC2130Stepper library to 2.2.1 or newer ver"" ##TMC2130STEPPER_VERSION
// #endif

TMC2130Stepper STEPPER0(STEPPER_0_CS, STEPPER_0_R_SENSE);
TMC2130Stepper STEPPER1(STEPPER_1_CS, STEPPER_1_R_SENSE);
TMC2130Stepper steppers[] = { STEPPER0, STEPPER1 };

// Following Quick configuration guide Page 81/103
// https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf

// Use internal reference voltage for current calculations. This is the default.
// Following values from Trinamic's spreadsheet with values for a NEMA17 (42BYGHW609)
// https://www.trinamic.com/products/integrated-circuits/details/tmc2130/

  void tmc2130_init(TMC2130Stepper &st, const uint16_t mA, const float spmm) {
    st.begin();
    st.I_scale_analog(false);
    st.internal_Rsense(false);
    st.rms_current(mA, stepperconfig.hold_multiplier);
    st.setSPISpeed(4e6);
    st.microsteps(stepperconfig.microsteps);
    st.blank_time(stepperconfig.blank_time);
    st.intpol(stepperconfig.interpolate); // Interpolate
    // st.dedge(stepperconfig.dedge);
    st.TPOWERDOWN(128); // ~2s until driver lowers to hold current

    if (ESTOP_TYPE == ESTOP_TYPE::BRAKE) {
      st.stop_enable(true);
    } else {
      st.stop_enable(false);
    }

    // Could be tuned for stealthchop
    st.toff(5); // Only enables the driver if used with stealthChop
    st.hysteresis_start(3);
    st.hysteresis_end(2);

    if (STEALTHCHOP) {
      st.en_pwm_mode(true);
      st.pwm_freq(1); // f_pwm = 2/683 f_clk
      st.pwm_autoscale(true);
      st.pwm_grad(5);
      st.pwm_ampl(255);
      if (HYBRID_THRESHOLD) {
        st.TPWMTHRS(12650000UL*stepperconfig.microsteps/(256*stepperconfig.threshold*spmm));
      }
    }
    st.GSTAT(); // Clear GSTAT
  }

  //     stepperX.sgt(X_HOMING_SENSITIVITY);

#endif /* STEPPER_H */
