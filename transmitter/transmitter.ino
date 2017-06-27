
// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h>
#include <RH_RF95.h>

// Blinky on receipt
#define LED 13

/* for feather m0   */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  Serial1.begin(9600);
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

// FOR RECEIVING DATA FROM SERIAL PORT

// Example 3 - Receive with start- and end-markers

const byte numChars = 150;
char receivedChars[numChars];

int recvWithStartEndMarkers() {
  int numBytesReceived = 0;

  boolean newData = false;
  boolean recvInProgress = false;
  byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  // clear receive buffer
  memset(receivedChars, 0, numChars);

  while (Serial1.available() > 0) {
    rc = Serial1.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        numBytesReceived = ndx;
        recvInProgress = false;
        ndx = 0;
        break;
      }
    }
    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
  return numBytesReceived;
}
// END RECEIVING DATA FROM SERIAL PORT

int blinkTime = 0;
boolean ledState = false;

void loop()
{
  // receive data from serial port to transmit using the radio
  // receives into 'receivedChars'
  int numBytes = recvWithStartEndMarkers();
  if (numBytes) {
    Serial.print("Sending "); Serial.println(receivedChars);

    rf95.send((uint8_t *)receivedChars, numBytes);
    rf95.waitPacketSent();
  }
  else {
    if (blinkTime > 50000) {
      Serial.println("TX Nothing available!");
      blinkTime = 0;
      ledState = !ledState;
      if (ledState)
        digitalWrite(LED, HIGH);
      else
        digitalWrite(LED, LOW);
    }
  }
  blinkTime++;
}
