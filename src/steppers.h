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
#define MICROSTEPPING 4

#define MOTORSTEPS 200
/* https://e3d-online.dozuki.com/Guide/Extruder+steps+per+mm/96 */
#define GEAR_RATIO 3
#define HOB_DIAMETER 7.3
//The prescaler cannot be changed without modifying the CSXY bits setupStepperTimerX
#define STEP_PRESCALER 8
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
constexpr double STEPS_PER_MM = MOTORSTEPS*MICROSTEPPING*GEAR_RATIO / (HOB_DIAMETER * PI);


// TMC Stepper Config
inline struct TMCDriverConfig {
  const uint16_t microsteps      = MICROSTEPPING;    // TODO
  const uint8_t blank_time       = 24;   // [16, 24, 36, 54]
  const uint8_t off_time         = 3;    // [1..15]
  const uint8_t hysteresis_start = 1;    // [1..8]
  const int8_t hysteresis_end    = 12;   // [-3..12]
  const float hold_multiplier    = 0.5f; // [0..1]
  const float threshold          = 0.1f; // TODO
  const bool interpolate         = true;
  const bool dedge               = true;
  const uint16_t mA              = 900;
  const float spmm               = 800; // TODO
} TMCStepperConfig;

#ifndef STEALTHCHOP
#define STEALTHCHOP false
#endif /* ifndef STEALTHCHOP */
#ifndef HYBRID_THRESHOLD
#define HYBRID_THRESHOLD false
#endif /* ifndef HYBRID_THRESHOLD */

/******************************************************************************/



#ifndef STEPPER_R_SENSE
#define STEPPER_R_SENSE (0.11f)
#endif
#define STEPPER_0_R_SENSE STEPPER_R_SENSE
#define STEPPER_1_R_SENSE STEPPER_R_SENSE

class Stepper;
extern Stepper* extruderInstance;

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
      uint16_t targetPulse = 0xFFFF;
      Direction direction = Direction::CW;

      bool invertDir = false;

      TMC2130Stepper &stpdrv;
      TMCDriverConfig tmcConf;
      uint16_t lastStatusUpdate = 0;
      uint32_t drvStatus = 0;

      // Internal functions
      void changeStepper0Direction();
      void changeStepper1Direction();

      void tmc2130_init();

      static void setupStepperTimer4();
      static void setupStepperTimer5();
      static void updateStepperTimer4(const Stepper &st);
      static void updateStepperTimer5(const Stepper &st);

      public:
      // Following Quick configuration guide Page 81/103
      // https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf
      Stepper(uint8_t enablePin, uint8_t stepPin, uint8_t dirPin, TMC2130Stepper &stp, bool invert=false);
      ~Stepper() = default;
      void setup();
      void setDirection(Direction dir);
      void changeDirection();
      void setTargetStepperSpeed(const double& speed);
      void disable();
      void enable();
      void setupPins();
      uint32_t getDrvStatus();
      bool stallStatus();
      bool isEnabled();
      void setupTimers();
      void updateSpeed();
      void printStatus();

      static void Stepper0ISR();
      static void Stepper1ISR();
};

// timerSetup(); //TODO

#endif /* STEPPER_H */
