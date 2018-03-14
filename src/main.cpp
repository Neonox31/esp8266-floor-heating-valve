#include <Stepper.h>
#include <Homie.h>

#define FW_NAME "heating-valve-firmware"
#define FW_VERSION "1.0.0"

const int MOTOR_PINS[] = {D4, D5, D2, D1};
const int MOTOR_SPEED = 4;
const int MOTOR_REVOLUTION_STEPS = 2048;

const int VALVE_CYCLE_STEPS = (MOTOR_REVOLUTION_STEPS * 1.5);

int currentLevel = -1;
int targetLevel = -1;

Stepper motor(4096, MOTOR_PINS[0], MOTOR_PINS[2], MOTOR_PINS[1], MOTOR_PINS[3]);
HomieNode valveNode("valve", "valve");

void resetMotorPins() {
  for (int i = 0; i < 4; ++i) {
    digitalWrite(MOTOR_PINS[i], LOW);
  }
}

void initMotor() {
  Homie.getLogger() << "init motor speed and position..." << endl;
  motor.setSpeed(MOTOR_SPEED);
  motor.step(VALVE_CYCLE_STEPS + MOTOR_REVOLUTION_STEPS);
  currentLevel = 0;
  targetLevel = 100;
}

void setLevel(int level) {
  int steps = 0;

  if (level != currentLevel) {
    steps = (int) ((VALVE_CYCLE_STEPS * (currentLevel - level)) / 100);
    Homie.getLogger() << "move motor by " << steps << " steps (" << level << "%)..." << endl;
    motor.step(steps);
    resetMotorPins();
    currentLevel = level;
    valveNode.setProperty("level").send(String(currentLevel));
  }
}

bool valveLevelHandler(const HomieRange& range, const String& value) {
  if (value.toInt() < 0 || value.toInt() > 100) {
    return false;
  }
  targetLevel = value.toInt();
  return true;
}

void setupHandler() {
  initMotor();
  valveNode.setProperty("unit").send("%");
}

void loopHandler() {
  setLevel(targetLevel);
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  
  valveNode.advertise("unit");
  valveNode.advertise("level").settable(valveLevelHandler);

  Homie.setup();
}

void loop() {
  Homie.loop();
}
