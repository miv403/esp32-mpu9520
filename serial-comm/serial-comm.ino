#include <Wire.h>
#include <MPU9250.h>

#define CMD_CALIBRATE   0x01
#define CMD_RECV_CALIB  0x02
#define CMD_GET_SENSOR  0x03

#define RESP_OK           0xAA
#define RESP_ERR          0xFF
#define RESP_CALIB_DONE   0x10
#define RESP_CALIB_RCVD   0x11
#define RESP_SENSOR_MODE  0x12
#define RESP_SENSOR_SENT  0x13

MPU9250 mpu;

struct CalibrationData {
  float accBias[3];
  float gyroBias[3];
} calib;

struct SensorData {
  float gyro[3];
  float acc[3];
}sensorDat;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  bool start_loop = false;
  while(!start_loop) {
    if (Serial.available()) {
      uint8_t cmd = Serial.read();

      if        (cmd == CMD_CALIBRATE) {
        Serial.write(RESP_OK);
        calibrate();
        Serial.write(RESP_CALIB_DONE);

        // Serial.write(0xAA);
        // Serial.write(0x55);

        Serial.write((uint8_t*)&calib, sizeof(CalibrationData));
      } else if (cmd == CMD_RECV_CALIB) {
        while   (Serial.available() < sizeof(CalibrationData));
        Serial.readBytes( (char*)&calib, sizeof(CalibrationData));
      } else if (cmd == CMD_GET_SENSOR) {
        start_loop = true;
      }
    }
  }
}

void loop() {
  if (Serial.available()) {
    uint8_t cmd = Serial.read();

    if (cmd == CMD_GET_SENSOR) {
      float data[9] = {
        mpu.getAccX(), mpu.getAccY(), mpu.getAccZ(),
        mpu.getGyroX(), mpu.getGyroY(), mpu.getGyroZ(),
        mpu.getMagX(), mpu.getMagY(), mpu.getMagZ()
      };
      Serial.write((byte*)data, sizeof(data));
    }
  }
}

void calibrate() {

    // calibrate anytime you want to
    // Serial.println("Accel Gyro calibration will start in 5sec.");
    // Serial.println("Please leave the device still on the flat plane.");
    mpu.verbose(true);
    // delay(3000);
    mpu.calibrateAccelGyro();

    // Serial.println("Mag calibration will start in 5sec.");
    // Serial.println("Please Wave device in a figure eight until done.");
    // delay(3000);
    // mpu.calibrateMag();

    setCalibrationData();

    // print_calibration();
    mpu.verbose(false);
}

void setCalibrationData() {
  calib.accBias[0]  = mpu.getAccBiasX();
  calib.accBias[1]  = mpu.getAccBiasY();
  calib.accBias[2]  = mpu.getAccBiasZ();

  calib.gyroBias[0] = mpu.getGyroBiasX();
  calib.gyroBias[1] = mpu.getGyroBiasY();
  calib.gyroBias[2] = mpu.getGyroBiasZ();
}

/*
 * 
  if(Serial.available) {
    sensorDat.gyro[0] = mpu.getGyroX();
    sensorDat.gyro[1] = mpu.getGyroY();
    sensorDat.gyro[2] = mpu.getGyroZ();
    sensorDat.acc[0] = mpu.getAccX();
    sensorDat.acc[1] = mpu.getAccY();
    sensorDat.acc[2] = mpu.getAccZ();

    Serial.write(0xAA);
    Serial.write(0x55);

    Serial.write((uint8_t*)&sensorDat, sizeof(sensorDat));
  }
 */
