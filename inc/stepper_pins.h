#ifndef STEPPER_PINS_H
#define STEPPER_PINS_H

#define STEPPER_PIN( PIN_NAME, PIN_N ) define STEP ## PIN_NAME (PIN_N)
#ifndef MISO
#define MISO 52
#endif
#ifndef MOSI
#define MOSI 51
#endif
#ifndef SCK
#define SCK 52
#endif

#ifndef STEP0_CS
#define STEP0_CS D13
#endif
#ifndef STEP1_CS
#define STEP1_CS D11
#endif
#ifndef STEP0_EN
#define STEP0_EN D12
#endif
#ifndef STEP1_EN
#define STEP1_EN D10
#endif

// Defines STEPX_YY value
STEPPER_PIN(0_SO, MISO)
STEPPER_PIN(0_SI, MOSI)
STEPPER_PIN(0_CK, SCK)
STEPPER_PIN(0_CS, STEP0_CS)
STEPPER_PIN(0_EN, STEP0_EN)

STEPPER_PIN(1_SO, MISO)
STEPPER_PIN(1_SI, MOSI)
STEPPER_PIN(1_CK, SCK)
STEPPER_PIN(1_CS, STEP1_CS)
STEPPER_PIN(1_EN, STEP1_EN)

#endif /* STEPPER_PINS_H */
