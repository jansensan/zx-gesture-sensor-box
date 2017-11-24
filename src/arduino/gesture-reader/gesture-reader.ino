/****************************************************************

GESTURE READER

Program that reads the ZX sensor, and sends data via serial port.

References:
- https://github.com/sparkfun/SparkFun_ZX_Distance_and_Gesture_Sensor_Arduino_Library
- https://learn.sparkfun.com/tutorials/zx-distance-and-gesture-sensor-hookup-guide

****************************************************************/


#include <Wire.h>
#include <ZX_Sensor.h>


// constants
const int PORT_NUMBER = 9600;
const int ZX_ADDR = 0x10;    // ZX Sensor I2C address
const int INTERRUPT_NUM = 0; // Pin 2 on the UNO
const String CHARS[52] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                          "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
                          "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
                          "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};


// global Variables
ZX_Sensor zxSensor = ZX_Sensor(ZX_ADDR);
uint8_t gestureSpeed;
volatile GestureType gesture;
volatile bool isInterrupted;


// arduino methods
void setup() {
  // init gesture to "no gesture"
  gesture = NO_GESTURE;

  // init serial port
  Serial.begin(PORT_NUMBER);

  // init ZX Sensor (configure I2C and read model ID)
  zxSensor.init(GESTURE_INTERRUPTS);

  // init interrupt service routine
  isInterrupted = false;
  zxSensor.clearInterrupt();
  attachInterrupt(INTERRUPT_NUM, interruptRoutine, RISING);
}

void loop() {
  // If we have an interrupt, read and print the gesture
  if (isInterrupted) {

    // Clear the interrupt flag
    isInterrupted = false;

    // You MUST read the STATUS register to clear interrupt!
    zxSensor.clearInterrupt();

    // Read last gesture
    gesture = zxSensor.readGesture();

    // Get speed. Note: higher speed numbers mean slower
    gestureSpeed = zxSensor.readGestureSpeed();

    switch (gesture) {
      case NO_GESTURE:
        sendGesture("NONE", 0);
        break;
      case RIGHT_SWIPE:
        sendGesture("RIGHT", gestureSpeed);
        break;
      case LEFT_SWIPE:
        sendGesture("LEFT", gestureSpeed);
        break;
      case UP_SWIPE:
        sendGesture("UP", gestureSpeed);
        break;
      default:
        break;
    }
  }
}


// method definitions
String createID() {
  String id = "";
  int ID_LENGTH = 4;
  int index;
  for (int i = 0; i < ID_LENGTH; i++) {
    index = random(52);
    id += CHARS[index];
  }
  return id;
}

void sendGesture(String gestureDirection, int gestureSpeed) {
  String id = createID();
  Serial.print(id);
  Serial.print(",");
  Serial.print(gestureDirection);
  Serial.print(",");
  Serial.print(gestureSpeed, DEC);
  Serial.print(";");
}

void interruptRoutine() {
  isInterrupted = true;
}

