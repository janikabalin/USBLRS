#include "Adafruit_TinyUSB.h"

// -----------------------------
// HID SETUP
// -----------------------------
Adafruit_USBD_HID usb_hid;

uint8_t const desc_hid_report[] = {
  0x05, 0x01,       // Usage Page (Generic Desktop)
  0x09, 0x05,       // Usage (Game Pad)
  0xA1, 0x01,       // Collection (Application)
  0x15, 0x81,       //   Logical Minimum (-127)
  0x25, 0x7F,       //   Logical Maximum (127)
  0x75, 0x08,       //   Report Size (8)
  0x95, 0x04,       //   Report Count (4)
  0x09, 0x30,       //   Usage (X)
  0x09, 0x31,       //   Usage (Y)
  0x09, 0x32,       //   Usage (Z)
  0x09, 0x35,       //   Usage (Rz)
  0x81, 0x02,       //   Input (Data, Variable, Absolute)
  0x05, 0x09,       // Usage Page (Button)
  0x19, 0x01,       // Usage Minimum (Button 1)
  0x29, 0x08,       // Usage Maximum (Button 8)
  0x15, 0x00,       // Logical Minimum (0)
  0x25, 0x01,       // Logical Maximum (1)
  0x75, 0x01,       // Report Size (1)
  0x95, 0x08,       // Report Count (8)
  0x81, 0x02,       // Input (Data, Variable, Absolute)
  0xC0              // End Collection
};

typedef struct {
  int8_t x;
  int8_t y;
  int8_t z;
  int8_t rZ;
  uint8_t buttons; // 8 bits = 8 buttons
} __attribute__((packed)) JoystickReport;

JoystickReport joyReport = {0};

// -----------------------------
// SBUS SETUP
// -----------------------------
uint8_t sbusFrame[25];
uint16_t channels[16];

void setup() {
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  Serial1.begin(100000, SERIAL_8E2); // SBUS
  Serial.begin(115200);             // Debug (optional)
  delay(200);
}

bool readSbus() {
  static uint8_t buffer[25];
  static uint8_t byteIndex = 0;

  while (Serial1.available()) {
    uint8_t b = Serial1.read();
    if (b == 0x0F && byteIndex == 0) {
      buffer[byteIndex++] = b;
    } else if (byteIndex > 0) {
      buffer[byteIndex++] = b;
      if (byteIndex == 25) {
        memcpy(sbusFrame, buffer, 25);
        byteIndex = 0;
        return true;
      }
    }
  }
  return false;
}

void decodeSbus() {
  uint8_t* b = sbusFrame;

  channels[0]  = ((b[1]     | b[2]  << 8)                         & 0x07FF);
  channels[1]  = ((b[2] >> 3 | b[3]  << 5)                        & 0x07FF);
  channels[2]  = ((b[3] >> 6 | b[4]  << 2 | b[5] << 10)           & 0x07FF);
  channels[3]  = ((b[5] >> 1 | b[6]  << 7)                        & 0x07FF);
  channels[4]  = ((b[6] >> 4 | b[7]  << 4)                        & 0x07FF);
  channels[5]  = ((b[7] >> 7 | b[8]  << 1 | b[9] << 9)            & 0x07FF);
  channels[6]  = ((b[9] >> 2 | b[10] << 6)                        & 0x07FF);
  channels[7]  = ((b[10] >> 5 | b[11] << 3)                       & 0x07FF);
  channels[8]  = ((b[12]    | b[13] << 8)                         & 0x07FF);
  channels[9]  = ((b[13] >> 3 | b[14] << 5)                       & 0x07FF);
  channels[10] = ((b[14] >> 6 | b[15] << 2 | b[16] << 10)         & 0x07FF);
  channels[11] = ((b[16] >> 1 | b[17] << 7)                       & 0x07FF);
  channels[12] = ((b[17] >> 4 | b[18] << 4)                       & 0x07FF);
  channels[13] = ((b[18] >> 7 | b[19] << 1 | b[20] << 9)          & 0x07FF);
  channels[14] = ((b[20] >> 2 | b[21] << 6)                       & 0x07FF);
  channels[15] = ((b[21] >> 5 | b[22] << 3)                       & 0x07FF);
}

int8_t sbusToAxis(uint16_t val) {
  int32_t norm = map(val, 172, 1811, -127, 127);
  return (int8_t)constrain(norm, -127, 127);
}

bool sbusHigh(uint16_t val) {
  return val > 1000; // Adjust threshold as needed
}

void loop() {
  if (readSbus()) {
    decodeSbus();

    // Axes
    joyReport.x  = sbusToAxis(channels[0]); // Roll
    joyReport.y  = sbusToAxis(channels[1]); // Pitch
    joyReport.z  = sbusToAxis(channels[2]); // Throttle
    joyReport.rZ = sbusToAxis(channels[3]); // Yaw

    // Buttons from CH5–12
    joyReport.buttons = 0;
    for (int i = 4; i < 12; i++) {
      if (sbusHigh(channels[i])) {
        joyReport.buttons |= (1 << (i - 4));
      }
    }

    usb_hid.sendReport(0, &joyReport, sizeof(joyReport));

    // Optional debug
    /*
    Serial.print("Buttons: ");
    Serial.println(joyReport.buttons, BIN);
    */
  }

  //delay(20);
}