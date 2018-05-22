/**
 * This program is used to set the team number on your Wild Thumper.
 * The team number on the Wild Thumper must match the team number sent
 * by the ADK board.  This is primarially important when using XBee
 * for wireless communication, but the team number must match even
 * if you are wired directly (since it's in the code due to wireless).
 *
 * Instructions for using this program:
 * Connect both Rx and Tx to the Wild Thumper and turn on battery power.
 * The LCD screen on the ADK board should display the desired team number
 * as well as the most recently received team number from Wild Thumper.
 *   Analog joystick up and down to change the desired team number.
 *   Right pushbutton - Request (without changing) the current team number.
 *   Left pushbutton - Change the team number to the desired team number.
 *      (note that the A1 row must have a jumper to change team number).
 *   Joystick press (select button) - Clear the LCD value that displays the
 *      most recent team number received (handy to make sure it's fresh data). 
 */

#include <LiquidCrystal.h>
#include <WildThumperCom.h>
#include <TimerEvent.h>
#include <TimerEventScheduler.h>

#define TEAM_NUMBER 4  // Replace this with your team number if you like.
// This is the initial Desired Team Number.
// This is also a value sent within messages, but not important here.

/***  Pin I/O   ***/ 
#define PIN_RIGHT_BUTTON 2
#define PIN_LEFT_BUTTON 3
#define PIN_SELECT_BUTTON 21
#define PIN_HORZ_ANALOG 0
#define PIN_VERT_ANALOG 1

#define LOW_THRESHOLD 100
#define HIGH_THRESHOLD 900

/*** Interrupt flags ***/
volatile int mainEventFlags = 0;
#define FLAG_RIGHT_BUTTON                  0x0001
#define FLAG_LEFT_BUTTON                   0x0002
#define FLAG_SELECT_BUTTON                 0x0004

TimerEventScheduler tes;
// Create timer events that are initially disabled (call in 0 ms = never)
TimerEvent rightButtonDebounceTimerEvent(rightButtonDebounceCallback, 0);
TimerEvent leftButtonDebounceTimerEvent(leftButtonDebounceCallback, 0);
TimerEvent selectButtonDebounceTimerEvent(selectButtonDebounceCallback, 0);

LiquidCrystal lcd(14, 15, 16, 17, 18, 19, 20);
#define LINE_1 0
#define LINE_2 1

WildThumperCom wildThumperCom(TEAM_NUMBER);
byte desiredTeamNumber = TEAM_NUMBER;
byte wildThumperComTeamNumber = TEAM_NUMBER;
boolean thumperIsMoving = false;

void setup() {
  Serial.begin(9600);  // Change if you are using a different baudrate.
  pinMode(PIN_LEFT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_SELECT_BUTTON, INPUT_PULLUP);
  attachInterrupt(0, int0_isr, FALLING);
  attachInterrupt(1, int1_isr, FALLING);
  attachInterrupt(2, int2_isr, FALLING);
  tes.addTimerEvent(&rightButtonDebounceTimerEvent);
  tes.addTimerEvent(&leftButtonDebounceTimerEvent);
  tes.addTimerEvent(&selectButtonDebounceTimerEvent);
  lcd.begin(16, 2);
    
  // Register callbacks for commands you might receive from the Wild Thumper.
  wildThumperCom.registerTeamNumberReplyCallback(teamNumberReplyFromThumper);
  
  lcd.clear();
  lcd.home();
  lcd.print("Desired   Set as");
  lcd.setCursor(0, LINE_2);
  lcd.print(desiredTeamNumber);
  lcd.setCursor(13, LINE_2);
  lcd.print("?? ");
}

void teamNumberReplyFromThumper(byte teamNumber) {
  // Display the team number on the LCD.
  lcd.setCursor(13, LINE_2);
  lcd.print(teamNumber);
  lcd.print("  ");
}

void loop() {
  // Note, this uses a fancier button debounce mechanism.
  if (mainEventFlags & FLAG_RIGHT_BUTTON) {
    mainEventFlags &= ~FLAG_RIGHT_BUTTON;
    // Request the current team number on the Thumper.
    wildThumperCom.sendTeamNumberRequest();
  }
  if (mainEventFlags & FLAG_LEFT_BUTTON) {
    mainEventFlags &= ~FLAG_LEFT_BUTTON;
    // Change the team number on the Thumper.
    wildThumperCom.sendTeamNumberChangeRequest(desiredTeamNumber);
    
  }
  if (mainEventFlags & FLAG_SELECT_BUTTON) {
    mainEventFlags &= ~FLAG_SELECT_BUTTON;
    // Clear the displayed current team number line.
    lcd.setCursor(13, LINE_2);
    lcd.print("-- ");
  }
  // Change the desired team number.
  int vertReading = analogRead(PIN_VERT_ANALOG);
  if (vertReading < LOW_THRESHOLD) {
    desiredTeamNumber--;
  } else if (vertReading > HIGH_THRESHOLD) {
    desiredTeamNumber++;
  }
  if (desiredTeamNumber != wildThumperComTeamNumber) {
    wildThumperCom.setTeamNumber(desiredTeamNumber);
    wildThumperComTeamNumber = desiredTeamNumber;
  }
  // Move the wheels as a test.
  int horzReading = analogRead(PIN_HORZ_ANALOG);
  if (horzReading < LOW_THRESHOLD) {
    // Send a command to go in reverse.
    if (!thumperIsMoving) {
      wildThumperCom.sendWheelSpeed(WHEEL_SPEED_MODE_REVERSE, WHEEL_SPEED_MODE_REVERSE, 125, 125);
      thumperIsMoving = true;
    }
  } else if (horzReading > HIGH_THRESHOLD) {
    // Send a command to go forward.
    if (!thumperIsMoving) {
      wildThumperCom.sendWheelSpeed(WHEEL_SPEED_MODE_FORWARD, WHEEL_SPEED_MODE_FORWARD, 125, 125);
      thumperIsMoving = true;
    }
  } else {
    // Stop the Thumper.
    if (thumperIsMoving) {
      wildThumperCom.sendWheelSpeed(WHEEL_SPEED_MODE_BRAKE, WHEEL_SPEED_MODE_BRAKE, 0, 0);
      thumperIsMoving = false;
    }
  }
  // Update the display every 150 milliseconds.
  delay(150);
  lcd.setCursor(0, LINE_2);
  lcd.print(desiredTeamNumber);
  lcd.print("  ");
}

/** Send all bytes received to the Wild Thumper Com object. */
void serialEvent() {
  while (Serial.available()) {
    wildThumperCom.handleRxByte(Serial.read());
  }
}


// Fancier debounce version using TES
void int0_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  rightButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

int rightButtonDebounceCallback() {
  if (!digitalRead(PIN_RIGHT_BUTTON)) {
    mainEventFlags |= FLAG_RIGHT_BUTTON;
  }
  return 0; // Don't call this Timer Event again (0 is off).
}

void int1_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  leftButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

int leftButtonDebounceCallback() {
  if (!digitalRead(PIN_LEFT_BUTTON)) {
    mainEventFlags |= FLAG_LEFT_BUTTON;
  }
  return 0;  // Don't call this Timer Event again (0 is off).
}

void int2_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  selectButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

int selectButtonDebounceCallback() {
  if (!digitalRead(PIN_SELECT_BUTTON)) {
    mainEventFlags |= FLAG_SELECT_BUTTON;
  }
  return 0;  // Don't call this Timer Event again (0 is off).
}
