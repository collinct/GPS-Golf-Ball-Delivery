#include <LiquidCrystal.h>
#include <Servo.h>
#include <ArmServos.h>

ArmServos robotArm;

/***  Pin I/O   ***/ 
#define PIN_LED_1 64
#define PIN_LED_2 65
#define PIN_LED_3 66
#define PIN_LED_4 67
#define PIN_LED_5 68
#define PIN_LED_6 69
#define PIN_RIGHT_BUTTON 2
#define PIN_LEFT_BUTTON 3
#define PIN_SELECT_BUTTON 21
#define PIN_CONTRAST_ANALOG 8
#define PIN_HORZ_ANALOG 0
#define PIN_VERT_ANALOG 1

/*** Interrupt flags ***/
volatile int mainEventFlags = 0;
#define FLAG_INTERRUPT_0 0x01
#define FLAG_INTERRUPT_1 0x02
#define FLAG_INTERRUPT_2 0x04

LiquidCrystal lcd(14, 15, 16, 17, 18, 19, 20);
#define LINE_1 0
#define LINE_2 1

int servoAngles[6];
#define RESET_JOINT_1_ANGLE 0
#define RESET_JOINT_2_ANGLE 90
#define RESET_JOINT_3_ANGLE 0
#define RESET_JOINT_4_ANGLE -90
#define RESET_JOINT_5_ANGLE 90
#define RESET_GRIPPER_DISTANCE 50
  
#define LOW_THRESHOLD 100
#define HIGH_THRESHOLD 900
#define VERT_MOVE_AMOUNT 4
#define HORZ_MOVE_AMOUNT 1

int currentJoint = 0;
#define MIN_CURRENT_JOINT 0
#define MAX_CURRENT_JOINT 5

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_LED_4, OUTPUT);
  pinMode(PIN_LED_5, OUTPUT);
  pinMode(PIN_LED_6, OUTPUT);
  digitalWrite(PIN_LED_1, LOW);
  digitalWrite(PIN_LED_2, LOW);
  digitalWrite(PIN_LED_3, LOW);
  digitalWrite(PIN_LED_4, LOW);
  digitalWrite(PIN_LED_5, LOW);
  digitalWrite(PIN_LED_6, LOW);
  pinMode(PIN_LEFT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_SELECT_BUTTON, INPUT_PULLUP);
  attachInterrupt(0, int0_isr, FALLING);
  attachInterrupt(1, int1_isr, FALLING);
  attachInterrupt(2, int2_isr, FALLING);
  lcd.begin(16, 2);
  servoAngles[1] = RESET_JOINT_1_ANGLE;
  servoAngles[2] = RESET_JOINT_2_ANGLE;
  servoAngles[3] = RESET_JOINT_3_ANGLE;
  servoAngles[4] = RESET_JOINT_4_ANGLE;
  servoAngles[5] = RESET_JOINT_5_ANGLE;
  servoAngles[0] = RESET_GRIPPER_DISTANCE;
  robotArm.attach();
  updateServos();
}

void loop() {
  int horzReading = analogRead(PIN_HORZ_ANALOG);
  int vertReading = analogRead(PIN_VERT_ANALOG);
  if (mainEventFlags & FLAG_INTERRUPT_0) {
    delay(20);
    mainEventFlags &= ~FLAG_INTERRUPT_0;
    if (!digitalRead(PIN_RIGHT_BUTTON)) {
      currentJoint++;
      if (currentJoint > MAX_CURRENT_JOINT) {
        currentJoint = MIN_CURRENT_JOINT;
      }
    }
  }
  if (mainEventFlags & FLAG_INTERRUPT_1) {
    delay(20);
    mainEventFlags &= ~FLAG_INTERRUPT_1;
    if (!digitalRead(PIN_LEFT_BUTTON)) {
      currentJoint--;
      if (currentJoint < MIN_CURRENT_JOINT) {
        currentJoint = MAX_CURRENT_JOINT;
      }
    }
  }
  if (mainEventFlags & FLAG_INTERRUPT_2) {
    delay(20);
    mainEventFlags &= ~FLAG_INTERRUPT_2;
    if (!digitalRead(PIN_SELECT_BUTTON)) {
      switch(currentJoint) {
        case 0:
          servoAngles[0] = RESET_GRIPPER_DISTANCE;
          break;
        case 1:
          servoAngles[1] = RESET_JOINT_1_ANGLE;
          break;
        case 2:
          servoAngles[2] = RESET_JOINT_2_ANGLE;
          break;
        case 3:
          servoAngles[3] = RESET_JOINT_3_ANGLE;
          break;
        case 4:
          servoAngles[4] = RESET_JOINT_4_ANGLE;
          break;
        case 5:
          servoAngles[5] = RESET_JOINT_5_ANGLE;
          break;
      }
    }
  }
  updateLeds();
  
  if (horzReading < LOW_THRESHOLD) {
    servoAngles[currentJoint] -= HORZ_MOVE_AMOUNT;
  } else if (horzReading > HIGH_THRESHOLD) {
    servoAngles[currentJoint] += HORZ_MOVE_AMOUNT;
  }
  if (vertReading < LOW_THRESHOLD) {
    servoAngles[currentJoint] -= VERT_MOVE_AMOUNT;
  } else if (vertReading > HIGH_THRESHOLD) {
    servoAngles[currentJoint] += VERT_MOVE_AMOUNT;
  }
  // Limit each joint using the DH angle limits.
  servoAngles[1] = constrain(servoAngles[1], -90, 90);
  servoAngles[2] = constrain(servoAngles[2], 0, 180);
  servoAngles[3] = constrain(servoAngles[3], -90, 90);
  servoAngles[4] = constrain(servoAngles[4], -180, 0);
  servoAngles[5] = constrain(servoAngles[5], 0, 180);
  servoAngles[0] = constrain(servoAngles[0], 0, 71);
  
  updateLcd();
  updateServos();
  delay(100);
}

void updateLeds() {
  digitalWrite(PIN_LED_1, LOW);
  digitalWrite(PIN_LED_2, LOW);
  digitalWrite(PIN_LED_3, LOW);
  digitalWrite(PIN_LED_4, LOW);
  digitalWrite(PIN_LED_5, LOW);
  digitalWrite(PIN_LED_6, LOW);
  switch(currentJoint) {
    case 0:
      digitalWrite(PIN_LED_6, HIGH);
      break;
    case 1:
      digitalWrite(PIN_LED_1, HIGH);
      break;
    case 2:
      digitalWrite(PIN_LED_2, HIGH);
      break;
    case 3:
      digitalWrite(PIN_LED_3, HIGH);
      break;
    case 4:
      digitalWrite(PIN_LED_4, HIGH);
      break;
    case 5:
      digitalWrite(PIN_LED_5, HIGH);
      break;
  }
}

void updateLcd() {
  lcd.setCursor(0, LINE_1);
  lcd.print(servoAngles[1]);
  lcd.print("   ");
  lcd.setCursor(5, LINE_1);
  lcd.print(servoAngles[2]);
  lcd.print("   ");
  lcd.setCursor(10, LINE_1);
  lcd.print(servoAngles[3]);
  lcd.print("   ");
  lcd.setCursor(0, LINE_2);
  lcd.print(servoAngles[4]);
  lcd.print("   ");
  lcd.setCursor(5, LINE_2);
  lcd.print(servoAngles[5]);
  lcd.print("   ");
  lcd.setCursor(10, LINE_2);
  lcd.print(servoAngles[0]);
  lcd.print("   ");
}

void updateServos() {
  robotArm.setPosition(servoAngles[1],
      servoAngles[2],
      servoAngles[3],
      servoAngles[4],
      servoAngles[5]);
  // Could've also used individual joint angle commands.
//  robotArm.setJointAngle(1, servoAngles[1]);
//  robotArm.setJointAngle(2, servoAngles[2]);
//  robotArm.setJointAngle(3, servoAngles[3]);
//  robotArm.setJointAngle(4, servoAngles[4]);
//  robotArm.setJointAngle(5, servoAngles[5]);
  robotArm.setGripperDistance(servoAngles[0]);
}
  
void int0_isr() {
  mainEventFlags |= FLAG_INTERRUPT_0;
}
void int1_isr() {
  mainEventFlags |= FLAG_INTERRUPT_1;
}
void int2_isr() {
  mainEventFlags |= FLAG_INTERRUPT_2;
}