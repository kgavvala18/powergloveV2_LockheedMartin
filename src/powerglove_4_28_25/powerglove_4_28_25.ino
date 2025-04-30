// #include <tinyekf.hpp>

/*this is an implementation of mouse control using the gyroscope of the LSM6DSOX IMU
An Extended Kalman Filter is used to smooth the acceleration values
which are scaled and used directly for control input
the KB2040 board was used to test this implmentation using usb hid and we are still
converting this to use on the feather nRF52840 sense board with BLe HID
  */

constexpr int EKF_MOTION_N = 4; //  States: Y lin Acc, X lin Acc, Y ang Vel, Z ang Vel, 
constexpr int EKF_MOTION_M = 4; //  Measurements:  Y lin Acc, X lin Acc, Y ang Vel, Z ang Vel,

constexpr int EKF_FLEX_N = 5; // pointer, middle, thumb, ring, pinky
constexpr int EKF_FLEX_M = 5; // pointer, middle, thumb, ring, pinky

#include <bluefruit.h>
// #include <Adafruit_NeoPixel.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_Sensor.h>
#include <tinyekf_motion.h>
#include <tinyekf_flex.h>
// #include <Mouse.h>
#include <gesture.h>
#include <math.h>
#include <Adafruit_TinyUSB.h>

BLEDis bledis;
BLEHidAdafruit blehid;

// sensor variables
float indexFiltered = 0.0;
float middleFiltered = 0.0;
float thumbFiltered = 0.0;
float pinkyFiltered = 0.0;
float ringFiltered = 0.0;

int middleFinger;
int indexFinger;
int thumb;
int ringFinger;
int pinkyFinger;

float my = 0.0; //mouse movement 
float mz = 0.0;

float ax = 0.0; // raw acceleration
float ay = 0.0;

float a_x = 0.0; //filtered acc
float a_y = 0.0;

float gy = 0.0;  // raw gyroscope values
float gz = 0.0;  

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;
//end motion variables

//motion filter
static const float EPS = 1e-6;

static const float Pdiag[EKF_MOTION_N] = {0.001, 0.001, 0.001, 0.001};


static const float Q[EKF_MOTION_N * EKF_MOTION_N] = {

    EPS, 0, 0, 0, 
    0, EPS, 0, 0, 
    0, 0, EPS, 0, 
    0, 0, 0, EPS};

static const float ER[EKF_MOTION_M * EKF_MOTION_M] = {
    // USED TO BE THE R MATRIX
    0.002, 0, 0, 0,
    0, 0.002, 0, 0, 
    0, 0, 0.002, 0,
    0, 0, 0, 0.002};

static const float H[EKF_MOTION_M * EKF_MOTION_N] = {
    1, 0, 0, 0, 
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1};

static ekf_t_motion _ekf;

//flex sensor filter 
static const float EPS_ = 1e-6;

static const float Pdiag_flex[EKF_FLEX_N] = {0.001, 0.001, 0.001, 0.001, 0.001};

static const float Q_flex[EKF_FLEX_N * EKF_FLEX_N] = {
    EPS_, 0, 0, 0, 0,
    0, EPS_, 0, 0, 0, 
    0, 0, EPS_, 0, 0,
    0, 0, 0, EPS_, 0,
    0, 0, 0, 0, 1e-10 };

static const float ER_flex[EKF_FLEX_M * EKF_FLEX_M] = {
    // USED TO BE THE R MATRIX
    0.002, 0, 0, 0, 0,
    0, 0.002, 0, 0, 0,
    0, 0, 0.002, 0, 0,
    0, 0, 0, 0.002, 0,
    0, 0, 0, 0, 0.1};

static const float H_flex[EKF_FLEX_M * EKF_FLEX_N] = {
    1, 0, 0, 0, 0,
    0, 1, 0, 0, 0,
    0, 0, 1, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 0, 1};

static ekf_t_flex _ekf_flex;



Adafruit_LSM6DSOX sox;  
float prev = 0.0;
float curr = 0.0;
float dt = 0.01;

// constexpr int MIDDLE_FINGER_PIN = A0;
// constexpr int INDEX_FINGER_PIN = A1;
// constexpr int THUMB_PIN = A2;
// constexpr int RING_FINGER_PIN = A5;
// constexpr int PINKY_FINGER_PIN = A4;

// needed for gesture key presses
uint8_t emptyKeycode[6] = {0};
uint8_t activeKey[6] = {0};

bool mouseEnabled = true;
bool toggleMouse = false;


constexpr Gestures LEFT_CLICK_GESTURE = I;
constexpr Gestures RIGHT_CLICK_GESTURE = M;
constexpr Gestures DRAG_GESTURE = TI;
constexpr Gestures LASER_GESTURE = T;
constexpr Gestures DISABLE_MOUSE_GESTURE = TIMRP;
constexpr Gestures SNIP_GESTURE = TMRP;
constexpr Gestures ALT_F4_GESTURE = TIRP;
constexpr Gestures ALT_TAB_GESTURE = MRP;
constexpr Gestures WIN_TAB_GESTURE = IRP;
constexpr Gestures ZOOM_GESTURE = TP;
constexpr Gestures SCROLL_GESTURE = TRP;

int MOUSE_SENSITIVITY = 20;

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
    WIN_TAB,
    DISABLE_MOUSE,
    ZOOM,
    SCROLL
};

GestureState gestureState = IDLE;
Gestures previousGesture = NONE;
Gestures currentGesture = NONE;

const int button_none[] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
const int button0[] = {1, 0, 0, 0, 0, 0, 1, 0, 1};
const int button1[] = {1, 0, 0, 0, 0, 1, 1, 0, 1};
const int button2[] = {1, 0, 0, 0, 0, 0, 1, 1, 1};
const int button3[] = {1, 1, 0, 0, 1, 0, 0, 0, 0};
const int button4[] = {1, 1, 0, 1, 1, 0, 0, 0, 0};
const int button5[] = {1, 0, 0, 0, 0, 1, 1, 1, 0};
const int button6[] = {1, 0, 0, 0, 0, 1, 1, 1, 1};
const int button7[] = {1, 0, 0, 0, 1, 0, 0, 0, 0};
const int button8[] = {1, 0, 0, 1, 1, 0, 0, 0, 0};
const int button9[] = {1, 0, 1, 0, 1, 0, 0, 0, 0};
const int buttonProg[] = {1, 1, 1, 1, 1, 0, 0, 0, 0};
const int buttonEnter[] = {1, 0, 1, 1, 1, 0, 0, 0, 0};
const int leftArrow[] = {1, 0, 0, 0, 0, 1, 1, 0, 0};
const int upArrow[] = {1, 0, 0, 0, 0, 0, 1, 1, 0};
const int rightArrow[] = {1, 0, 0, 0, 0, 0, 1, 0, 0};
const int downArrow[] = {1, 0, 0, 0, 0, 1, 0, 1, 1};
const int buttonCenter[] = {1, 0, 0, 0, 0, 0, 0, 1, 1};
const int buttonSelect[] = {1, 1, 1, 1, 0, 0, 0, 0, 0};
const int buttonStart[] = {1, 1, 1, 0, 0, 0, 0, 0, 0};
const int buttonB[] = {1, 1, 1, 0, 1, 0, 0, 0, 0};
const int buttonA[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
const int numButtons = 21;

const int *buttonSignatures[] = {
    button0, button1, button2, button3, button4, button5, button6,
    button7, button8, button9, buttonProg, buttonEnter, leftArrow,
    upArrow, rightArrow, downArrow, buttonCenter, buttonSelect,
    buttonStart, buttonB, buttonA};

// const char *buttonNames[] = {
//     "Button 0", "Button 1", "Button 2", "Button 3", "Button 4",
//     "Button 5", "Button 6", "Button 7", "Button 8", "Button 9",
//     "Button Prog", "Button Enter", "Left Arrow", "Up Arrow",
//     "Right Arrow", "Down Arrow", "Button Center", "Button Select",
//     "Button Start", "Button B", "Button A"};

const int inputPins[] = {2, 5, 6, 9, 10, 11, 12, 13, 23};
const int numPins = 9;
int pinReadings[numPins];

bool pressed = false;
bool program = false;

// Find what button is being pressed
int matchButton(const int *pinReadings)
{
    // Check if no button is pressed and return to save time
    bool none = true;
    for (int i = 0; i < numPins; i++)
    {
        if (button_none[i] != pinReadings[i])
        {
            none = false;
            break;
        }
    }

    if (none)
    {
        return -1;
    }

    for (int i = 0; i < numButtons; i++)
    {
        bool match = true;
        for (int j = 0; j < numPins; j++)
        {
            if (pinReadings[j] != buttonSignatures[i][j])
            {
                match = false;
                break;
            }
        }
        if (match)
            return i;
    }
    return -1;
}

// Function to send windows key + number or other key depending on button pressed
void pressModifierAndKey(uint8_t number)
{
    int modifier = 0x08;            // windows key
    if (number >= 0 && number <= 9) // windows plus num 1-9
    {
        activeKey[0] = HID_KEY_1 + number; // 0x1E
    }
    else if (number == 11) // enter = win + l
    {
        activeKey[0] = HID_KEY_L;
    }
    else if (number == 12) // left arrow = win + tab
    {
        activeKey[0] = HID_KEY_TAB;
    }
    else if (number == 13) // up arrow = alt + f4
    {
        modifier = KEYBOARD_MODIFIER_LEFTALT;
        activeKey[0] = HID_KEY_F4; // 0x2B
    }
    else if (number == 14) // right arrow = alt + tab
    {
        modifier = KEYBOARD_MODIFIER_LEFTALT; // 0x04
        activeKey[0] = HID_KEY_TAB;
    }
    else if (number == 15) // down arrow = win + d
    {
        activeKey[0] = HID_KEY_D; // 0x07
    }
    else if (number == 17) // select = win + shift + s (snipping tool)
    {
        modifier = modifier | KEYBOARD_MODIFIER_LEFTSHIFT; // windows + shift
        activeKey[0] = HID_KEY_S;                          // 0x16
    }
    else if (number == 18)
    {
        activeKey[0] = HID_KEY_E;
    }
    else if (number == 19) // B button = ctrl + v
    {
        modifier = KEYBOARD_MODIFIER_LEFTCTRL; // l-ctrl
        activeKey[0] = HID_KEY_V;              // 0x19
    }
    else if (number == 20) // A button = ctrl + c
    {
        modifier = KEYBOARD_MODIFIER_LEFTCTRL; // l-ctrl
        activeKey[0] = HID_KEY_C;              // 0x06
    }
    blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, modifier, activeKey);
    blehid.keyRelease();
}

// the setup routine runs once when you press reset:
void setup()
{

    Serial.begin(115200);
    while (!Serial)
    {
        Serial.println("cannot proceed!");
        delay(10);
    }

    // Serial.println("Motion Control Glove - Starting");

    ekf_initialize_motion(&_ekf, Pdiag);
    ekf_initialize_flex(&_ekf_flex, Pdiag_flex);    

    if (!sox.begin_SPI(1))
    {
        // if (!sox.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
        // Serial.println("Failed to find LSM6DSOX chip");
        while (1)
        {
            delay(10);
        }
    }
    // Serial.println("LSM6DSOX Found!");
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

    // Initialize button input pins
    for (int i = 0; i < numPins; i++)
    {
        pinMode(inputPins[i], INPUT);
    }

    // Set up and start advertising
    startAdv();
    pinMode(22, OUTPUT); // move the laser to a digital pin instead of analog
    digitalWrite(22, LOW);
}

void getMotionData(){
    sox.getEvent(&accel, &gyro, &temp);

    ax = accel.acceleration.x + 0.95;
    ay = accel.acceleration.y - 0.95;
    //az = accel.acceleration.z - 10.03;

    //gx = gyro.gyro.x;
    gy = 0.01 + gyro.gyro.y;
    gz = 0.01 + gyro.gyro.z;

    const float z[EKF_MOTION_M] = {ay, ax, gy, gz};

    //const float z[EKF_MOTION_M] = {gy, gz, indexFinger, middleFinger, thumb, ringFinger};
    const float F[EKF_MOTION_N * EKF_MOTION_N] = {
        1, 0, 0, 0,  
        0, 1, 0, 0,
        0, 0, 1, 0, 
        0, 0, 0, 1};

    // Process model f(x)
    const float fx[EKF_MOTION_N] = {_ekf.x[0] , _ekf.x[1], _ekf.x[2] , _ekf.x[3]}; 

    // Run the prediction step of the eKF
    ekf_predict_motion(&_ekf, fx, F, Q);

    const float hx[EKF_MOTION_M] = {_ekf.x[0], _ekf.x[1], _ekf.x[2], _ekf.x[3]};

    // Run the update step
    ekf_update_motion(&_ekf, z, hx, H, ER);
}


void getFlexData(){
    middleFinger = analogRead(A0);
    indexFinger = analogRead(A1);
    thumb = analogRead(A2);
    ringFinger = analogRead(A4);
    pinkyFinger = analogRead(A5);

    const float z[EKF_FLEX_M] = {thumb, indexFinger, middleFinger, ringFinger, pinkyFinger};

    //const float z[EKF_MOTION_M] = {gy, gz, indexFinger, middleFinger, thumb, ringFinger};
    const float F[EKF_FLEX_N * EKF_FLEX_N] = {
        1, 0, 0, 0, 0,  
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 1};

    // Process model f(x)
    const float fx[EKF_FLEX_N] = {_ekf_flex.x[0] , _ekf_flex.x[1], _ekf_flex.x[2], _ekf_flex.x[3], _ekf_flex.x[4] }; 

    // Run the prediction step of the eKF
    ekf_predict_flex(&_ekf_flex, fx, F, Q_flex);

    const float hx[EKF_FLEX_M] = {_ekf_flex.x[0], _ekf_flex.x[1], _ekf_flex.x[2], _ekf_flex.x[3], _ekf_flex.x[4]};

    // Run the update step
    ekf_update_flex(&_ekf_flex, z, hx, H_flex, ER_flex);

}

// the loop routine runs over and over again forever:
void loop()
{
    curr = millis();

    dt = (curr - prev) * oneThousandth;
    prev = curr;
    
    getMotionData();
    getFlexData();

    // scale the acceleration values by 20
    /*the scaling factor was empiricaly determined. We need to find why this works
    to see if there may be a more optimal value*/


    

    my = MOUSE_SENSITIVITY * _ekf.x[2];
    mz = -MOUSE_SENSITIVITY * _ekf.x[3];
    a_x = _ekf.x[1];

    thumbFiltered = _ekf_flex.x[0];
    indexFiltered = _ekf_flex.x[1];
    middleFiltered = _ekf_flex.x[2];
    
    ringFiltered = _ekf_flex.x[3];
    pinkyFiltered = _ekf_flex.x[4];
   
    if (mouseEnabled)
    {
        blehid.mouseMove(mz, my);
    }

    ////
    ////GESTURE TESTING
    previousGesture = currentGesture;
    //currentGesture = NONE;
    currentGesture = gesture(thumbFiltered, indexFiltered, middleFiltered, ringFiltered, pinkyFiltered);
    //currentGesture = gesture(thumb, indexFinger, middleFinger, ringFinger, pinkyFinger);

    //Serial.println(a_x);

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

        // else if (currentGesture == ALT_F4_GESTURE)
        // {
        //   mouseEnabled = false;
        //   activeKey[0] = HID_KEY_F4;                                                            // 0x3D
        //   blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTALT, activeKey); // 0x04
        //   gestureState = ALT_F4;
        // }

        else if (currentGesture == ALT_TAB_GESTURE)
        {
            mouseEnabled = false;
            activeKey[0] = HID_KEY_TAB;                                                           // 0x2B
            blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTALT, activeKey); // 0x04
            gestureState = ALT_TAB;
        }

        else if (currentGesture == WIN_TAB_GESTURE)
        {
            mouseEnabled = false;
            activeKey[0] = HID_KEY_TAB; // 0x2B
            // 0x6 should work (left alt + left shift)
            blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTGUI, activeKey); // previously 0x24
            gestureState = WIN_TAB;
        }

        else if (currentGesture == SCROLL_GESTURE)
        {
            mouseEnabled = false;

            // scroll up
            if (a_x > 2)
            {
                blehid.mouseScroll(1);
                //delay(200);
                gestureState = SCROLL;
            }

            // scroll down
            else if (a_x < -2)
            {
                blehid.mouseScroll(-1);
                //delay(200);
                gestureState = SCROLL;
            }
        }

        else if (currentGesture == ZOOM_GESTURE)
        {
            mouseEnabled = false;

            // zoom in
            if (a_x > 2)
            {
                blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTCTRL, emptyKeycode); // 0x01
                blehid.mouseScroll(-1);
                //delay(300);
                gestureState = ZOOM;
            }

            // zoom out
            else if (a_x < -2)
            {
                blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTCTRL, emptyKeycode); // 0x01
                blehid.mouseScroll(1);
                //delay(300);
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

        // case ALT_F4:
        //   if (currentGesture == NONE || previousGesture != currentGesture)
        //   {
        //     blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, HID_KEY_NONE, emptyKeycode); // 0
        //     blehid.keyRelease();
        //     gestureState = IDLE;
        //   }
        //   break;

    case ALT_TAB:
        if (currentGesture == NONE || previousGesture != currentGesture)
        {
            blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, HID_KEY_NONE, emptyKeycode); // 0
            blehid.keyRelease();
            gestureState = IDLE;
        }
        break;

    case WIN_TAB:
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
            break;
        }

        // scroll up
        if (a_x > 2)
        {
            blehid.mouseScroll(1);
            delay(300);
        }

        // scroll down
        else if (a_x < -2)
        {
            blehid.mouseScroll(-1);
            delay(300);
        }
        else{
            blehid.mouseScroll(0);
            //blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, HID_KEY_NONE, emptyKeycode); // 0
            //blehid.keyRelease();
           // mouseEnabled = true;
           // gestureState = IDLE;
            //break;
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
            break;
        }

        if (a_x > 2)
        {
            blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTCTRL, emptyKeycode); // 0x01
            blehid.mouseScroll(-1);
            delay(400);
        }

        // zoom out
        else if (a_x < -2)
        {
            blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, KEYBOARD_MODIFIER_LEFTCTRL, emptyKeycode); // 0x01
            blehid.mouseScroll(1);
            delay(400);
        }
        else{
            blehid.mouseScroll(0);
            //blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, HID_KEY_NONE, emptyKeycode); // 0
            //blehid.keyRelease();
            //mouseEnabled = true;
            //gestureState = IDLE;
            //break;
        }

        break;
    }

    // Handle buttons
    // Read buttons
    for (int i = 0; i < numPins; i++)
    {
        pinReadings[i] = digitalRead(inputPins[i]);
    }

    int index = matchButton(pinReadings);

    // if (index != -1)
    // {
    // Serial.print("Detected: ");
    // Serial.println(buttonNames[index]);
    // }

    if (!pressed)
    {
        if ((index >= 0 && index <= 9) || index == 11 || index == 12 || index == 14 || index == 17 || index == 18 || index == 19 || index == 20)
        {
            pressModifierAndKey(index);
            pressed = true;
            delay(100);
        }

        switch (index)
        {
        case 10: // prog button
            program = !program;
            pressed = true;
            delay(100);
            break;

        case 13: // up arrow = win+tab (task view)      // CHANGE TO ALT F4
            if (program)
            {
                if (MOUSE_SENSITIVITY < 50)
                    MOUSE_SENSITIVITY++;
                delay(100);
                break;
            }
            pressModifierAndKey(index);
            pressed = true;
            delay(100);
            break;

        case 15: // down arrow = win+d (show desktop)
            if (program)
            {
                if (MOUSE_SENSITIVITY > 5)
                    MOUSE_SENSITIVITY--;
                delay(100);
                break;
            }
            pressModifierAndKey(index);
            pressed = true;
            delay(100);
            break;

        case 16: // center button = center mouse on screen **NOT WORKING GREAT**
                 // possibly make into win +
            for (int i = 0; i < 10; i++)
            {
                blehid.mouseMove(-127, -127); // Move to corner before going to center
            }

            blehid.mouseMove(127, 110);
            blehid.mouseMove(90, 0);

            pressed = true;
            delay(100);
            break;
        }
    }
    else if (index == -1)
    {
        pressed = false;
    }

    // Serial.print(_ekf.x[0]); Serial.print(",");
    // Serial.print(_ekf.x[1]); Serial.print(",");
    // Serial.print(_ekf.x[2]); Serial.print(",");
    // Serial.print(_ekf.x[3]); Serial.print(",");

    // Serial.print(ay); Serial.print(",");
    // Serial.print(ax); Serial.print(",");
    // Serial.print(gy); Serial.print(",");
    // Serial.print(gz); Serial.print(",");

    // Serial.print(rightclick);
    // Serial.print(",");

    // Serial.print(leftclick);
    // Serial.print(",");
    //Serial.print("thumb: ");
    Serial.print(thumbFiltered); // middle
    Serial.print(",");

    //Serial.print(" pointer: ");
    Serial.print(indexFiltered); // pointer
    Serial.print(",");
   // Serial.print(" middle: ");
    Serial.print(middleFiltered); // thumb

    Serial.print(",");
    //Serial.print(" ring: ");
    Serial.print(ringFiltered); // ring
    Serial.print(",");

    //Serial.print(" pinky: ");
    Serial.print(pinkyFiltered); // pinky
    Serial.print(",");

    // Serial.print(MOUSE_SENSITIVITY);
    // Serial.print(" ");

    Serial.print(curr * oneThousandth);
    Serial.println();
    // Serial.print("mouseEnabled: %d\n", mouseEnabled);
    // Serial.print("toggleMouse: %d", toggleMouse);
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
