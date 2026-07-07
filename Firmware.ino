#include <Wire.h>
#include <SPI.h>
#include <Adafruit_ISM330DHCX.h>

// Define SPI pins according to our custom hardware schematic map
#define IMU_CS   10
#define IMU_SCK  12
#define IMU_MOSI 11
#define IMU_MISO 13

// Create the sensor object using hardware SPI configuration
Adafruit_ISM330DHCX ism330;

void setup(void) {
  // Initialize native USB Serial
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for USB serial port to connect (needed for native USB-C)
  }

  Serial.println("--- ESP32-S3 ISM330DHCX Custom Board Test ---");

  // Initialize the IMU using hardware SPI pins passed to the library wrapper
  // We explicitly override the default pins to match our custom layout mapping
  if (!ism330.begin_SPI(IMU_CS, IMU_SCK, IMU_MISO, IMU_MOSI)) {
    Serial.println("Error: Could not find a valid ISM330DHCX chip over SPI! Check hardware connections.");
    while (1) {
      delay(10);
    }
  }

  Serial.println("ISM330DHCX successfully initialized over SPI!");

  // --- Configure Sensor Ranges & Data Rates ---
  // Accelerometer range: +/- 4G
  ism330.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
  
  // Gyroscope range: +/- 500 degrees per second
  ism330.setGyroRange(LSM6DS_GYRO_RANGE_500_DPS);

  // Set Output Data Rate (ODR) to 104 Hz for stable telemetry streaming
  ism330.setAccelDataRate(LSM6DS_RATE_104_HZ);
  ism330.setGyroDataRate(LSM6DS_RATE_104_HZ);

  Serial.println("Sensor operational configurations locked.");
}

void loop() {
  // Instantiating normalized event storage structures
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;

  // Fetch packed metrics over the SPI bus interface
  ism330.getEvent(&accel, &gyro, &temp);

  // --- Print Telemetry Stream over Native USB-C Data Lines ---
  Serial.print("ACCEL (m/s^2) -> X: ");
  Serial.print(accel.acceleration.x, 3);
  Serial.print(" \tY: ");
  Serial.print(accel.acceleration.y, 3);
  Serial.print(" \tZ: ");
  Serial.print(accel.acceleration.z, 3);

  Serial.print("  |  GYRO (rad/s) -> X: ");
  Serial.print(gyro.gyro.x, 4);
  Serial.print(" \tY: ");
  Serial.print(gyro.gyro.y, 4);
  Serial.print(" \tZ: ");
  Serial.println(gyro.gyro.z, 4);

  // Sample delay throttling to match the 104 Hz device refresh rate interval (~10ms)
  delay(10);
}
