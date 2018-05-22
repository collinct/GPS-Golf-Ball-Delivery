#ifndef RobotAsciiCom_h
#define RobotAsciiCom_h

#include "Arduino.h"

#define MAX_ASCII_MESSAGE_LENGTH 	64 // If you have longer message change this!
#define MESSAGE_TERMINATOR 			10

// Constants for WHEEL SPEED messages.
#define WHEEL_SPEED_MODE_REVERSE 0  // Mode to go in reverse
#define WHEEL_SPEED_MODE_BRAKE   1  // Mode to stop
#define WHEEL_SPEED_MODE_FORWARD 2  // Mode to go forward

class RobotAsciiCom
{
  public:
    RobotAsciiCom();
    void handleRxByte(byte newRxByte);
    void handleRxBytes(byte newRxBytes[], int length);
    void registerPositionCallback(void (* positionCallback)(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) );
    void registerJointAngleCallback(void (* jointAngleCallback)(byte jointNumber, int jointAngle) );
    void registerGripperCallback(void (* gripperCallback)(int gripperDistance) );
    void registerAttachSelectedServosCallback( void (* attachSelectedServosCallback)(byte servosToEnable) );
	void registerWheelSpeedCallback(void (* wheelSpeedCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) );
    void registerBatteryVoltageRequestCallback(void (* batteryVoltageRequestCallback)(void) );
    void registerWheelCurrentRequestCallback(void (* wheelCurrentRequestCallback)(void) );
	void registerCustomStringCallback( void (* customStringCallback)(String customString) );
	int prepareBatteryVoltageReply(int batteryMillivolts, char buf[], int maxLength);
	int prepareWheelCurrentReply(int leftWheelMotorsMilliamps, int rightWheelMotorsMilliamps, char buf[], int maxLength);
  private:
    char _rxMessageBuffer[MAX_ASCII_MESSAGE_LENGTH];
    int _nextOpenByteInMessageBuffer;
    void (* _positionCallback)(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle);
    void (* _jointAngleCallback)(byte joint, int jointAngle);
    void (* _gripperCallback)(int gripperDistance);
    void (* _attachSelectedServosCallback)(byte servosToEnable);
    void (* _wheelSpeedCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
    void (* _batteryVoltageRequestCallback)(void);
    void (* _wheelCurrentRequestCallback)(void);
    void (* _customStringCallback)(String customString);
    void _parseStringCommand(String command);
};

#endif
