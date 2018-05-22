#include "Arduino.h"
#include "RobotAsciiCom.h"

RobotAsciiCom::RobotAsciiCom() {
	_nextOpenByteInMessageBuffer = 0;
	_positionCallback = NULL;
	_jointAngleCallback = NULL;
	_gripperCallback = NULL;
	_batteryVoltageRequestCallback = NULL;
	_wheelCurrentRequestCallback = NULL;
	_wheelSpeedCallback = NULL;
}

/**
 * Convenience method to handle multiple bytes.
 * Just calls the handleRxByte method with each byte individually.
 */
void RobotAsciiCom::handleRxBytes(byte newRxBytes[], int length) {
	for (int i = 0; i < length; i++) {
		handleRxByte(newRxBytes[i]);
	}
}

/**
 * Handle a newly received byte.
 * If the byte is the MESSAGE_TERMINATOR process the message.
 * If the byte is not the MESSAGE_TERMINATOR then just save it.
 */
void RobotAsciiCom::handleRxByte(byte newRxByte) {
	if (newRxByte == MESSAGE_TERMINATOR) {
		// Convert the rx message buffer to a String and parse.
		_rxMessageBuffer[_nextOpenByteInMessageBuffer] = '\0';
		String rxStr = String(_rxMessageBuffer);
		_parseStringCommand(rxStr);  // The real work happens here.
		_nextOpenByteInMessageBuffer = 0;
	} else {
		// Mid message.  Save the byte.
		_rxMessageBuffer[_nextOpenByteInMessageBuffer] = newRxByte;
		_nextOpenByteInMessageBuffer++;
	}
}

void RobotAsciiCom::registerPositionCallback(
		void (*positionCallback)(int joint1Angle, int joint2Angle,
				int joint3Angle, int joint4Angle, int joint5Angle)) {
	_positionCallback = positionCallback;
}

void RobotAsciiCom::registerJointAngleCallback(
		void (*jointAngleCallback)(byte jointNumber, int jointAngle)) {
	_jointAngleCallback = jointAngleCallback;
}

void RobotAsciiCom::registerGripperCallback(
		void (*gripperCallback)(int gripperDistance)) {
	_gripperCallback = gripperCallback;
}

void RobotAsciiCom::registerAttachSelectedServosCallback(
		void (*attachSelectedServosCallback)(byte servosToEnable)) {
	_attachSelectedServosCallback = attachSelectedServosCallback;
}

void RobotAsciiCom::registerWheelSpeedCallback(
		void (*wheelSpeedCallback)(byte leftMode, byte rightMode,
				byte leftDutyCycle, byte rightDutyCycle)) {
	_wheelSpeedCallback = wheelSpeedCallback;
}

void RobotAsciiCom::registerBatteryVoltageRequestCallback(
		void (*batteryVoltageRequestCallback)(void)) {
	_batteryVoltageRequestCallback = batteryVoltageRequestCallback;
}

void RobotAsciiCom::registerWheelCurrentRequestCallback(
		void (*wheelCurrentRequestCallback)(void)) {
	_wheelCurrentRequestCallback = wheelCurrentRequestCallback;
}

void RobotAsciiCom::registerCustomStringCallback(
		void (*customStringCallback)(String customString)) {
	_customStringCallback = customStringCallback;
}

int RobotAsciiCom::prepareBatteryVoltageReply(int batteryMillivolts, char buf[],
		int maxLength) {
	int batteryVoltageReplyLength = 27;
	// 123456789012345678901234567
	// BATTERY VOLTAGE REPLY 1.234
	// BATTERY VOLTAGE REPLY %d.%03d"
	if (maxLength < batteryVoltageReplyLength) {
		return -1;
	}
	sprintf(buf, "BATTERY VOLTAGE REPLY %d.%03d", batteryMillivolts / 1000,
			batteryMillivolts % 1000);
	if (batteryMillivolts / 1000 > 10) {
		batteryVoltageReplyLength++;
	}
	return batteryVoltageReplyLength;
}

int RobotAsciiCom::prepareWheelCurrentReply(int leftWheelMotorsMilliamps,
		int rightWheelMotorsMilliamps, char buf[], int maxLength) {
	int wheelCurrentReplyLength = 31;
	// 1234567890123456789012345678901
	// WHEEL CURRENT REPLY 1.234 2.345
	// WHEEL CURRENT REPLY %d.%03d %d.%03d
	sprintf(buf, "WHEEL CURRENT REPLY %d.%03d %d.%03d",
			leftWheelMotorsMilliamps / 1000, leftWheelMotorsMilliamps % 1000,
			rightWheelMotorsMilliamps / 1000, rightWheelMotorsMilliamps % 1000);
	if (leftWheelMotorsMilliamps / 1000 > 10) {
		wheelCurrentReplyLength++;
	}
	if (rightWheelMotorsMilliamps / 1000 > 10) {
		wheelCurrentReplyLength++;
	}
	return wheelCurrentReplyLength;
}

/**
 * Process the complete message.
 * CONSIDER: This function is too big.  
 *     Break up the POSITION and WHEEL SPEED commands to helper functions.
 */
void RobotAsciiCom::_parseStringCommand(String command) {
	int spaceIndex = command.indexOf(' ');
	String angleStr;
	if (command.startsWith("POSITION")) {
		int jointNumber = 1;
		int jointAngles[6]; // Note, gripper is joint 0, then normal order.
		angleStr = command;
		while (spaceIndex != -1 && jointNumber < 6) {
			angleStr = angleStr.substring(spaceIndex + 1);
			jointAngles[jointNumber] = angleStr.toInt();
			jointNumber++;
			spaceIndex = angleStr.indexOf(' ');
		}
		if (jointNumber == 6 && _positionCallback != NULL) {
			_positionCallback(jointAngles[1], jointAngles[2], jointAngles[3],
					jointAngles[4], jointAngles[5]);
		}
	} else if (command.startsWith("JOINT")) {
		String jointNumStr = command.substring(spaceIndex + 1, spaceIndex + 2);
		// For now I'm ignoring the next keyword ANGLE (lazy since it's the only command for a JOINT target)
		int lastSpaceIndex = command.lastIndexOf(' ');
		angleStr = command.substring(lastSpaceIndex + 1);
		if (_jointAngleCallback != NULL) {
			_jointAngleCallback((byte)(jointNumStr.toInt()), angleStr.toInt());
		}
	} else if (command.startsWith("GRIPPER")) {
		String gripperValueStr = command.substring(spaceIndex + 1);
		if (_gripperCallback != NULL) {
			_gripperCallback(gripperValueStr.toInt());
		}
	} else if (command.startsWith("ATTACH")) {
		String servosToEnableStr = command.substring(spaceIndex + 1);
		if (servosToEnableStr.length() != 6) {
			return;
		}
		byte servosToEnable = 0;
		if (servosToEnableStr.charAt(0) == '1') {
			servosToEnable |= 0x20;  // Joint 5
		}
		if (servosToEnableStr.charAt(1) == '1') {
			servosToEnable |= 0x10;  // Joint 4
		}
		if (servosToEnableStr.charAt(2) == '1') {
			servosToEnable |= 0x08;  // Joint 3
		}
		if (servosToEnableStr.charAt(3) == '1') {
			servosToEnable |= 0x04;  // Joint 2
		}
		if (servosToEnableStr.charAt(4) == '1') {
			servosToEnable |= 0x02;  // Joint 1
		}
		if (servosToEnableStr.charAt(5) == '1') {
			servosToEnable |= 0x01;  // Gripper
		}
		if (_attachSelectedServosCallback != NULL) {
			_attachSelectedServosCallback(servosToEnable);
		}
	} else if (command.startsWith("WHEEL SPEED")) {
		command = command.substring(spaceIndex + 1); // Removes the word WHEEL
		spaceIndex = command.indexOf(' ');
		command = command.substring(spaceIndex + 1); // Removes the word SPEED

		// Grab the left wheel parameters.
		byte leftMode = WHEEL_SPEED_MODE_BRAKE;
		if (command.startsWith("FORWARD")) {
			leftMode = WHEEL_SPEED_MODE_FORWARD;
		} else if (command.startsWith("REVERSE")) {
			leftMode = WHEEL_SPEED_MODE_REVERSE;
		}
		spaceIndex = command.indexOf(' ');
		command = command.substring(spaceIndex + 1); // Removes the left mode
		spaceIndex = command.indexOf(' ');
		String leftDutyCycleStr = command.substring(0, spaceIndex);
		byte leftDutyCycle = (byte)(leftDutyCycleStr.toInt());
		command = command.substring(spaceIndex + 1); // Removes the left duty cycle

		// Grab the right wheel parameters.
		// Note, obviously this should be a function as it's basically the same as the above (lazy).
		byte rightMode = WHEEL_SPEED_MODE_BRAKE;
		if (command.startsWith("FORWARD")) {
			rightMode = WHEEL_SPEED_MODE_FORWARD;
		} else if (command.startsWith("REVERSE")) {
			rightMode = WHEEL_SPEED_MODE_REVERSE;
		}
		spaceIndex = command.indexOf(' ');
		String rightDutyCycleStr = command.substring(spaceIndex + 1); // Removes the right mode
		byte rightDutyCycle = (byte)(rightDutyCycleStr.toInt());

		// Inform the callback of the command.  Note, no error handling.  Send legal messages. :)
		if (_wheelSpeedCallback != NULL) {
			_wheelSpeedCallback(leftMode, rightMode, leftDutyCycle,
					rightDutyCycle);
		}
	} else if (command.startsWith("BATTERY VOLTAGE REQUEST")) {
		if (_batteryVoltageRequestCallback != NULL) {
			_batteryVoltageRequestCallback();
		}
	} else if (command.startsWith("WHEEL CURRENT REQUEST")) {
		if (_wheelCurrentRequestCallback != NULL) {
			_wheelCurrentRequestCallback();
		}
	} else if (command.startsWith("CUSTOM")) {
		String customStr = command.substring(spaceIndex + 1);
		if (_customStringCallback != NULL) {
			_customStringCallback(customStr);
		}
	}
}
