#include <Servo.h>

#include <ArmServos.h>

ArmServos robotArm;

void setup() {
  Serial.begin(115200);
  delay(100);
  robotArm.attach();  
  Serial.println(robotArm.getGripperDistance());
  robotArm.setGripperDistance(25);
  Serial.println(robotArm.getGripperDistance());
}

void loop() {
}
