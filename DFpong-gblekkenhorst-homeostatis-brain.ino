/*********************************************************************
 * DF Pong Controller - homeostasis BRAIN
 * 
 * This program implements a Bluetooth Low Energy controller for Pong.
 * It sends movement data to a central device running in the browser and
 * provides audio feedback through a buzzer. It uses a pressure sensor to 
 * collect data - it takes the starting pressure and calculates threshold 
 * above and below that value to decide how much pressure is required to 
 * switch between Up/Down/Still. 
 *
 * Game Link : https://digitalfuturesocadu.github.io/df-pong/
 * 
 * Movement Values:
 * 0 = No movement / Neutral position
 * 1 = UP movement (paddle moves up)
 * 2 = DOWN movement (paddle moves down)
 * 3 = Handshake signal (used for initial connection verification)
 * 
 *********************************************************************/


// libraries
#include <ArduinoBLE.h>
#include "ble_functions.h"
#include "buzzer_functions.h"


//Name of the controller
const char* deviceName = "meat";

bool startValueNil = true;  // If true, controller will calibrate STAY to the starting pressure value. 
int weightOffset = 50;      // if startValueNil, will recalculate up and down threshold based on calibration

// Pin definitions
const int BUZZER_PIN = 11;    // Pin for haptic feedback buzzer
const int LED_PIN_BLE = 7;    // Status LED pin, also indicated end of calibration period 
const int sensorPin = A0;  // sensor pin

// Movement state tracking
int currentMovement = 0;  // Current movement value (0=none, 1=up, 2=down, 3=handshake)


// sensor variables:
int sensorValue = 0;       // the sensor value
int sensorMin = 0;         // minimum sensor value
int sensorMax = 1023;      // maximum sensor value

//threshholds
int stillThreshold;      // Holds the starting pressure during calibration  
int downThreshold = 30;  // highest value for "DOWN", used if !startValueNil
int upThreshold = 120;   // lowest value for "UP", used if !startValueNil

// How frequently to check for input
int interval = 5000;


void setup() {

  Serial.begin(9600);


  // Configure LED for connection status indication
  pinMode(LED_PIN_BLE, OUTPUT);

  // Initialize Bluetooth Low Energy with device name and status LED
  setupBLE(deviceName, LED_PIN_BLE);

  // Initialize buzzer for feedback
  setupBuzzer(BUZZER_PIN);

  // Calibrate sensors to find up/down/still thresholds
  calibrateSensorStart();

  debugText();
}

  // calibrate during the first five seconds - From Arduino examples
void calibrateSensorStart() {

  while (millis() < 5000) {
    sensorValue = analogRead(sensorPin);

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }

    // check to see if the thresholds should be recalculated dynamically, otherwise run regular calibration
    if (startValueNil) {
      CalculateStartPositions();
    } else {
      // record the minimum sensor value
      if (sensorValue > sensorMin) {
        sensorMin = sensorValue;
      }
    }

  }
}


// recalculate thresholds dynamically from calibrated 'start' pressure
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
}

void loop() {
  // Update BLE connection status and handle incoming data
  updateBLE();

  // get the sensor value, adjusted for calibration
  sensorInput();

  //read the inputs te determine the current state
  //results in changing the value of currentMovement
  handleInput();

  //send the movement state to P5
  sendMovement(currentMovement);

  // send movement state to the buzzer
  updateBuzzer(currentMovement);
}


// prints calibration data after startup
void debugText() {
  Serial.print("min: ");
  Serial.print(sensorMin);
  Serial.print(" max: ");
  Serial.print(sensorMax);
  Serial.print(" pressure: ");
  Serial.println(sensorValue);
}

// prints additional sensor information - static for each playthrough, but changes on recalibration 
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

void handleInput() {
  if (sensorValue > upThreshold) {
    currentMovement = 1;  // UP movement
    sensorRead();
    Serial.println("UP");
  } else if (sensorValue < downThreshold) {
    currentMovement = 2;  // DOWN movement
    sensorRead();
    Serial.println("DOWN");
  } else {
    currentMovement = 0;  // No movement
    sensorRead();
    Serial.println("stay");
  }
}
