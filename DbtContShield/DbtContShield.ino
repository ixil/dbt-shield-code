#include "pins.h"
#include "controller.h"
#include "serialcommand.h"
#include "steppers.h"
#include "Thermistor.h"
#include "pid.h"
#include "interrupts.h"


#define TEMP_MIN_THRESHOLD 100
#define SETPOINT_THRESHOLD 8

TMC2130Stepper extruderDriver(EXTRUDER_CS, STEPPER_R_SENSE, 0);
Stepper* extruderInstance = new Stepper(EXTRUDER_EN, EXTRUDER_STP, EXTRUDER_DIR, extruderDriver, false);
Stepper extruderStepper = *extruderInstance;

// ADC0 is pin 0
Thermistor thermistor(0);

bool motorsEnabled=false;
double temperature;
double setPoint, outputVal;
bool heaterOn;
double extruderTargetSpeed = 1;
bool speedUpdate;
bool statusCheck=false;
 

ISR(ADC_vect){ (*InterruptGlobals::ADCInterrupt)(); }

ISR(TIMER5_COMPA_vect){
  Stepper::Stepper0ISR();
}

using namespace Controller;

void printStatus(){
  Serial.print("Inputs");
  Serial.println(Controller::readOptoInputs(), BIN);
  Serial.print(" ExtruderTargetSpeed set to: ");
  Serial.print(extruderTargetSpeed);
  Serial.println("mm/s");
  Serial.print(" Heaters : ");
  Serial.print(heaterOn);
  Serial.print(" Motors: ");
  Serial.print(motorsEnabled);
  Serial.print(" speeds: ");
  extruderStepper.printStatus();
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  while(!Serial);
  Serial.println("\nStart...");
  Serial.flush();
  // put your setup code here, to run once:
  cli();
  setupPinModes();
  setupKillPins();

  thermistor.init();
  extruderStepper.setup(); // after SPI
  initPID();

  /* setupControlTimers(); */
  // this is a hack to make the interrupts slow
  extruderStepper.setTargetStepperSpeed(0);
  extruderStepper.setupTimers();
  Controller::enableExtruder();
  extruderStepper.setTargetStepperSpeed(0);
  extruderStepper.disable();
  sei();
}

void errorCondition(){
  //heaterOn = false;
  extruderStepper.setTargetStepperSpeed(0);
  extruderStepper.disable();
  Serial.println("Error condition, Disabling");
}

void pollThermistor(){
  thermistor.run();
  TemperatureStatus tstat = thermistor.readTemperature(temperature);
  switch (tstat) {
    case TemperatureStatus::NotReady:
      break;
    case TemperatureStatus::OpenCircuit:
      Serial.print(temperature); Serial.println("C.");
      Serial.print(" Open circuit!");
      [[fallthrough]]
    case TemperatureStatus::ShortCircuit:
        //FIXME renable
      /* Serial.print(temperature); Serial.println("C."); */
      /* Serial.print(" Short circuit!"); */
      [[fallthrough]]
    case TemperatureStatus::Error:
      // errorCondition(); //TODO uncomment once finished testing
      break;
    default:
      break;
  }
}

void poll(){
  pollThermistor();
  pollControlPins();
  motorsEnable &= !(temperature < TEMP_MIN_THRESHOLD);
  motorsEnabled &= myPID.atSetPoint(SETPOINT_THRESHOLD);
  //FIXME only update on a change
  if (motorsEnabled){
    /* Serial.println("e enabled"); */
    extruderStepper.enable();
  }
  else{
    Serial.println("e disabled");
    extruderStepper.disable();
  }
  if (speedUpdate){
    extruderStepper.setTargetStepperSpeed(extruderTargetSpeed);
    Serial.print("updated speed to: ");
    Serial.println(extruderTargetSpeed);
    speedUpdate=false;
  }
  if (statusCheck){
    printStatus();
    statusCheck=false;
  }
  /* Serial.print("Target speed is currently: "); */
  /* Serial.println(extruderTargetSpeed); */
}

void loop() {
  poll();
  runPID();
  //Serial.println(temperature);
  processCom(); // Process the serial commands
  //digitalWrite(6, HIGH);
  Serial.flush();
}

