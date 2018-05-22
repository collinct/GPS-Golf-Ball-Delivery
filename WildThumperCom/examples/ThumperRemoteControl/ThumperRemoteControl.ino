/**
 * This code runs on the ADK board to send signals to the Wild Thumper controller.
 */

#include <WildThumperCom.h>
#include <LiquidCrystal.h>

#define TEAM_NUMBER 4   // Change as appropriate for your team.

#define WHEEL_MODE_REVERSE 0
#define WHEEL_MODE_BRAKE   1
#define WHEEL_MODE_FORWARD 2
#define CENTER_DEADBAND 5
#define CHANGE_THRESHOLD 5

#define PIN_HORZ_ANALOG 0
#define PIN_VERT_ANALOG 1

WildThumperCom wtc(TEAM_NUMBER);
int lastSentVertReading = 0;
int lastSentHorzReading = 0;
byte leftMode, rightMode, leftDutyCycle, rightDutyCycle;
int initialHorzReading, initialVertReading;

LiquidCrystal lcd(14, 15, 16, 17, 18, 19, 20);
#define LINE_1 0
#define LINE_2 1

void setup() {
  Serial.begin(9600);
  initialHorzReading = analogRead(PIN_HORZ_ANALOG) - 512;
  initialVertReading = analogRead(PIN_VERT_ANALOG) - 512;
  lcd.begin(16, 2);
  lcd.clear();
  lcd.home();
  lcd.print("Ready to drive");
}

void loop() {
  // Read both analog sensors
  int horzReading = analogRead(PIN_HORZ_ANALOG) - 512 - initialHorzReading;
  int vertReading = analogRead(PIN_VERT_ANALOG) - 512 - initialVertReading;
  // Make near center readings exactly center
  if (abs(vertReading) < CENTER_DEADBAND) {
    vertReading = 0;
  }
  if (abs(horzReading) < CENTER_DEADBAND) {
    horzReading = 0;
  }
  // Determine if the change is worth mentioning to the thumper.
  if (abs(vertReading - lastSentVertReading) > CHANGE_THRESHOLD ||
      abs(horzReading - lastSentHorzReading) > CHANGE_THRESHOLD) {
      lastSentVertReading = vertReading;
      lastSentHorzReading = horzReading;
      int leftMotor = (vertReading - horzReading) / 2;
      int rightMotor = (vertReading + horzReading) / 2;
      if (abs(leftMotor) < CENTER_DEADBAND) {
        leftMode = WHEEL_MODE_BRAKE;
      } else if (leftMotor > 0) {
        leftMode = WHEEL_MODE_FORWARD;
      } else {
        leftMode = WHEEL_MODE_REVERSE;
      }
      if (abs(rightMotor) < CENTER_DEADBAND) {
        rightMode = WHEEL_MODE_BRAKE;
      } else if (rightMotor > 0) {
        rightMode = WHEEL_MODE_FORWARD;
      } else {
        rightMode = WHEEL_MODE_REVERSE;
      }
      leftDutyCycle = min(abs(leftMotor), 255);
      rightDutyCycle = min(abs(rightMotor), 255);
      wtc.sendWheelSpeed(leftMode, rightMode, leftDutyCycle, rightDutyCycle);
      // Print the value on the LCD for user feedback.
      displayCommandOnLcd(leftMode, rightMode, leftDutyCycle, rightDutyCycle);
  }
  delay(100);  
}

void displayCommandOnLcd(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) {
  lcd.clear();
  lcd.home();
  switch (leftMode) {
    case WHEEL_MODE_REVERSE:
      lcd.print("Reverse");
      break;
    case WHEEL_MODE_BRAKE:
      lcd.print("Brake");
      break;
    case WHEEL_MODE_FORWARD:
      lcd.print("Forward");
      break;
  }
  lcd.setCursor(0, LINE_2);
  lcd.print(leftDutyCycle);
  lcd.setCursor(9, LINE_1);
  switch (rightMode) {
    case WHEEL_MODE_REVERSE:
      lcd.print("Reverse");
      break;
    case WHEEL_MODE_BRAKE:
      lcd.print("  Brake");
      break;
    case WHEEL_MODE_FORWARD:
      lcd.print("Forward");
      break;
  }
  lcd.setCursor(13, LINE_2);
  if (rightDutyCycle < 100) {
    lcd.print(" ");
  }
  if (rightDutyCycle < 10) {
    lcd.print(" ");
  }
  lcd.print(rightDutyCycle);
}
