/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <bluefruit.h>

BLEDis bledis;
BLEHidAdafruit blehid;

#define MOVE_STEP    10

void setup() 
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Bluefruit52 HID Mouse Example");
  Serial.println("-----------------------------\n");
  Serial.println("Go to your phone's Bluetooth settings to pair your device");
  Serial.println("then open an application that accepts mouse input");
  Serial.println();

  Serial.println("Enter following characters");
  Serial.println("- 'WASD'  to move mouse (up, left, down, right)");
  Serial.println("- 'LRMBF' to press mouse button(s) (left, right, middle, backward, forward)");
  Serial.println("- 'X'     to release mouse button(s)");

  Bluefruit.begin();
  // HID Device can have a min connection interval of 9*1.25 = 11.25 ms
  Bluefruit.Periph.setConnInterval(9, 16); // min = 9*1.25=11.25 ms, max = 16*1.25=20ms
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather 52");
  bledis.begin();

  // BLE HID
  blehid.begin();

  // Set up and start advertising
  startAdv();
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
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void loop() 
{    
  if (Serial.available())
  {
    char ch = (char) Serial.read();

    // convert to upper case
    ch = (char) toupper(ch);
    
    // echo
    Serial.println(ch);
    uint8_t emptyKeycode[6] = {0};
    uint8_t activeKey[6] = {0};
    switch(ch)
    {
      // WASD to move the mouse
      case 'W':
        blehid.mouseMove(0, -MOVE_STEP);
      break;

      case 'A':
        blehid.mouseMove(-MOVE_STEP, 0);
      break;

      case 'S':
        blehid.mouseMove(0, MOVE_STEP);
      break;

      case 'D':
        blehid.mouseMove(MOVE_STEP, 0);
      break;

      // LRMBF for mouse button(s)
      case 'L':
        blehid.mouseButtonPress(MOUSE_BUTTON_LEFT);
      break;

      case 'R':
        blehid.mouseButtonPress(MOUSE_BUTTON_RIGHT);
      break;

      case 'M':
        blehid.mouseButtonPress(MOUSE_BUTTON_MIDDLE);
      break;

      case 'B':
        blehid.mouseButtonPress(MOUSE_BUTTON_BACKWARD);
      break;

      case 'F':
        // This key is not always supported by every OS
        blehid.mouseButtonPress(MOUSE_BUTTON_FORWARD);
      break;

      case 'X':
        // X to release all buttons
        blehid.mouseButtonRelease();
      break;

      case '[':
           
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x01, emptyKeycode);  // Hold Ctrl (0x01 = Left Ctrl) is modifier for control
        blehid.mouseScroll(-1);
        blehid.mouseScroll(0);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode); // Release Ctrl modifier
        blehid.keyRelease();
      break;

      case ']':
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x01, emptyKeycode);  // Hold Ctrl (0x01 = Left Ctrl) is modifier for control
        blehid.mouseScroll(1);
        blehid.mouseScroll(0);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode); // Release Ctrl modifier
        blehid.keyRelease();
      break;

      case '1': //alt f4
        activeKey[0] = 0x3D;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x04, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

    case '2': //alt tab //works but not as intended, only swaps between most recent tabs instead of allowing scrolling
        activeKey[0] = 0x2B;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x04, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case '3': //alt shift tab //might change to a polling event were a finger trigger holds alt and cant hit tab as many times as you want until release
        activeKey[0] = 0x2B;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x24, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case '4': //win d
        activeKey[0] = 0x07;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x08, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case '5': //win L
        activeKey[0] = 0x0f;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x08, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case '6': // win e
        activeKey[0] = 0x08;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x08, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case '7': // snip
        activeKey[0] = 0x16;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x28, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case '8': // printscreen
        activeKey[0] = 0x46;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x04, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case '9': // ctrl win 1
        activeKey[0] = 0x1e;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x28, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case '0': // win home
        activeKey[0] = 0x4A;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x08, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case 'c': // copy //not working
        activeKey[0] = 0x06;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x01, activeKey);
        delay(10000);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      case 'v': // paste //not working
        activeKey[0] = 0x19;
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0x02, activeKey);
        blehid.keyboardReport(BLE_CONN_HANDLE_INVALID, 0, emptyKeycode);
        blehid.keyRelease();
      break;

      default: break;
    }
  }
}
