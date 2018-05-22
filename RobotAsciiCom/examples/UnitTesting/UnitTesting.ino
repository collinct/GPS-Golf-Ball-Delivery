#include <RobotAsciiCom.h>

RobotAsciiCom robotCom;

void setup() {
  Serial.begin(9600);
  delay(100);
  robotCom.registerPositionCallback(positionCallback);
  robotCom.registerJointAngleCallback(jointAngleCallback);
  robotCom.registerGripperCallback(gripperCallback);
  robotCom.registerBatteryVoltageRequestCallback(batteryVoltageRequestCallback);
  robotCom.registerWheelSpeedCallback(wheelSpeedCallback);
  robotCom.registerCustomStringCallback(customStringCallback);
  robotCom.registerAttachSelectedServosCallback(attachSelectedServosCallback);
}

void positionCallback(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) {
  Serial.println();
  Serial.print("Position command: ");
  Serial.print(joint1Angle);
  Serial.print(" ");
  Serial.print(joint2Angle);
  Serial.print(" ");
  Serial.print(joint3Angle);
  Serial.print(" ");
  Serial.print(joint4Angle);
  Serial.print(" ");
  Serial.print(joint5Angle);
  Serial.println();
}

void jointAngleCallback(byte jointNumber, int jointAngle) {
  Serial.println();
  Serial.print("Joint angle command for joint ");
  Serial.print(jointNumber);
  Serial.print(" to move to ");
  Serial.print(jointAngle);
  Serial.println();
}

void gripperCallback(int gripperDistance) {
  Serial.println();
  Serial.print("Gripper command to ");
  Serial.print(gripperDistance);
  Serial.println();

}

void batteryVoltageRequestCallback(void) {
  Serial.println();
  Serial.print("Battery voltage request");
  Serial.println();

}

void wheelSpeedCallback(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) {
  Serial.println();
  Serial.print("Wheel speed command.  ");
  switch (leftMode) {
    case WHEEL_SPEED_MODE_REVERSE:
      Serial.print("Left Reverse ");
      break;
    case WHEEL_SPEED_MODE_BRAKE:
      Serial.print("Left Brake ");
      break;
    case WHEEL_SPEED_MODE_FORWARD:
      Serial.print("Left Forward ");
      break;
  }
  Serial.print(leftDutyCycle);
  switch (rightMode) {
    case WHEEL_SPEED_MODE_REVERSE:
      Serial.print(" Right Reverse ");
      break;
    case WHEEL_SPEED_MODE_BRAKE:
      Serial.print(" Right Brake ");
      break;
    case WHEEL_SPEED_MODE_FORWARD:
      Serial.print(" Right Forward ");
      break;
  }
  Serial.print(rightDutyCycle);  
  Serial.println();
}

void customStringCallback(String customString) {
  Serial.print(customString);  
  Serial.println();
}

void attachSelectedServosCallback(byte servosToEnable) {
  Serial.print("Attach: ");  
  Serial.print(servosToEnable, BIN);  
  Serial.println();
}


void loop() {
  
  Serial.print("Test 1: ");
  test1();
  Serial.println("");
  Serial.print("Test 2: ");
  test2();
  Serial.println("");
  Serial.print("Test 3: ");
  test3();
  Serial.println("");
  Serial.print("Test 4: ");
  test4();
  Serial.println("");
  Serial.print("Test 5: ");
  test5();
  Serial.println("");
  Serial.print("Test 6: ");
  test6();
  Serial.println("");
  Serial.println("Test 7: ");
  test7();
  Serial.println("");
  Serial.println("Test 8: ");
  test8();
  Serial.println("");
  Serial.println("Test 9: ");
  test9();
  Serial.println("");
  // Output should be... (ignoring line breaks)
  // Test 1: Gripper command to 42
  // Test 2: Position command: 10 20 30 40 50
  // Test 3: Battery voltage request
  // Test 4: Joint angle command for joint 3 to move to 33
  // Test 5: Wheel speed command.  Left Forward 120 Right Brake 4
  // Test 6: Joint angle command for joint 1 to move to -88
  // Test 7: Hello, World!
  // Test 8: 
  // Test 9: Attach: 111110
  
  while(1);
}

void test1() {
  robotCom.handleRxByte('G');
  robotCom.handleRxByte('R');
  robotCom.handleRxByte('I');
  robotCom.handleRxByte('P');
  robotCom.handleRxByte('P');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('R');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('4');
  robotCom.handleRxByte('2');
  robotCom.handleRxByte('\n');
}

void test2() {
  robotCom.handleRxByte('P');
  robotCom.handleRxByte('O');
  robotCom.handleRxByte('S');
  robotCom.handleRxByte('I');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte('I');
  robotCom.handleRxByte('O');
  robotCom.handleRxByte('N');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('1');
  robotCom.handleRxByte('0');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('2');
  robotCom.handleRxByte('0');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('3');
  robotCom.handleRxByte('0');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('4');
  robotCom.handleRxByte('0');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('5');
  robotCom.handleRxByte('0');
  robotCom.handleRxByte('\n');
}

void test3() {
  robotCom.handleRxByte('B');
  robotCom.handleRxByte('A');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('R');
  robotCom.handleRxByte('Y');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('V');
  robotCom.handleRxByte('O');
  robotCom.handleRxByte('L');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte('A');
  robotCom.handleRxByte('G');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('R');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('Q');
  robotCom.handleRxByte('U');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('S');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte('\n');
}

void test4() {
  robotCom.handleRxByte('J');
  robotCom.handleRxByte('O');
  robotCom.handleRxByte('I');
  robotCom.handleRxByte('N');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('3');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('A');
  robotCom.handleRxByte('N');
  robotCom.handleRxByte('G');
  robotCom.handleRxByte('L');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('3');
  robotCom.handleRxByte('3');
  robotCom.handleRxByte('\n');
}

void test5() {
  robotCom.handleRxByte('W');
  robotCom.handleRxByte('H');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('L');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('S');
  robotCom.handleRxByte('P');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('D');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('F');
  robotCom.handleRxByte('O');
  robotCom.handleRxByte('R');
  robotCom.handleRxByte('W');
  robotCom.handleRxByte('A');
  robotCom.handleRxByte('R');
  robotCom.handleRxByte('D');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('1');
  robotCom.handleRxByte('2');
  robotCom.handleRxByte('0');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('B');
  robotCom.handleRxByte('R');
  robotCom.handleRxByte('A');
  robotCom.handleRxByte('K');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('4');
  robotCom.handleRxByte('\n');
}

void test6() {
  byte testArray[] = "JOINT 1 ANGLE -88 ";
  testArray[17] = '\n';
  robotCom.handleRxBytes(testArray, 18);
}

void test7() {
  byte testArray[] = "CUSTOM Hello, World! ";
  testArray[20] = '\n';
  robotCom.handleRxBytes(testArray, 21);
}

void test8() {
  byte testArray[] = "ATTACH 1101 ";
  testArray[11] = '\n';
  robotCom.handleRxBytes(testArray, 12);
}

void test9() {
  byte testArray[] = "ATTACH 111110 ";
  testArray[13] = '\n';
  robotCom.handleRxBytes(testArray, 14);
}