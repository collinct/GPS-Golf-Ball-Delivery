#include "Arduino.h"
#include "WildThumperCom.h"

WildThumperCom::WildThumperCom(byte teamNumber) {
	_teamNumber = teamNumber;
	_wheelSpeedCallback = NULL;
	_positionCallback = NULL;
	_jointAngleCallback = NULL;
	_gripperCallback = NULL;
	_batteryVoltageRequestCallback = NULL;
	_batteryVoltageReplyCallback = NULL;
	_wheelCurrentRequestCallback = NULL;
	_wheelCurrentReplyCallback = NULL;
    _teamNumberRequestCallback = NULL;
    _teamNumberReplyCallback = NULL;
    _teamNumberChangeRequestCallback = NULL;
    _attachSelectedServosCallback = NULL;
	_lastByteWasStartByte = false;
	_lastByteWasEscapeByte = false;
	_bytesRemainingInMessage = -1;
}

void WildThumperCom::setTeamNumber(byte teamNumber) {
	_teamNumber = teamNumber;
}

void WildThumperCom::sendWheelSpeed(byte leftMode, byte rightMode,
		byte leftDutyCycle, byte rightDutyCycle) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_WHEEL_SPEED;
	_txMessageBuffer[WHEEL_SPEED_LEFT_MODE] = leftMode;
	_txMessageBuffer[WHEEL_SPEED_RIGHT_MODE] = rightMode;
	_txMessageBuffer[WHEEL_SPEED_LEFT_DUTY_CYCLE] = leftDutyCycle;
	_txMessageBuffer[WHEEL_SPEED_RIGHT_DUTY_CYCLE] = rightDutyCycle;
	_sendMessage (WHEEL_SPEED_MESSAGE_LENGTH);
}

void WildThumperCom::sendPosition(int joint1Angle, int joint2Angle,
		int joint3Angle, int joint4Angle, int joint5Angle) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_SET_ARM_POSITION;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_1_LSB] = (byte) joint1Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_1_MSB] = (byte)(joint1Angle >> 8);
	_txMessageBuffer[SET_ARM_POSITION_JOINT_2_LSB] = (byte) joint2Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_2_MSB] = (byte)(joint2Angle >> 8);
	_txMessageBuffer[SET_ARM_POSITION_JOINT_3_LSB] = (byte) joint3Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_3_MSB] = (byte)(joint3Angle >> 8);
	_txMessageBuffer[SET_ARM_POSITION_JOINT_4_LSB] = (byte) joint4Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_4_MSB] = (byte)(joint4Angle >> 8);
	_txMessageBuffer[SET_ARM_POSITION_JOINT_5_LSB] = (byte) joint5Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_5_MSB] = (byte)(joint5Angle >> 8);
	_sendMessage (SET_ARM_POSITION_LENGTH);
}

void WildThumperCom::sendJointAngle(byte jointNumber, int jointAngle) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_SET_JOINT_ANGLE;
	_txMessageBuffer[SET_JOINT_ANGLE_JOINT_NUMBER] = jointNumber;
	_txMessageBuffer[SET_JOINT_ANGLE_ANGLE_LSB] = (byte) jointAngle;
	_txMessageBuffer[SET_JOINT_ANGLE_ANGLE_MSB] = (byte)(jointAngle >> 8);
	_sendMessage (SET_JOINT_ANGLE_LENGTH);
}

void WildThumperCom::sendGripperDistance(int gripperDistance) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_SET_GRIPPER_DISTANCE;
	_txMessageBuffer[SET_GRIPPER_DISTANCE_LSB] = (byte) gripperDistance;
	_txMessageBuffer[SET_GRIPPER_DISTANCE_MSB] = (byte)(gripperDistance >> 8);
	_sendMessage (SET_GRIPPER_DISTANCE_LENGTH);
}

void WildThumperCom::sendBatteryVoltageRequest() {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_BATTERY_VOLTAGE_REQUEST;
	_sendMessage (BATTERY_VOLTAGE_REQUEST_LENGTH);
}

void WildThumperCom::sendBatteryVoltageReply(int batteryMillivolts) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_BATTERY_VOLTAGE_REPLY;
	_txMessageBuffer[BATTERY_VOLTAGE_REPLY_LSB] = (byte) batteryMillivolts;
	_txMessageBuffer[BATTERY_VOLTAGE_REPLY_MSB] = (byte)(
			batteryMillivolts >> 8);
	_sendMessage (BATTERY_VOLTAGE_REPLY_LENGTH);
}

void WildThumperCom::sendWheelCurrentRequest() {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_WHEEL_CURRENT_REQUEST;
	_sendMessage (WHEEL_CURRENT_REQUEST_LENGTH);
}

void WildThumperCom::sendWheelCurrentReply(int leftWheelMotorsMilliamps,
		int rightWheelMotorsMilliamps) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_WHEEL_CURRENT_REPLY;
	_txMessageBuffer[WHEEL_CURRENT_REPLY_LEFT_LSB] =
			(byte) leftWheelMotorsMilliamps;
	_txMessageBuffer[WHEEL_CURRENT_REPLY_LEFT_MSB] = (byte)(
			leftWheelMotorsMilliamps >> 8);
	_txMessageBuffer[WHEEL_CURRENT_REPLY_RIGHT_LSB] =
			(byte) rightWheelMotorsMilliamps;
	_txMessageBuffer[WHEEL_CURRENT_REPLY_RIGHT_MSB] = (byte)(
			rightWheelMotorsMilliamps >> 8);
	_sendMessage (WHEEL_CURRENT_REPLY_LENGTH);
}

void WildThumperCom::sendTeamNumberRequest() {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_TEAM_NUMBER_REQUEST;
	_sendMessage (TEAM_NUMBER_REQUEST_REQUEST_LENGTH);
}

void WildThumperCom::sendTeamNumberReply(byte teamNumber) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_TEAM_NUMBER_REPLY;
	_txMessageBuffer[TEAM_NUMBER_REPLY_TEAM_NUMBER] = teamNumber;
	_sendMessage (TEAM_NUMBER_REPLY_LENGTH);
}

void WildThumperCom::sendTeamNumberChangeRequest(byte teamNumber) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_TEAM_NUMBER_CHANGE_REQUEST;
	_txMessageBuffer[TEAM_NUMBER_CHANGE_REQUEST_NEW_TEAM_NUMBER] = teamNumber;
	_sendMessage (TEAM_NUMBER_CHANGE_REQUEST_LENGTH);
}

void WildThumperCom::sendAttachSelectedServos(byte servosToEnable) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_ATTACH_SELECTED_SERVOS;
	_txMessageBuffer[ATTACH_SELECTED_SERVOS_ENABLE_BYTE] = servosToEnable;
	_sendMessage (ATTACH_SELECTED_SERVOS_LENGTH);
}

void WildThumperCom::_sendMessage(byte messageLength) {
	byte crc = 0;
	Serial.write(START_BYTE);
	_sendByte(messageLength);
	for (int i = 0; i < messageLength; i++) {
		_sendByte (_txMessageBuffer[i]);
		crc += _txMessageBuffer[i];
	}
	_sendByte(-crc);
}

void WildThumperCom::_sendByte(byte unescapedbyte) {
	if (unescapedbyte == START_BYTE || unescapedbyte == ESCAPE_BYTE) {
		Serial.write(ESCAPE_BYTE);
		Serial.write(unescapedbyte ^ ESCAPE_XOR);
	} else {
		Serial.write(unescapedbyte);
	}
}

void WildThumperCom::registerWheelSpeedCallback(
		void (*wheelSpeedCallback)(byte leftMode, byte rightMode,
				byte leftDutyCycle, byte rightDutyCycle)) {
	_wheelSpeedCallback = wheelSpeedCallback;
}

void WildThumperCom::registerPositionCallback(
		void (*positionCallback)(int joint1Angle, int joint2Angle,
				int joint3Angle, int joint4Angle, int joint5Angle)) {
	_positionCallback = positionCallback;
}

void WildThumperCom::registerJointAngleCallback(
		void (*jointAngleCallback)(byte jointNumber, int jointAngle)) {
	_jointAngleCallback = jointAngleCallback;
}

void WildThumperCom::registerGripperCallback(
		void (*gripperCallback)(int gripperDistance)) {
	_gripperCallback = gripperCallback;
}

void WildThumperCom::registerBatteryVoltageRequestCallback(
		void (*batteryVoltageRequestCallback)(void)) {
	_batteryVoltageRequestCallback = batteryVoltageRequestCallback;
}

void WildThumperCom::registerBatteryVoltageReplyCallback(
		void (*batteryVoltageReplyCallback)(int batteryMillivolts)) {
	_batteryVoltageReplyCallback = batteryVoltageReplyCallback;
}

void WildThumperCom::registerWheelCurrentRequestCallback(
		void (*wheelCurrentRequestCallback)(void)) {
	_wheelCurrentRequestCallback = wheelCurrentRequestCallback;
}

void WildThumperCom::registerWheelCurrentReplyCallback(
		void (*wheelCurrentReplyCallback)(int leftWheelMotorsMilliamps,
				int rightWheelMotorsMilliamps)) {
	_wheelCurrentReplyCallback = wheelCurrentReplyCallback;
}

void WildThumperCom::registerTeamNumberRequestCallback(void (* teamNumberRequestCallback)(void) ) {
	_teamNumberRequestCallback = teamNumberRequestCallback;
}

void WildThumperCom::registerTeamNumberReplyCallback(void (* teamNumberReplyCallback)(byte teamNumber) ) {
	_teamNumberReplyCallback = teamNumberReplyCallback;
}

void WildThumperCom::registerTeamNumberChangeRequestCallback(void (* teamNumberChangeRequestCallback)(byte teamNumber) ) {
	_teamNumberChangeRequestCallback = teamNumberChangeRequestCallback;
}

void WildThumperCom::registerAttachSelectedServosCallback(void (* attachSelectedServosCallback)(byte servosToEnable) ) {
	_attachSelectedServosCallback = attachSelectedServosCallback;
}


void WildThumperCom::handleRxByte(byte newRxByte) {
	// Highest priority is the start byte.
	if (newRxByte == START_BYTE) {
		_lastByteWasStartByte = true;
		_lastByteWasEscapeByte = false;
		return;
	} else if (!_lastByteWasStartByte) {
		// Make sure we are expecting bytes (ignore unexpected bytes).
		if (_bytesRemainingInMessage < 0) {
			return;
		}
	}
	// Next handle the escaping mechanism.
	if (_lastByteWasEscapeByte) {
		newRxByte ^= ESCAPE_XOR;
	} else {
		if (newRxByte == ESCAPE_BYTE) {
			_lastByteWasEscapeByte = true;
			return;
		}
	}
	_lastByteWasEscapeByte = false;
	// Next handle the length byte
	if (_lastByteWasStartByte) {
		// This is the length byte.
		_bytesRemainingInMessage = newRxByte;
		_crc = 0;
		_nextOpenByteInMessageBuffer = 0;
		_lastByteWasStartByte = false;
		return;
	}
	// Handle this data byte.
	_crc += newRxByte;
	if (_bytesRemainingInMessage > 0) {
		// Receiving bytes in the message.
		_rxMessageBuffer[_nextOpenByteInMessageBuffer] = newRxByte;
		_nextOpenByteInMessageBuffer++;
	} else {
		// Message just finished time to validate the CRC byte.
		if (_crc == 0) {
			_parseValidMessage();
		}
	}
	_bytesRemainingInMessage--;
}

void WildThumperCom::_parseValidMessage() {
	byte teamNumber = _rxMessageBuffer[TEAM_NUMBER_BYTE];
	if (teamNumber != _teamNumber
		&& !(_rxMessageBuffer[COMMAND_BYTE] == COMMAND_TEAM_NUMBER_REQUEST ||
			 _rxMessageBuffer[COMMAND_BYTE] == COMMAND_TEAM_NUMBER_REPLY ||
			 _rxMessageBuffer[COMMAND_BYTE] == COMMAND_TEAM_NUMBER_CHANGE_REQUEST)) {
		// Silently do nothing if it's not our team.
		return;
	}
	switch (_rxMessageBuffer[COMMAND_BYTE]) {
	case COMMAND_WHEEL_SPEED:
		if (_wheelSpeedCallback != NULL) {
			_wheelSpeedCallback(_rxMessageBuffer[WHEEL_SPEED_LEFT_MODE],
					_rxMessageBuffer[WHEEL_SPEED_RIGHT_MODE],
					_rxMessageBuffer[WHEEL_SPEED_LEFT_DUTY_CYCLE],
					_rxMessageBuffer[WHEEL_SPEED_RIGHT_DUTY_CYCLE]);
		}
		break;
	case COMMAND_SET_ARM_POSITION:
		if (_positionCallback != NULL) {
			int joint1Angle = _rxMessageBuffer[SET_ARM_POSITION_JOINT_1_MSB];
			joint1Angle = joint1Angle << 8;
			joint1Angle += _rxMessageBuffer[SET_ARM_POSITION_JOINT_1_LSB];
			int joint2Angle = _rxMessageBuffer[SET_ARM_POSITION_JOINT_2_MSB];
			joint2Angle = joint2Angle << 8;
			joint2Angle += _rxMessageBuffer[SET_ARM_POSITION_JOINT_2_LSB];
			int joint3Angle = _rxMessageBuffer[SET_ARM_POSITION_JOINT_3_MSB];
			joint3Angle = joint3Angle << 8;
			joint3Angle += _rxMessageBuffer[SET_ARM_POSITION_JOINT_3_LSB];
			int joint4Angle = _rxMessageBuffer[SET_ARM_POSITION_JOINT_4_MSB];
			joint4Angle = joint4Angle << 8;
			joint4Angle += _rxMessageBuffer[SET_ARM_POSITION_JOINT_4_LSB];
			int joint5Angle = _rxMessageBuffer[SET_ARM_POSITION_JOINT_5_MSB];
			joint5Angle = joint5Angle << 8;
			joint5Angle += _rxMessageBuffer[SET_ARM_POSITION_JOINT_5_LSB];
			_positionCallback(joint1Angle, joint2Angle, joint3Angle,
					joint4Angle, joint5Angle);
		}
		break;
	case COMMAND_SET_JOINT_ANGLE:
		if (_jointAngleCallback != NULL) {
			int jointAngle = _rxMessageBuffer[SET_JOINT_ANGLE_ANGLE_MSB];
			jointAngle = jointAngle << 8;
			jointAngle += _rxMessageBuffer[SET_JOINT_ANGLE_ANGLE_LSB];
			_jointAngleCallback(_rxMessageBuffer[SET_JOINT_ANGLE_JOINT_NUMBER],
					jointAngle);
		}
		break;
	case COMMAND_SET_GRIPPER_DISTANCE:
		if (_gripperCallback != NULL) {
			int gripperDistance = _rxMessageBuffer[SET_GRIPPER_DISTANCE_MSB];
			gripperDistance = gripperDistance << 8;
			gripperDistance += _rxMessageBuffer[SET_GRIPPER_DISTANCE_LSB];
			_gripperCallback(gripperDistance);
		}
		break;
	case COMMAND_BATTERY_VOLTAGE_REQUEST:
		if (_batteryVoltageRequestCallback != NULL) {
			_batteryVoltageRequestCallback();
		}
		break;
	case COMMAND_BATTERY_VOLTAGE_REPLY:
		if (_batteryVoltageReplyCallback != NULL) {
			int batteryVoltageMillivolts =
					_rxMessageBuffer[BATTERY_VOLTAGE_REPLY_MSB];
			batteryVoltageMillivolts = batteryVoltageMillivolts << 8;
			batteryVoltageMillivolts +=
					_rxMessageBuffer[BATTERY_VOLTAGE_REPLY_LSB];
			_batteryVoltageReplyCallback(batteryVoltageMillivolts);
		}
		break;
	case COMMAND_WHEEL_CURRENT_REQUEST:
		if (_wheelCurrentRequestCallback != NULL) {
			_wheelCurrentRequestCallback();
		}
		break;
	case COMMAND_WHEEL_CURRENT_REPLY:
		if (_wheelCurrentReplyCallback != NULL) {
			int leftWheelMotorsMilliamps =
					_rxMessageBuffer[WHEEL_CURRENT_REPLY_LEFT_MSB];
			leftWheelMotorsMilliamps = leftWheelMotorsMilliamps << 8;
			leftWheelMotorsMilliamps += 
					_rxMessageBuffer[WHEEL_CURRENT_REPLY_LEFT_LSB];
			int rightWheelMotorsMilliamps =
					_rxMessageBuffer[WHEEL_CURRENT_REPLY_RIGHT_MSB];
			rightWheelMotorsMilliamps = rightWheelMotorsMilliamps << 8;
			rightWheelMotorsMilliamps += 
					_rxMessageBuffer[WHEEL_CURRENT_REPLY_RIGHT_LSB];
			_wheelCurrentReplyCallback(leftWheelMotorsMilliamps, rightWheelMotorsMilliamps);
		}
		break;
	case COMMAND_TEAM_NUMBER_REQUEST:
		if (_teamNumberRequestCallback != NULL) {
			_teamNumberRequestCallback();
		}
		break;
	case COMMAND_TEAM_NUMBER_REPLY:
		if (_teamNumberReplyCallback != NULL) {
			_teamNumberReplyCallback(_rxMessageBuffer[TEAM_NUMBER_REPLY_TEAM_NUMBER]);
		}
		break;
	case COMMAND_TEAM_NUMBER_CHANGE_REQUEST:
		if (_teamNumberChangeRequestCallback != NULL) {
			_teamNumberChangeRequestCallback(_rxMessageBuffer[TEAM_NUMBER_CHANGE_REQUEST_NEW_TEAM_NUMBER]);
		}
		break;
	case COMMAND_ATTACH_SELECTED_SERVOS:
		if (_attachSelectedServosCallback != NULL) {
			_attachSelectedServosCallback(_rxMessageBuffer[ATTACH_SELECTED_SERVOS_ENABLE_BYTE]);
		}
		break;
	default:
		// Silently do nothing with unknown commands
		break;
	}
}
