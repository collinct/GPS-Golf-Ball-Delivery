#include <Servo.h>
#include <ArmServos.h>
#include <RobotAsciiCom.h>

ArmServos armServos;
RobotAsciiCom robotCom;

void setup() {
  Serial.begin(9600);
  delay(100);
  armServos.attach();
  robotCom.registerPositionCallback(positionCallback);
  robotCom.registerJointAngleCallback(jointAngleCallback);
  robotCom.registerGripperCallback(gripperCallback);
}

void positionCallback(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) {
  armServos.setPosition(joint1Angle, joint2Angle, joint3Angle, joint4Angle, joint5Angle);
}

void jointAngleCallback(byte jointNumber, int jointAngle) {
  armServos.setJointAngle(jointNumber, jointAngle);
}

void gripperCallback(int gripperDistance) {
  armServos.setGripperDistance(gripperDistance);
}

void loop() {
}


/** Send all bytes received to the Wild Thumper Com object. */
void serialEvent() {
  while (Serial.available()) {
    robotCom.handleRxByte(Serial.read());
  }
}
