// #include <tinyekf.hpp>

/*this is an implementation of mouse control using the gyroscope of the LSM6DSOX IMU
An Extended Kalman Filter is used to smooth the acceleration values
which are scaled and used directly for control input
the KB2040 board was used to test this implmentation using usb hid and we are still
converting this to use on the feather nRF52840 sense board with BLe HID
  */

constexpr int EKF_N = 8; //  States: Y angular disp, Y ang vel, Z ang disp, Z ang vel, pointer, middle, thumb
// Want 9: Y lin Acc, Y lin Vel, Z lin Acc, Z lin Vel, Y ang Vel, Z ang Vel, pointer, middle, thumb, ring, pinky
constexpr int EKF_M = 6; //  Measurements: Y ang vel, Z ang vel, pointer, middle, thumb, ring, pinky
// Want 7: Y lin Acc, Z lin Acc, Y ang Vel, Z ang Vel, pointer, middle, thumb, ring pinky

#include <bluefruit.h>
// #include <Adafruit_NeoPixel.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_Sensor.h>
#include <tinyekf.h>
// #include <Mouse.h>
#include <gesture.h>
#include <math.h>
#include <Adafruit_TinyUSB.h>

BLEDis bledis;
BLEHidAdafruit blehid;

static const float EPS = 1.5e-6;

static const float Pdiag[EKF_N] = {0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001};

// static const float Q[EKF_N * EKF_N] = {

//     EPS, 0, 0, 0,
//     0, EPS * 10, 0, 0,
//     0, 0, EPS, 0,
//     0, 0, 0, EPS * 10};
static const float Q[EKF_N * EKF_N] = {

    EPS, 0, 0, 0, 0, 0, 0, 0,
    0, EPS * 10, 0, 0, 0, 0, 0, 0,
    0, 0, EPS, 0, 0, 0, 0, 0,
    0, 0, 0, EPS * 10, 0, 0, 0, 0,
    0, 0, 0, 0, EPS * 10, 0, 0, 0,
    0, 0, 0, 0, 0, EPS * 10, 0, 0,
    0, 0, 0, 0, 0, 0, EPS * 10, 0,
    0, 0, 0, 0, 0, 0, 0, EPS * 10};

// static const float R[EKF_M * EKF_M] = {
//     // 2x2

//     0.0005,
//     0,
//     0,
//     0.0005,
// };

static const float ER[EKF_M * EKF_M] = {
    // USED TO BE THE R MATRIX
    0.0005, 0, 0, 0, 0, 0,
    0, 0.0005, 0, 0, 0, 0,
    0, 0, 0.0005, 0, 0, 0,
    0, 0, 0, 0.0005, 0, 0,
    0, 0, 0, 0, 0.0005, 0,
    0, 0, 0, 0, 0, 0.0005};

// So process model Jacobian is identity matrix
// static const float F[EKF_N*EKF_N] = {   //
//     1, 1, 0, 0,
//     0, 1, 0, 0,
//     0, 0, 1, 1,
//     1, 1, 0, 1
// };

// static const float H[EKF_M * EKF_N] = {
//     0,
//     1,
//     0,
//     0,
//     0,
//     0,
//     0,
//     1,
// };

static const float H[EKF_M * EKF_N] = {
    0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 1};

static ekf_t _ekf;

Adafruit_LSM6DSOX sox;
float prev = 0.0;
float curr = 0.0;
float dt = 0.01;

// constexpr int MIDDLE_FINGER_PIN = A0;
// constexpr int INDEX_FINGER_PIN = A1;
// constexpr int THUMB_PIN = A2;
// constexpr int RING_FINGER_PIN = A5;
// constexpr int PINKY_FINGER_PIN = A4;

int middleFinger;
int indexFinger;
int thumb;
int ringFinger;
int pinkyFinger;

// needed for gesture key presses
uint8_t emptyKeycode[6] = {0};
uint8_t activeKey[6] = {0};

bool mouseEnabled = true;
bool toggleMouse = false;

float indexFiltered = 0.0;
float middleFiltered = 0.0;
float thumbFiltered = 0.0;
float pinkyFiltered = 0.0;
float ringFiltered = 0.0;

float oldIndexFiltered = 0.0;
float oldMiddleFiltered = 0.0;
float oldThumbFiltered = 0.0;
float oldPinkyFiltered = 0.0;
float oldRingFiltered = 0.0;

float my = 0.0;
float mz = 0.0;

float ax = 0.0;
float ay = 0.0;
float az = 0.0;

float gx = 0.0;
float gy = 0.0;
float gz = 0.0;

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

constexpr Gestures LEFT_CLICK_GESTURE = I;
constexpr Gestures RIGHT_CLICK_GESTURE = M;
constexpr Gestures DRAG_GESTURE = TI;
constexpr Gestures LASER_GESTURE = T;
constexpr Gestures DISABLE_MOUSE_GESTURE = TIMRP;
constexpr Gestures SNIP_GESTURE = TMRP;
constexpr Gestures ALT_F4_GESTURE = TIRP;
constexpr Gestures ALT_TAB_GESTURE = MRP;
constexpr Gestures ALT_SHIFT_TAB_GESTURE = IRP;
constexpr Gestures ZOOM_GESTURE = TP;
constexpr Gestures SCROLL_GESTURE = TRP;

constexpr int MOUSE_SENSITIVITY = 20;

constexpr float oneThousandth = 1.0 / 1000.0;

enum GestureState
{
  IDLE,
  LEFT_CLICK_EVENT,
  RIGHT_CLICK_EVENT,
  DRAG_EVENT,
  LASER,
  SNIP,
  ALT_F4,
  ALT_TAB,
  ALT_SHIFT_TAB,
  DISABLE_MOUSE,
  ZOOM,
  SCROLL
};

GestureState gestureState = IDLE;
Gestures previousGesture = NONE;
Gestures currentGesture = NONE;

// the setup routine runs once when you press reset:
void setup()
{

  Serial.begin(115200);
  while (!Serial)
  {
    Serial.println("cannot proceed!");
    delay(10);
  }

  Serial.println("Motion Control Glove - Starting");

  ekf_initialize(&_ekf, Pdiag);

  if (!sox.begin_SPI(1))
  {
    // if (!sox.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
    // Serial.println("Failed to find LSM6DSOX chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("LSM6DSOX Found!");
  // Configure accelerometer/gyroscope
  sox.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  sox.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  sox.setAccelDataRate(LSM6DS_RATE_208_HZ);
  sox.setGyroDataRate(LSM6DS_RATE_208_HZ);

  configBluetooth();

  Bluefruit.begin();
  Bluefruit.setTxPower(4); // Check bluefruit.h for supported values

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather 52");
  bledis.begin();

  // BLE HID
  blehid.begin();

  // Set up and start advertising
  startAdv();
  pinMode(22, OUTPUT); // move the laser to a digital pin instead of analog
  digitalWrite(22, LOW);
}

// the loop routine runs over and over again forever:
void loop()
{
  curr = millis();

  dt = (curr - prev) * oneThousandth;
  prev = curr;

  middleFinger = analogRead(A0);
  indexFinger = analogRead(A1);
  thumb = analogRead(A2);
  ringFinger = analogRead(A5);
  pinkyFinger = analogRead(A4);

  sox.getEvent(&accel, &gyro, &temp);

  ax = accel.acceleration.x;
  ay = accel.acceleration.y + 0.32;
  az = accel.acceleration.z - 10.03;

  gx = gyro.gyro.x;
  gy = 0.01 + gyro.gyro.y;
  gz = 0.01 + gyro.gyro.z;

  const float z[EKF_M] = {gy, gz, indexFinger, middleFinger, thumb, ringFinger};

  const float F[EKF_N * EKF_N] = {
      1, dt, 0, 0, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 0, 0, 0,
      0, 0, 1, dt, 0, 0, 0, 0,
      0, 0, 0, 1, 0, 0, 0, 0,
      0, 0, 0, 0, 1, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0,
      0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 1};

  // Process model f(x)
  const float fx[EKF_N] = {_ekf.x[0] + dt * _ekf.x[1], _ekf.x[1], _ekf.x[2] + dt * _ekf.x[3], _ekf.x[3], _ekf.x[4], _ekf.x[5], _ekf.x[6], _ekf.x[7]}; // velocity y , velocity z

  // Run the prediction step of the eKF
  ekf_predict(&_ekf, fx, F, Q);

  const float hx[EKF_M] = {_ekf.x[1], _ekf.x[3], _ekf.x[4], _ekf.x[5], _ekf.x[6], _ekf.x[7]};
  //   hx[2] = .9987 * this->x[1] + .001;

  // const float hx[EKF_M] = {_ekf.x[0], _ekf.x[1] };

  // Run the update step
  ekf_update(&_ekf, z, hx, H, ER);

  // }
  // else{
  //   vx = 0;
  //   vy = 0;
  // };

  // scale the acceleration values by 20
  /*the scaling factor was empiricaly determined. We need to find why this works
  to see if there may be a more optimal value*/
  my = MOUSE_SENSITIVITY * _ekf.x[1];
  mz = -MOUSE_SENSITIVITY * _ekf.x[3];

  //set old values to check rate of change 
  oldIndexFiltered = indexFiltered;
  oldMiddleFiltered = middleFiltered;
  oldThumbFiltered = thumbFiltered;
  oldPinkyFiltered = pinkyFiltered;
  oldRingFiltered = ringFiltered;



  indexFiltered = _ekf.x[4];
  middleFiltered = _ekf.x[5];
  thumbFiltered = _ekf.x[6];
  ringFiltered = _ekf.x[7];
  pinkyFiltered = pinkyFinger;

  

  if (mouseEnabled)
  {
    blehid.mouseMove(mz, my);
  }

  ////
  ////GESTURE TESTING
  previousGesture = currentGesture;
  currentGesture = gesture(thumbFiltered, indexFiltered, middleFiltered, ringFiltered, pinkyFiltered);
  Serial.println(currentGesture);


  //Testing gesture change might need variable per finger
  /*if(fabs(indexFiltered - oldIndexFiltered) > 1 ||
  fabs(middleFiltered - oldMiddleFiltered) > 1 ||
  fabs(thumbFiltered - oldThumbFiltered) > 1 ||
  fabs(ringFiltered - oldRingFiltered) > 1 ||
  fabs(pinkyFiltered - oldPinkyFiltered) > 1  )
  Serial.print("Δindex: ");
  Serial.print(fabs(indexFiltered - oldIndexFiltered), 3);
  Serial.print(" | Δmiddle: ");
  Serial.print(fabs(middleFiltered - oldMiddleFiltered), 3);
  Serial.print(" | Δthumb: ");
  Serial.print(fabs(thumbFiltered - oldThumbFiltered), 3);
  Serial.print(" | Δring: ");
  Serial.print(fabs(ringFiltered - oldRingFiltered), 3);
  Serial.print(" | Δpinky: ");
  Serial.println(fabs(pinkyFiltered - oldPinkyFiltered), 3);

  {
    
    previousGesture = currentGesture;
    currentGesture = gesture(thumbFiltered, indexFiltered, middleFiltered, ringFiltered, pinkyFiltered);
    Serial.println(currentGesture);
  }

  else 
  {
  
  }*/

  switch (gestureState)
  {
  case IDLE:
    if (currentGesture == DRAG_GESTURE)
    {
      blehid.mouseButtonPress(MOUSE_BUTTON_LEFT);
      gestureState = DRAG_EVENT;
    }

    else if (currentGesture == LEFT_CLICK_GESTURE)
    {
      blehid.mouseButtonPress(MOUSE_BUTTON_LEFT);
      gestureState = LEFT_CLICK_EVENT;
      mouseEnabled = false;
    }

    else if (currentGesture == RIGHT_CLICK_GESTURE)
    {
      blehid.mouseButtonPress(MOUSE_BUTTON_RIGHT);
      gestureState = RIGHT_CLICK_EVENT;
      mouseEnabled = false;
    }

    else if (currentGesture == LASER_GESTURE)
    {
      mouseEnabled = false;
      digitalWrite(22, HIGH);
      gestureState = LASER;
    }

    else if (currentGesture == DISABLE_MOUSE_GESTURE)
    {
      toggleMouse = !toggleMouse;
      mouseEnabled = false;
      gestureState = DISABLE_MOUSE;
    }

    else if (currentGesture == SNIP_GESTURE)
    {
      activeKey[0] = HID_KEY_S; // 0x16
      // 0xA should work (left shift + left windows)
      blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTSHIFT + KEYBOARD_MODIFIER_LEFTGUI, activeKey); // previously 0x28
      gestureState = SNIP;
    }

    else if (currentGesture == ALT_F4_GESTURE)
    {
      mouseEnabled = false;
      activeKey[0] = HID_KEY_F4;                                                            // 0x3D
      blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTALT, activeKey); // 0x04
      gestureState = ALT_F4;
    }

    else if (currentGesture == ALT_TAB_GESTURE)
    {
      mouseEnabled = false;
      activeKey[0] = HID_KEY_TAB;                                                           // 0x2B
      blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTALT, activeKey); // 0x04
      gestureState = ALT_TAB;
    }

    else if (currentGesture == ALT_SHIFT_TAB_GESTURE)
    {
      mouseEnabled = false;
      activeKey[0] = HID_KEY_TAB; // 0x2B
      // 0x6 should work (left alt + left shift)
      blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTALT + KEYBOARD_MODIFIER_LEFTSHIFT, activeKey); // previously 0x24
      gestureState = ALT_SHIFT_TAB;
    }

    else if (currentGesture == SCROLL_GESTURE)
    {
      mouseEnabled = false;

      // scroll up
      if (ay >= 1)
      {
        blehid.mouseScroll(1);
        gestureState = SCROLL;
      }

      // scroll down
      else
      {
        blehid.mouseScroll(-1);
        gestureState = SCROLL;
      }
    }

    else if (currentGesture == ZOOM_GESTURE)
    {
      mouseEnabled = false;

      // zoom in
      if (ay >= 1)
      {
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTCTRL, emptyKeycode); // 0x01
        blehid.mouseScroll(-1);
        gestureState = ZOOM;
      }

      // zoom out
      else
      {
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTCTRL, emptyKeycode); // 0x01
        blehid.mouseScroll(1);
        gestureState = ZOOM;
      }
    }
    break;

  case LEFT_CLICK_EVENT:
    if (currentGesture == NONE)
    {
      mouseEnabled = true;
      blehid.mouseButtonRelease(MOUSE_BUTTON_LEFT);
      gestureState = IDLE;
    }
    else if (previousGesture != currentGesture)
    {
      mouseEnabled = true;
      gestureState = IDLE;
    }
    break;

  case RIGHT_CLICK_EVENT:
    if (currentGesture == NONE)
    {
      mouseEnabled = true;
      blehid.mouseButtonRelease(MOUSE_BUTTON_RIGHT);
      gestureState = IDLE;
    }
    else if (previousGesture != currentGesture)
    {
      mouseEnabled = true;
      gestureState = IDLE;
    }
    break;

  case DRAG_EVENT:
    if (currentGesture == NONE || previousGesture != currentGesture)
    {
      blehid.mouseButtonRelease(MOUSE_BUTTON_LEFT);
      gestureState = IDLE;
    }
    break;

  case LASER:
    if (currentGesture == NONE || previousGesture != currentGesture)
    {
      mouseEnabled = true;
      digitalWrite(22, LOW);
      gestureState = IDLE;
    }
    break;

  case DISABLE_MOUSE:
    if (!toggleMouse)
    {
      mouseEnabled = true;
    }
    gestureState = IDLE;
    break;

  case SNIP:
    if (currentGesture == NONE || previousGesture != currentGesture)
    {
      blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, HID_KEY_NONE, emptyKeycode); // 0
      blehid.keyRelease();
      gestureState = IDLE;
    }
    break;

  case ALT_F4:
    if (currentGesture == NONE || previousGesture != currentGesture)
    {
      blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, HID_KEY_NONE, emptyKeycode); // 0
      blehid.keyRelease();
      gestureState = IDLE;
    }
    break;

  case ALT_TAB:
    if (currentGesture == NONE || previousGesture != currentGesture)
    {
      blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, HID_KEY_NONE, emptyKeycode); // 0
      blehid.keyRelease();
      gestureState = IDLE;
    }
    break;

  case ALT_SHIFT_TAB:
    if (currentGesture == NONE || previousGesture != currentGesture)
    {
      blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, HID_KEY_NONE, emptyKeycode); // 0
      blehid.keyRelease();
      gestureState = IDLE;
    }
    break;

  case SCROLL:
    if (currentGesture == NONE || previousGesture != currentGesture)
    {
      mouseEnabled = true;
      blehid.mouseScroll(0);
      gestureState = IDLE;
    }
    break;

  case ZOOM:
    if (currentGesture == NONE || previousGesture != currentGesture)
    {
      blehid.mouseScroll(0);
      blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, HID_KEY_NONE, emptyKeycode); // 0
      blehid.keyRelease();
      mouseEnabled = true;
      gestureState = IDLE;
    }
    break;
  }

  // Serial.print(_ekf.x[1]); Serial.print(",");
  // Serial.print(_ekf.x[3]); Serial.print(",");
  // Serial.print(gy); Serial.print(",");
  // Serial.print(gz); Serial.print(",");

  //  Serial.print(ax); Serial.print(",");
  // Serial.print(ay); Serial.print(",");
  // Serial.print(az); Serial.print(",");
  // Serial.print(pointer_1);
  // Serial.print(",");
  // Serial.print(middle_1);
  // Serial.print(",");
  // Serial.print(rightclick);
  // Serial.print(",");

  // Serial.print(leftclick);
  // Serial.print(",");
  Serial.print("thumb: ");
  Serial.print(thumbFiltered); // middle
  // Serial.print(",");

  Serial.print(" pointer: ");
  Serial.print(indexFiltered); // pointer
  // Serial.print(",");
  Serial.print(" middle: ");
  Serial.print(middleFiltered); // thumb

  // Serial.print(",");
  Serial.print(" ring: ");
  Serial.print(ringFiltered); // ring
                              // Serial.print(",");

  Serial.print(" pinky: ");
  Serial.print(pinkyFiltered); // pinky
  Serial.print(" ");

  Serial.print(curr * oneThousandth);
  Serial.println();
  Serial.print("mouseEnabled: %d\n", mouseEnabled);
  Serial.print("toggleMouse: %d", toggleMouse);
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_MOUSE);

  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for 'Name' in the advertising packet
  Bluefruit.Advertising.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   *
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
  Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds
}

void configBluetooth()
{
  // add any bluetooth config stuff here
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.configPrphConn(100, 8, 4, 4);
}
