/**
 * Magic Wand Data Collection (on button press)
 * 
 * Attach button to pin 2. Press button to start collection. Raw x, y, z 
 * acceleration data (in G) and x, y, z, rotational (gyroscope) data (in 
 * degrees/sec) are printed over the serial port. Note that Edge Impulse 
 * requires a timestamp (in milliseconds) column.
 * 
 * Made for the Arduino Nano 33 BLE Sense, which has a built-in IMU (LSM9DS1).
 * 
 * For use with the serial-data-collect-csv.py script to save CSV files.
 * 
 * Author: Shawn Hymel (EdgeImpulse, Inc.)
 * Date: July 7, 2022
 * License: Apache-2.0 (apache.org/licenses/LICENSE-2.0)
 */

#include <Arduino_LSM9DS1.h>

// Settings
#define BTN_PIN             2         // Button pin
#define LED_R_PIN           22        // Red LED pin

// Constants
#define CONVERT_G_TO_MS2    9.80665f  // Used to convert G to m/s^2
#define SAMPLING_FREQ_HZ    100       // Sampling frequency (Hz)
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ   // Sampling period (ms)
#define NUM_SAMPLES         100       // 100 samples at 100 Hz is 1 sec window

void setup() {

  // Enable button pin
  pinMode(BTN_PIN, INPUT_PULLUP);

  // Enable LED pin (RGB LEDs are active low)
  pinMode(LED_R_PIN, OUTPUT);
  digitalWrite(LED_R_PIN, HIGH);

  // Start serial
  Serial.begin(115200);

  // Start accelerometer (part of IMU)
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() {

  float acc_x;
  float acc_y;
  float acc_z;
  float gyr_x;
  float gyr_y;
  float gyr_z;
  unsigned long timestamp;
  unsigned long start_timestamp;

  // Wait for button press
  while (digitalRead(BTN_PIN) == 1);

  // Turn on LED to show we're recording
  digitalWrite(LED_R_PIN, LOW);

  // Print header
  Serial.println("timestamp,accX,accY,accZ,gyrX,gyrY,gyrZ");

  // Record samples in buffer
  start_timestamp = millis();
  for (int i = 0; i < NUM_SAMPLES; i++) {

    // Take timestamp so we can hit our target frequency
    timestamp = millis();
    
    // Read and convert accelerometer data to m/s^2
    IMU.readAcceleration(acc_x, acc_y, acc_z);
    acc_x *= CONVERT_G_TO_MS2;
    acc_y *= CONVERT_G_TO_MS2;
    acc_z *= CONVERT_G_TO_MS2;

    // Read gyroscope data (in degrees/sec)
    IMU.readGyroscope(gyr_x, gyr_y, gyr_z);

    // Print CSV data with timestamp
    Serial.print(timestamp - start_timestamp);
    Serial.print(",");
    Serial.print(acc_x);
    Serial.print(",");
    Serial.print(acc_y);
    Serial.print(",");
    Serial.print(acc_z);
    Serial.print(",");
    Serial.print(gyr_x);
    Serial.print(",");
    Serial.print(gyr_y);
    Serial.print(",");
    Serial.println(gyr_z);

    // Wait just long enough for our sampling period
    while (millis() < timestamp + SAMPLING_PERIOD_MS);
  }

  // Print empty line to transmit termination of recording
  Serial.println();

  // Turn off LED to show we're done
  digitalWrite(LED_R_PIN, HIGH);

  // Make sure the button has been released for a few milliseconds
  while (digitalRead(BTN_PIN) == 0);
  delay(100);
}
