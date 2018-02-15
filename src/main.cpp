#include "Stepper.h"
#include <Homie.h>

#define FW_NAME "heating-valve-firmware"
#define FW_VERSION "1.0.0"

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

const int MOTOR_PINS[] = {D4, D5, D2, D1};
const int REVOLUTION_STEPS = 2048; // One turn
const int CYCLE_STEPS = (REVOLUTION_STEPS * 1) + (REVOLUTION_STEPS * 0.5); // One turn and a half
const int INIT_CYCLE_STEPS = CYCLE_STEPS + REVOLUTION_STEPS; // 2 turns and a half

int currentLevel = -1;

Stepper motor(4096, MOTOR_PINS[0], MOTOR_PINS[2], MOTOR_PINS[1], MOTOR_PINS[3]);
HomieNode valveNode("valve", "");

void resetMotorPins() {
  for (int i = 0; i < 4; ++i) {
    digitalWrite(MOTOR_PINS[i], LOW);
  }
}

void setLevel(int level) {
  int targetLevel = 0;
  int steps = 0;

  if (currentLevel == -1 || level < 0 || level > 100) {
    return;
  }

  targetLevel = currentLevel - level;
  steps = (int) ((CYCLE_STEPS * targetLevel) / 100);
  Serial.print("Move motor by ");
  Serial.print(steps);
  Serial.print(" steps (");
  Serial.print(level);
  Serial.println("%)...");
  motor.step(steps);
  currentLevel = level;
  Homie.setNodeProperty(valveNode, "level", String(level), true);
  resetMotorPins();
}

bool valveStateHandler(String level) {
  setLevel(level.toInt());
  return true;
}

void initMotor() {
  Serial.println("Init motor position...");
  motor.step(INIT_CYCLE_STEPS);
  currentLevel = 0;
  setLevel(100);
}

void setupHandler() {
  Homie.setNodeProperty(valveNode, "unit", "%", true);
}

void setup() {
  motor.setSpeed(4);
  initMotor();

  Homie.setFirmware(FW_NAME, FW_VERSION);
  valveNode.subscribe("level", valveStateHandler);
  Homie.registerNode(valveNode);
  Homie.setSetupFunction(setupHandler);
  Homie.setup();
}

void loop() {
  Homie.loop();
}
