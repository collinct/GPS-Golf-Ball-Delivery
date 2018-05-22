#include "Arduino.h"
#include "GolfBallStand.h"

GolfBallStand::GolfBallStand() {
	_init();  
}

// Creates an instance of this class for the sketch.
void GolfBallStand::_init() {
	pinMode(PIN_LED_1_UNDER, OUTPUT);
	pinMode(PIN_LED_1_FRONT, OUTPUT);
	pinMode(PIN_LED_2_UNDER, OUTPUT);
	pinMode(PIN_LED_2_FRONT, OUTPUT);
	pinMode(PIN_LED_3_UNDER, OUTPUT);
	pinMode(PIN_LED_3_FRONT, OUTPUT);
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(PIN_BLUE, OUTPUT);
	pinMode(PIN_GOLF_BALL_STAND_SWITCH, INPUT_PULLUP);
	digitalWrite(PIN_LED_1_UNDER, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_1_FRONT, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_2_UNDER, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_2_FRONT, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_3_UNDER, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_3_FRONT, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_RED, COLOR_TRANSISTOR_OFF);
	digitalWrite(PIN_GREEN, COLOR_TRANSISTOR_OFF);
	digitalWrite(PIN_BLUE, COLOR_TRANSISTOR_OFF);
}

// Turns on some color LED (or combination of colors) for some LED location (or combination of LEDs).
void GolfBallStand::setLedState(int ledColor, int location, int underOrFront){
	// Start by clearing off all LEDs and colors.
	digitalWrite(PIN_RED, COLOR_TRANSISTOR_OFF);
	digitalWrite(PIN_GREEN, COLOR_TRANSISTOR_OFF);
	digitalWrite(PIN_BLUE, COLOR_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_1_UNDER, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_2_UNDER, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_3_UNDER, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_1_FRONT, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_2_FRONT, LED_TRANSISTOR_OFF);
	digitalWrite(PIN_LED_3_FRONT, LED_TRANSISTOR_OFF);

	// Decide which of the six LEDs to turn on.
	if ((location & LOCATION_1) && (underOrFront & LED_UNDER)) {
		digitalWrite(PIN_LED_1_UNDER, LED_TRANSISTOR_ON);
	}
	if ((location & LOCATION_1) && (underOrFront & LED_FRONT)) {
		digitalWrite(PIN_LED_1_FRONT, LED_TRANSISTOR_ON);
	}
	if ((location & LOCATION_2) && (underOrFront & LED_UNDER)) {
		digitalWrite(PIN_LED_2_UNDER, LED_TRANSISTOR_ON);
	}
	if ((location & LOCATION_2) && (underOrFront & LED_FRONT)) {
		digitalWrite(PIN_LED_2_FRONT, LED_TRANSISTOR_ON);
	}
	if ((location & LOCATION_3) && (underOrFront & LED_UNDER)) {
		digitalWrite(PIN_LED_3_UNDER, LED_TRANSISTOR_ON);
	}
	if ((location & LOCATION_3) && (underOrFront & LED_FRONT)) {
		digitalWrite(PIN_LED_3_FRONT, LED_TRANSISTOR_ON);
	}

	// Set the color.
	if (ledColor & LED_BLUE) {
		digitalWrite(PIN_BLUE, COLOR_TRANSISTOR_ON);
	}
	if (ledColor & LED_GREEN) {
		digitalWrite(PIN_GREEN, COLOR_TRANSISTOR_ON);
	}
	if (ledColor & LED_RED) {
		digitalWrite(PIN_RED, COLOR_TRANSISTOR_ON);
	}

}

// Returns the analog value of the photodiode sensor at that location (0 to 1023).
int GolfBallStand::getAnalogReading(int location) {
	int photoReading = -1;
	switch (location) {
	case LOCATION_1:
		photoReading = analogRead(PIN_PHOTO_1);
		break;
	case LOCATION_2:
		photoReading = analogRead(PIN_PHOTO_2);
		break;
	case LOCATION_3:
		photoReading = analogRead(PIN_PHOTO_3);
		break;
	case LOCATION_EXTERNAL:
		photoReading = analogRead(PIN_PHOTO_EXTERNAL);
		break;
	}
	return photoReading;

}

// Returns the ball color at that sensor location.
int GolfBallStand::determineBallColor(int location) {
	int returnBallType = BALL_NONE;
	
	setLedState(LED_OFF, location, LED_UNDER_AND_FRONT);
	delay(GBS_TIME_DELAY);
	int offReading = getAnalogReading(location);

	setLedState(LED_RED, location, LED_UNDER_AND_FRONT);
	delay(GBS_TIME_DELAY);
	int redReading = getAnalogReading(location);
	
	setLedState(LED_GREEN, location, LED_UNDER_AND_FRONT);
	delay(GBS_TIME_DELAY);
	int greenReading = getAnalogReading(location);

	setLedState(LED_BLUE, location, LED_UNDER_AND_FRONT);
	delay(GBS_TIME_DELAY);
	int blueReading = getAnalogReading(location);

	setLedState(LED_WHITE, location, LED_UNDER_AND_FRONT);
	delay(GBS_TIME_DELAY);
	int whiteReading = getAnalogReading(location);
	
	setLedState(LED_OFF, location, LED_UNDER_AND_FRONT);
	
	int sumAll = redReading + greenReading + blueReading + whiteReading;
	int sumBlueGreen = blueReading + greenReading;
	int diffBlueGreen = blueReading - greenReading;
	int diffBlueRed = blueReading - redReading;
	
	// TODO: Figure out the ball color.
	// Example of an overly simple solution.
	if (offReading < 200) {
		// offReading determines if ball is present or not
		// Could use the external sensor instead of a hardcode 950.
		returnBallType = BALL_NONE;
	} else {
		  // Passed the offReading test, now use other color tests.
		  /* switch (location) {
			  case LOCATION_1:
			      if ((sumAll>1400)&&(sumAll <1775)&&(whiteReading>200)&&(whiteReading<275)&&(blueReading>500)&&(blueReading<725)) {
					  returnBallType = BALL_WHITE;
				  } else if ((sumAll>1775)&&(sumAll <2000)&&(whiteReading>250)&&(whiteReading<300)&&(blueReading>725)&&(blueReading<850)) {
					  returnBallType = BALL_YELLOW;
				  } else if ((sumAll>2400)&&(sumAll <3100)&&(redReading>550)&&(redReading<700)) {
					  returnBallType = BALL_RED;
				  } else if ((sumAll>3100)&&(sumAll <3800)&&(sumBlueGreen>1600)&&(sumBlueGreen<1900)) {
					  returnBallType = BALL_BLACK;
				  } else if ((redReading<blueReading)&&(diffBlueGreen>175)&&(diffBlueGreen<400)) {
					  returnBallType = BALL_GREEN;
				  } else if ((redReading > blueReading)&&(diffBlueGreen>125)&&(diffBlueGreen<300)) {
					  returnBallType = BALL_BLUE;
				  }
				  break;
			  case LOACTION_2:
				  if ((sumAll>900)&&(sumAll <1650)&&(whiteReading>150)&&(whiteReading<250)&&(blueReading>400)&&(blueReading<700)) {
					  returnBallType = BALL_WHITE;
				  } else if ((sumAll>1600)&&(sumAll <1900)&&(whiteReading>200)&&(whiteReading<300)&&(blueReading>700)&&(blueReading<800)) {
					  returnBallType = BALL_YELLOW;
				  } else if ((sumAll>2200)&&(sumAll <2800)&&(redReading>400)&&(redReading<600)) {
					  returnBallType = BALL_RED;
				  } else if ((sumAll>3000)&&(sumAll <3800)&&(sumBlueGreen>1400)&&(sumBlueGreen<1900)&&(blueReading>800)&&(blueReading<1023)) {
					  returnBallType = BALL_BLACK;
				  } else if () {
					  
				  }
			  case LOACTION_3:
				  if ((sumAll>800)&&(sumAll <1100)&&(whiteReading>100)&&(whiteReading<175)&&(blueReading>250)&&(blueReading<350)) {
					  returnBallType = BALL_WHITE;
				  } else if ((sumAll>1200)&&(sumAll <1550)&&(whiteReading>150)&&(whiteReading<225)&&(blueReading>400)&&(blueReading<625)) {
					  returnBallType = BALL_YELLOW;
				  } else if ((sumAll>1800)&&(sumAll <2400)&&(redReading>300)&&(redReading<500)) {
					  returnBallType = BALL_RED;
				  } else if ((sumAll>2500)&&(sumAll <3400)&&(sumBlueGreen>1000)&&(sumBlueGreen<1500)&&(greenReading>750)&&(greenReading<900)) {
					  returnBallType = BALL_BLACK;
				  } else if () {
					  
				  }
		  } */
		  switch (location) {
			  case LOCATION_1:
				  if ((sumAll<2200)&&(diffBlueRed>100)&&(diffBlueRed<325)) {
					  returnBallType = BALL_WHITE;
				  } else if ((sumAll<2200)&&(diffBlueRed>=325)&&(diffBlueRed<500)) {
					  returnBallType = BALL_YELLOW;
				  } else if ((greenReading>500)&&(greenReading<900)&&(diffBlueRed>175)&&(diffBlueRed<400)) {
					  returnBallType = BALL_RED;
				  } else if ((sumAll>3100)&&(sumAll<3800)&&(sumBlueGreen>1600)&&(sumBlueGreen<1900)&&(greenReading>675)&&(greenReading<950)) {
					  returnBallType = BALL_BLACK;
				  } else if ((redReading<blueReading)&&(diffBlueGreen>175)&&(diffBlueGreen<400)) {
					  returnBallType = BALL_GREEN;
				  } else if ((redReading>blueReading)&&(diffBlueGreen>125)&&(diffBlueGreen<300)) {
					  returnBallType = BALL_BLUE;
				  }
				  break;
			  case LOCATION_2:
				  if ((sumAll<2000)&&(diffBlueRed>75)&&(diffBlueRed<325)) {
					  returnBallType = BALL_WHITE;
				  } else if ((sumAll<2000)&&(diffBlueRed>=325)&&(diffBlueRed<500)) {
					  returnBallType = BALL_YELLOW;
				  } else if ((greenReading>500)&&(greenReading<900)&&(diffBlueRed>200)&&(diffBlueRed<500)) {
					  returnBallType = BALL_RED;
				  } else if ((sumAll>3000)&&(sumAll<3800)&&(sumBlueGreen>1600)&&(sumBlueGreen<2000)&&(blueReading>800)&&(blueReading<1023)&&(greenReading>675)&&(greenReading<950)) {
					  returnBallType = BALL_BLACK;
				  } else if ((redReading<blueReading)&&(diffBlueGreen>175)&&(diffBlueGreen<350)) {
					  returnBallType = BALL_GREEN;
				  } else if ((redReading>blueReading)&&(diffBlueGreen>100)&&(diffBlueGreen<275)) {
					  returnBallType = BALL_BLUE;
				  }
				  break;
			  case LOCATION_3:
				  if ((sumAll<1600)&&(diffBlueRed>0)&&(diffBlueRed<125)) {
					  returnBallType = BALL_WHITE;
				  } else if ((sumAll<1600)&&(diffBlueRed>=125)&&(diffBlueRed<400)) {
					  returnBallType = BALL_YELLOW;
				  } else if ((greenReading>500)&&(greenReading<900)&&(diffBlueRed>50)&&(diffBlueRed<450)) {
					  returnBallType = BALL_RED;
				  } else if ((sumAll>2500)&&(sumAll<3400)&&(sumBlueGreen>1350)&&(sumBlueGreen<1700)&&(greenReading>750)&&(greenReading<1000)&&(greenReading>750)&&(greenReading<950)) {
					  returnBallType = BALL_BLACK;
				  //} else if ((redReading<blueReading)&&(diffBlueGreen>-225)&&(diffBlueGreen<100)) {
			      } else if ((diffBlueGreen>-225)&&(diffBlueGreen<100)) {
					  returnBallType = BALL_GREEN;
				  } else if ((redReading>blueReading)&&(diffBlueGreen>-350)&&(diffBlueGreen<-125)) {
					  returnBallType = BALL_BLUE;
				  }
				  break;
				  /* if (redReading<blueReading) {
					  greenFactor = greenFactor + 0.5;
				  }
				  if (redReading>blueReading) {
					  blueFactor = blueFactor + 0.5;
				  }
				  if ((diffBlueGreen>-225)&&(diffBlueGreen<100)) {
					  greenFactor = greenFactor + 0.5;
				  }
				  if ((diffBlueGreen>-350)&&(diffBlueGreen<-125)) {
					  blueFactor = blueFactor + 0.5;
				  }
				  if () {
					  
				  }
				  if () {
					  
				  }
				  
				  
				  
				  
				  if (greenFactor > blueFactor) {
					  returnBallType = BALL_GREEN;
				  } else {
					  returnBallType = BALL_BLUE;
				  } */
				  
				  
				  
		  }
		  
	}

	Serial.println();
	Serial.print("Readings for location: ");
	Serial.println(location == LOCATION_3 ? 3 : location);
	Serial.println(" Off    Red    Green    Blue    White ");
	Serial.print(" ");
	Serial.print(offReading);
	Serial.print("   ");
	Serial.print(redReading);
	Serial.print("     ");
	Serial.print(greenReading);
	Serial.print("      ");
	Serial.print(blueReading);
	Serial.print("     ");
	Serial.print(whiteReading);
	
	
	// offReading + "  " + redReading + "  " + greenReading + "  " + blueReading + "  " + whiteReading);
	
	
	// Serial.print("  LED off reading   = ");
	// Serial.println(offReading);
	// Serial.print("  LED red reading   = ");
	// Serial.println(redReading);
	// Serial.print("  LED green reading = ");
	// Serial.println(greenReading);
	// Serial.print("  LED blue reading  = ");
	// Serial.println(blueReading);
	// Serial.print("  LED white reading = ");
	// Serial.println(whiteReading);
	
	return returnBallType;
}