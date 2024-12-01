/*********************************************************************
 * DF Pong Controller
 * 
 * This program implements a Bluetooth Low Energy controller for Pong.
 * It sends movement data to a central device running in the browser and
 * provides audio feedback through a buzzer.
 *
 * Game Link : https://digitalfuturesocadu.github.io/df-pong/
 * 
 * Movement Values:
 * 0 = No movement / Neutral position
 * 1 = UP movement (paddle moves up)
 * 2 = DOWN movement (paddle moves down)
 * 3 = Handshake signal (used for initial connection verification)
 * 
 * Key Functions:
 * - handleInput(): Process the inputs to generate the states
 * - sendMovement(): Sends movement data over BLE (0-3)
 * - updateBLE(): Handles BLE connection management and updates
 * - updateBuzzer(): Provides different buzzer patterns for different movements
 * 
 * Key Variables:
 * - currentMovement: Stores current movement state (0-2)
 * - deviceName : GIVE YOUR DEVICE AN APPROPRIATE NAME
 * - LED_PIN : It is important to see the status of the arduino through the LED. 
      if you can see the built-in add an external one and update the pin it is connected to
 * 

 *********************************************************************/



#include <ArduinoBLE.h>
#include "ble_functions.h"
#include "buzzer_functions.h"
//Since code is split over multiple files, we have to include them here


//Name your controller!
const char* deviceName = "meat";

bool startValueNil = true;
int weightOffset = 50;

// Pin definitions buzzer/LED
const int BUZZER_PIN = 11;    // Pin for haptic feedback buzzer
const int LED_PIN_BLE = 7;    // Status LED pin
const int LED_PIN_POWER = 2;  // Status LED pin

// Movement state tracking
int currentMovement = 0;  // Current movement value (0=none, 1=up, 2=down, 3=handshake)


// Pin definitions for Button input
const int BUTTON_UP_PIN = 6;    // Pin for UP movement button
const int BUTTON_DOWN_PIN = 2;  // Pin for DOWN movement button

// use button up pin only

// variables:
int sensorValue = 0;       // the sensor value
int sensorMin = 0;         // minimum sensor value
int sensorMax = 1023;      // maximum sensor value
const int sensorPin = A0;  // sensor pin

//threshholds
int downThreshold = 70;
int upThreshold = 300;

int interval = 5000;




void setup() {

  Serial.begin(9600);

  // Configure button pins with internal pullup resistors
  // Buttons will read LOW when pressed, HIGH when released
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);    // UP button configuration
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);  // DOWN button configuration

  // Configure LED for connection status indication
  pinMode(LED_PIN_BLE, OUTPUT);

  // Don't mess with the stuff below!

  // Initialize Bluetooth Low Energy with device name and status LED
  setupBLE(deviceName, LED_PIN_BLE);

  // Initialize buzzer for feedback
  setupBuzzer(BUZZER_PIN);
  calibrateSensorStart();

  debugText() ;
}

void calibrateSensorStart() {
  // calibrate during the first five seconds - From Arduino examples
  while (millis() < 5000) {
    sensorValue = analogRead(sensorPin);

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }



    if (startValueNil) {
      CalculateStartPositions();
    } else {
      // record the minimum sensor value
      if (sensorValue > sensorMin) {
        sensorMin = sensorValue;
      }
    }

    // signal the end of the calibration period
    digitalWrite(LED_PIN_POWER, HIGH);
  }
}

int stillThreshold;

void CalculateStartPositions() {
  stillThreshold = sensorValue;
  downThreshold = stillThreshold - 10;
  upThreshold = stillThreshold + 10;
}

void sensorInput() {
  // read the sensor:
  sensorValue = analogRead(sensorPin);

  // in case the sensor value is outside the range seen during calibration
  sensorValue = constrain(sensorValue, sensorMin, sensorMax);
  //  sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 2000);
}

void loop() {
  // Update BLE connection status and handle incoming data
  updateBLE();

  // get the sensor value, adjusted for calibration
  sensorInput();

  //read the inputs te determine the current state
  //results in changing the value of currentMovement
  handleInputStatic();
  //handleInputDynamic();

  //send the movement state to P5
  sendMovement(currentMovement);

  updateBuzzer(2);

}
int diff;

// not using this - doesn't work for gameplay.
void handleInputDynamic() {

  unsigned long currentMillis = millis();  // grab current time

  diff = currentMillis - previousMillis;
  if (currentMillis - previousMillis >= 1000) {

    handleInputStatic();
    Serial.println(diff);

    previousMillis = millis();
  }
  Serial.println(diff);
}

void debugText() {
  Serial.print("min: ");
  Serial.print(sensorMin);
  Serial.print(" max: ");
  Serial.print(sensorMax);
  Serial.print(" pressure: ");
  Serial.println(sensorValue);
}

void sensorRead() {


  Serial.print(" still: ");
  Serial.print(stillThreshold);
  Serial.print("                   ");

  Serial.print(" d ");
  Serial.print(downThreshold);
  Serial.print(" u: ");
  Serial.print(upThreshold);
  Serial.print("                  ");
  Serial.print(sensorValue);
  Serial.print(" ");
}
void handleInputStatic() {


  if (sensorValue > upThreshold) {
    currentMovement = 1;  // UP movement
    sensorRead();
    Serial.println("UP");
     digitalWrite(LED_PIN_POWER, HIGH);
  } else if (sensorValue < downThreshold) {
    currentMovement = 2;  // DOWN movement
    sensorRead();
    Serial.println("DOWN");
     digitalWrite(LED_PIN_POWER, HIGH);
  } else {
    currentMovement = 0;  // No movement
    sensorRead();
    Serial.println("stay");
    
  }
}




void handleInputOld() {
  //flipped read method because of INPUT_PULLUP
  bool upPressed = !digitalRead(BUTTON_UP_PIN);
  bool downPressed = !digitalRead(BUTTON_DOWN_PIN);

  if (upPressed && !downPressed) {
    currentMovement = 1;
    Serial.println(1);  // UP movement
  } else if (downPressed && !upPressed) {
    currentMovement = 2;  // DOWN movement
  } else {
    currentMovement = 0;  // No movement
  }
}