#ifndef ArmServos_h
#define ArmServos_h

#include "Arduino.h"
#include "Servo.h"

#define DEFAULT_JOINT_1_SERVO_PIN 12
#define DEFAULT_JOINT_2_SERVO_PIN 11
#define DEFAULT_JOINT_3_SERVO_PIN 10
#define DEFAULT_JOINT_4_SERVO_PIN  9
#define DEFAULT_JOINT_5_SERVO_PIN  8
#define DEFAULT_GRIPPER_SERVO_PIN  6

#define INITIAL_JOINT_1_ANGLE 	  0
#define INITIAL_JOINT_2_ANGLE 	 90
#define INITIAL_JOINT_3_ANGLE 	  0
#define INITIAL_JOINT_4_ANGLE 	-90
#define INITIAL_JOINT_5_ANGLE 	 90
#define INITIAL_GRIPPER_DISTANCE 50

#define NUM_SERVOS 6
#define GRIPPER_SERVO_INDEX 0

// When attaching select servos use these constants.
#define SERVO_GRIPPER 	0x01
#define SERVO_JOINT_1   0x02
#define SERVO_JOINT_2 	0x04
#define SERVO_JOINT_3 	0x08
#define SERVO_JOINT_4 	0x10
#define SERVO_JOINT_5 	0x20

class ArmServos
{
  public:
    ArmServos();
    ArmServos(byte joint1ServoPin, byte joint2ServoPin, byte joint3ServoPin, byte joint4ServoPin, byte joint5ServoPin, byte gripperServoPin);
    void attach();
    void attachSelectedServos(byte servosToEnable);
    void setJointAngle(byte jointNumber, int angle);
    int getJointAngle(byte jointNumber);
    void setGripperDistance(int distance);
    int getGripperDistance(void);
    void setPosition(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle);
  protected:
    byte _joint1ServoPin, _joint2ServoPin, _joint3ServoPin, _joint4ServoPin, _joint5ServoPin, _gripperServoPin;
    int _servoAngles[NUM_SERVOS];
    Servo _joint1Servo;
    Servo _joint2Servo;
    Servo _joint3Servo;
    Servo _joint4Servo;
    Servo _joint5Servo;
    Servo _gripperServo;
    void _init(void);
    void _updateServos(void);
};

#endif