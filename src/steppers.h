#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>
#include "TMCStepper.h"
#include <SPI.h>
#include "pins.h"

// Settings
enum class ESTOP_TYPE { BRAKE, FREEWHEEL };

// Settings
constexpr ESTOP_TYPE ESTOP_TYPE = ESTOP_TYPE::FREEWHEEL;
#define STEALTHCHOP true
// #define HYBRID_THRESHOLD true
#define MICROSTEPPING 16

#define MOTORSTEPS 200
/* https://e3d-online.dozuki.com/Guide/Extruder+steps+per+mm/96 */
#define GEAR_RATIO 3
#define HOB_DIAMETER 7.3
#define STEP_PRESCALER 256
#define TIMER_FREQ 16000000UL/STEP_PRESCALER*2
#define MAX_STEPPERIOD 16000000UL/STEP_PRESCALER*2
constexpr double STEPS_PER_MM = MOTORSTEPS*MICROSTEPPING*GEAR_RATIO / (HOB_DIAMETER * PI);


// TMC Stepper Config
struct TMCDriverConfig {
  const uint16_t microsteps      = MICROSTEPPING;    // TODO
  const uint8_t blank_time       = 24;   // [16, 24, 36, 54]
  const uint8_t off_time         = 3;    // [1..15]
  const uint8_t hysteresis_start = 1;    // [1..8]
  const int8_t hysteresis_end    = 12;   // [-3..12]
  const float hold_multiplier    = 0.5f; // [0..1]
  const float threshold          = 0.1f; // TODO
  const bool interpolate         = true;
  const uint16_t mA = 900;
  const float spmm = 800; // TODO
} TMCStepperConfig;

#ifndef STEALTHCHOP
#define STEALTHCHOP false
#endif /* ifndef STEALTHCHOP */
#ifndef HYBRID_THRESHOLD
#define HYBRID_THRESHOLD false
#endif /* ifndef HYBRID_THRESHOLD */

/******************************************************************************/


// Using direct register manipulation can reach faster stepping times
#define STEP_PORT PORTA // Match with STEP_PIN
#define STEPPER0_BIT_POS      0 // Match with STEP_PIN
#define STEPPER1_BIT_POS      3 // Match with STEP_PIN

  class Stepper {

      enum class Direction { CW, CCW };

      uint8_t enablePin;
      uint8_t stepPin;
      uint8_t dirPin;

      // unsigned long last_micro = 0;
      // unsigned long last_time = 0;
      // unsigned long currentMillis;
      // unsigned long currentMicros;

      volatile bool speedChange = false;
      uint16_t targetPulse = 0;
      Direction direction = CW;

      bool invertDir = false;

      TMC2130Stepper &stdrv;
      TMCDriverConfig tmcConf;
      decltype(millis()) lastStatusUpdate = 0;
      uint32_t drvStatus = 0;

      // Internal functions
      void changeStepper0Direction();
      void changeStepper1Direction();

      void tmc2130_init();

      static void setupStepperTimer4();
      static void setupStepperTimer5();
      static void updateStepperTimer4();
      static void updateStepperTimer5();

      public:
      // Following Quick configuration guide Page 81/103
      // https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf
      Stepper(uint8_t enablePin, uint8_t stepPin, uint8_t dirPin, TMC2130Stepper &stp, bool invert=false);
      virtual ~Stepper();
      void setup();
      void setDirection(Direction dir);
      void changeDirection();
      void setTargetStepperSpeed(double speed);
      void disableStepper();
      void enableStepper();
      void setupPins();
      uint32_t getDrvStatus();
      bool stallStatus();
      bool enabled();

      static void StepperISR();
}

// timerSetup(); //TODO

#endif /* STEPPER_H */



