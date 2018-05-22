#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <ArmServos.h>
#include <ArmServosSpeedControlled.h>
#include <RobotAsciiCom.h>

char manufacturer[] = "Rose-Hulman";
char model[] = "Arm Scripts";
char versionStr[] = "1.0";
        
// Only Manufacturer, Model, and Version matter to Android
AndroidAccessory acc(manufacturer,
                     model,
                     "Basic command set to carry out Arm Scripts",
                     versionStr,
                     "https://sites.google.com/site/me435spring2013/",
                     "12345");
byte rxBuf[255];
// Note, when sending commands to Android I don't add the '\n'.
// Turned out to be easier to just assume the whole message arrives together.
// Seems to work fine.  Just separate into different write() commands.
char rightButtonScript[] = "port_ball";
char leftButtonScript[] = "center_ball";
char selectButtonScript[] = "starboard_ball";

/***  Pin I/O   ***/ 
#define PIN_RIGHT_BUTTON 2
#define PIN_LEFT_BUTTON 3
#define PIN_SELECT_BUTTON 21

/*** Interrupt flags ***/
volatile int mainEventFlags = 0;
#define FLAG_INTERRUPT_0 0x01
#define FLAG_INTERRUPT_1 0x02
#define FLAG_INTERRUPT_2 0x04

LiquidCrystal lcd(14, 15, 16, 17, 18, 19, 20);
#define LINE_1 0
#define LINE_2 1

ArmServosSpeedControlled armServos;
RobotAsciiCom robotCom;

void setup() {
  pinMode(PIN_LEFT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_SELECT_BUTTON, INPUT_PULLUP);
  attachInterrupt(0, int0_isr, FALLING);
  attachInterrupt(1, int1_isr, FALLING);
  attachInterrupt(2, int2_isr, FALLING);
  lcd.begin(16, 2);
  armServos.attach(); // Attach all servos at the start (even gripper).
  robotCom.registerPositionCallback(positionCallback);
  robotCom.registerJointAngleCallback(jointAngleCallback);
  robotCom.registerGripperCallback(gripperCallback);
  robotCom.registerAttachSelectedServosCallback(attachSelectedServosCallback);
  lcd.clear();
  lcd.print("Ready");
  delay(1500);
  acc.powerOn();
}

void positionCallback(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) {
  armServos.setPosition(joint1Angle, joint2Angle, joint3Angle, joint4Angle, joint5Angle);
  lcd.clear();
  lcd.print("Position:");
  lcd.setCursor(0, LINE_2);
  lcd.print(joint1Angle);
  lcd.print(" ");
  lcd.print(joint2Angle);
  lcd.print(" ");
  lcd.print(joint3Angle);
  lcd.print(" ");
  lcd.print(joint4Angle);
  lcd.print(" ");
  lcd.print(joint5Angle);
}

void jointAngleCallback(byte jointNumber, int jointAngle) {
  armServos.setJointAngle(jointNumber, jointAngle);
  lcd.clear();
  lcd.print("Joint angle:");
  lcd.setCursor(0, LINE_2);
  lcd.print("J");
  lcd.print(jointNumber);
  lcd.print(" move to ");
  lcd.print(jointAngle);
}

void gripperCallback(int gripperDistance) {
  if (gripperDistance < 10) {
    gripperDistance = 10;
  }
  armServos.setGripperDistance(gripperDistance);
  lcd.clear();
  lcd.print("Gripper:");
  lcd.setCursor(0, LINE_2);
  lcd.print("Gripper to ");
  lcd.print(gripperDistance);
}   

void attachSelectedServosCallback(byte servosToEnable) {
  armServos.attachSelectedServos(servosToEnable);
  lcd.clear();
  lcd.print("Attach:");
  lcd.setCursor(0, LINE_2);
  lcd.print("54321G = ");
  lcd.print(servosToEnable, BIN);
}

void loop() {
  // See if there is a new message from Android.
  if (acc.isConnected()) {
    int len = acc.read(rxBuf, sizeof(rxBuf), 1);
    if (len > 0) {
      robotCom.handleRxBytes(rxBuf, len);
    }
    if (mainEventFlags & FLAG_INTERRUPT_0) {
      delay(20);
      mainEventFlags &= ~FLAG_INTERRUPT_0;
      if (!digitalRead(PIN_RIGHT_BUTTON)) {
        acc.write(rightButtonScript, sizeof(rightButtonScript));
      }
    }
    if (mainEventFlags & FLAG_INTERRUPT_1) {
      delay(20);
      mainEventFlags &= ~FLAG_INTERRUPT_1;
      if (!digitalRead(PIN_LEFT_BUTTON)) {
        acc.write(leftButtonScript, sizeof(leftButtonScript));
      }
    }
    if (mainEventFlags & FLAG_INTERRUPT_2) {
      delay(20);
      mainEventFlags &= ~FLAG_INTERRUPT_2;
      if (!digitalRead(PIN_SELECT_BUTTON)) {
        acc.write(selectButtonScript, sizeof(selectButtonScript));
      }
    }    
  }
  armServos.updateServos();
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
