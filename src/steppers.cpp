#include "steppers.h"

void Stepper::StepperISR(){
  if (extruderInstance){
    if (extruderInstance->speedChange){
      updateStepperTimer5(*extruderInstance);
    }
  }
}

Stepper::Stepper(uint8_t enablePin, uint8_t stepPin, uint8_t dirPin, TMC2130Stepper &stp, bool
    invert) : enablePin(enablePin), stepPin(stepPin), dirPin(dirPin), speedChange(false),
  targetPulse(0), direction(Direction::CW), invertDir(invert), stpdrv(stp), tmcConf(TMCStepperConfig)
{}

void Stepper::setup(){
  setupPins();
  tmc2130_init();
}

void Stepper::setupTimers(){
  setupStepperTimer5();
}

  void Stepper::tmc2130_init() {
    stpdrv.begin();
    stpdrv.I_scale_analog(false);
    stpdrv.internal_Rsense(false);
    stpdrv.rms_current(tmcConf.mA, tmcConf.hold_multiplier);
    stpdrv.setSPISpeed(4e6);
    stpdrv.microsteps(tmcConf.microsteps);
    stpdrv.blank_time(tmcConf.blank_time);
    stpdrv.intpol(tmcConf.interpolate); // Interpolate
    stpdrv.dedge(tmcConf.dedge);
    stpdrv.TPOWERDOWN(128); // ~2s until driver lowers to hold current

    if (ESTOP_TYPE == ESTOP_TYPE::BRAKE) {
      stpdrv.stop_enable(true);
    } else {
      stpdrv.stop_enable(false);
    }

    // Could be tuned for stealthchop
    stpdrv.toff(5); // Only enables the driver if used with stealthChop
    stpdrv.hysteresis_start(3);
    stpdrv.hysteresis_end(2);

    if (STEALTHCHOP) {
      stpdrv.en_pwm_mode(true);
      stpdrv.pwm_freq(1); // f_pwm = 2/683 f_clk
      stpdrv.pwm_autoscale(true);
      stpdrv.pwm_grad(5);
      stpdrv.pwm_ampl(255);
      if (HYBRID_THRESHOLD) {
        stpdrv.TPWMTHRS(12650000UL*tmcConf.microsteps/(256*tmcConf.threshold*tmcConf.spmm));
      }
    }
    stpdrv.GSTAT(); // Clear GSTAT
  }

void Stepper::setDirection(Stepper::Direction dir){
    direction = dir;
    if (invertDir ^ (direction == Direction::CW))
        digitalWrite(dirPin, HIGH);
    else
        digitalWrite(dirPin, LOW);
}

void Stepper::changeDirection(){
    if (direction == Direction::CW)
        setDirection(Direction::CCW);
    else
        setDirection(Direction::CW);
}

void Stepper::changeStepper0Direction(){
  TIMSK5 &= ~bit(OCIE5A) & ~bit(TOIE5); // disable interrupt
  TIMSK5 |= bit(OCF5A); // clear interrupt flag
  stpdrv.shaft(invertDir ^ (direction == Direction::CW)); //TODO may have to wait for this to have happened?
  TIFR5 |= bit(OCF5A) | bit(TOV5); // clear flags
  TIMSK5 |= bit(OCIE5A) | bit(TOIE5); // reenable interrupt
}

void Stepper::changeStepper1Direction(){
  TIMSK4 &= ~bit(OCIE4B) & ~bit(TOIE4); // disable interrupt
  TIMSK4 |= bit(OCF4B); // disable interrupt
  stpdrv.shaft(invertDir ^ (direction == Direction::CW)); //TODO may have to wait for this to have happened?
  TIFR4 |= bit(OCF4B) | bit(TOV4); // clear flags
  TIMSK4 |= bit(OCIE4B) | bit(TOIE4); // reenable interrupt
}

void Stepper::setTargetStepperSpeed(double speed){
    targetPulse = uint16_t((speed * STEPS_PER_MM  / ((double)(F_CPU) / STEP_PRESCALER)) - 0.5); // round, and do 0 count offset
    Serial.print("timer freq ");
    Serial.print(uint16_t((  / ((double)(F_CPU) / STEP_PRESCALER)) - 0.5));
    Serial.print(" SPMM: ");
    Serial.print(STEPS_PER_MM);
    Serial.print(" speed: ");
    Serial.println(speed);
    Serial.print("Target pulse: ");
    Serial.println(targetPulse);
}

void Stepper::updateStepperTimer5(Stepper &st){
    OCR5A = st.targetPulse;
    // order matters, High byte write first
    // OCR5AH = targetPulse[0] >> 8;
    // OCR5AL = targetPulse[0] & 0xFF;
}

void Stepper::updateStepperTimer4(Stepper &st){
    OCR4B = st.targetPulse;
    // order matters, High byte write first
    // OCR4BH = targetPulse[1] >> 8;
    // OCR4BL = targetPulse[1] & 0xFF;
}

void Stepper::setupStepperTimer5(){
    TCCR5A = bit(COM5A1);
    TCCR5B = bit(WGM52) | bit(CS52); //| bit(CS40); // CS42, CS41, CS40; // Prescaler 0, 1, 8, 64, 256, 1024, external
    // Force output compare, (for immediate compare match)
    TCCR5C =  bit(FOC5A) | bit(FOC5B) | bit(FOC5C); // CS42, CS41, CS40; // 0, 1, 8, 64, 256, 1024, external
    // interrupt mask (input / output compare channels and timer overflow)
    TIMSK5 = bit(OCIE5A) ;
}

void Stepper::setupStepperTimer4(){
    // TODO This will clash with the PWM pin
    TCCR4A = bit(COM4A1);
    TCCR4B = bit(WGM52) | bit(CS42); //| bit(CS40); // CS42, CS41, CS40; // Prescaler 0, 1, 8, 64, 256, 1024, external
    // Force output compare, (for immediate compare match)
    TCCR4C =  bit(FOC4A) | bit(FOC4B) | bit(FOC4C);
    // interrupt mask (input / output compare channels and timer overflow)
    TIMSK4 = bit(OCIE4B) ;
}

void Stepper::disable(){
    digitalWrite(enablePin, HIGH);
}

void Stepper::enable(){
    digitalWrite(enablePin, LOW);
}

void Stepper::setupPins(){
    // Steppers are active low
    pinMode(stepPin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    disable();
}

uint32_t Stepper::getDrvStatus(){
    return stpdrv.DRV_STATUS();
}

bool Stepper::stallStatus() {
    return stpdrv.stallguard();
}
bool Stepper::isEnabled(){
    return stpdrv.isEnabled();
}
