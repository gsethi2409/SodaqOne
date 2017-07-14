/*
 * Compatible with:
 * SODAQ MBILI
 * SODAQ Autonomo
 * SODAQ ONE
 * SODAQ ONE BETA
 * SODAQ EXPLORER
 */

#include "Arduino.h"
#include <Sodaq_RN2483.h>

#define debugSerial SERIAL_PORT_MONITOR

#if defined(ARDUINO_AVR_SODAQ_MBILI)
#define loraSerial Serial1
#define BEE_VCC 20

#elif defined(ARDUINO_SODAQ_AUTONOMO) || defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA)
#define loraSerial Serial1

#elif defined(ARDUINO_SODAQ_EXPLORER)
#define loraSerial Serial2

#else
// please select a sodaq board
debugSerial.println("Please select a sodaq board!!");
#endif

// ABPs
const uint8_t devAddr[4] = { 0x26, 0x01, 0x10, 0xDB };

//const uint8_t appSKey[16] = { 0xFB, 0x98, 0x7F, 0x6A, 0x39, 0xD3, 0xE1, 0xDC, 0x42, 0x6B, 0x01, 0xA8, 0x39, 0x3D, 0xFC, 0xE3 };
//const uint8_t nwkSKey[16] = { 0x58, 0x94, 0x85, 0xCB, 0x62, 0x4A, 0x69, 0x11, 0x54, 0xE2, 0xBC, 0xDF, 0x7B, 0x19, 0xE3, 0x99 };

//const uint8_t appSKey[16] = { 0x7C, 0xC2, 0x56, 0xD6, 0x1F, 0x11, 0x2B, 0xAB, 0xC4, 0x08, 0xB8, 0xD5, 0x59, 0xB4, 0xFB, 0x11 };
//onst uint8_t nwkSKey[16] = { 0xEE, 0x39, 0x82, 0x2E, 0x0C, 0xE2, 0xDC, 0xDB, 0x95, 0x7D, 0x5F, 0x49, 0xDC, 0xBF, 0x5D, 0x5A };

//const uint8_t appSKey[16] = { 0x32, 0x81, 0x79, 0x53, 0xE2, 0xDF, 0x31, 0x05, 0xBD, 0x8C, 0x61, 0x75, 0xA8, 0x1E, 0x73, 0x72 };
//const uint8_t nwkSKey[16] = { 0x2C, 0xAB, 0x67, 0x83, 0x8E, 0x16, 0x4E, 0xC3, 0x26, 0x1D, 0xAD, 0xB8, 0xB0, 0xDF, 0x64, 0xF6 };

const uint8_t appSKey[16] = { 0xDB, 0x5A, 0x43, 0x97, 0x6C, 0xFD, 0x07, 0x9E, 0xF2, 0x9C, 0xC4, 0xE2, 0xB2, 0x3E, 0x76, 0xF0 };
const uint8_t nwkSKey[16] = { 0x69, 0x34, 0x91, 0xE5, 0x10, 0xF4, 0x02, 0x41, 0x01, 0x49, 0x6A, 0x52, 0xAD, 0x76, 0x06, 0x2D };



// OTAA
uint8_t DevEUI[8] = { 0x00, 0x04, 0xA3, 0x0B, 0x00, 0x1B, 0xEF, 0x52 };
uint8_t AppEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xF0, 0x00, 0x61, 0xF7 };
uint8_t AppKey[16] = { 0x2B, 0x64, 0xFC, 0xDE, 0x4F, 0xEB, 0x9F, 0xF5, 0x35, 0xE8, 0xCB, 0x7B, 0x89, 0xC5, 0x9D, 0x2A };


const char *diag[] = {"sys get hweui", "mac get dr", "radio get sf", "mac get adr","mac get devaddr", "mac get deveui", "mac get band", "mac set pwridx 1", "mac get pwridx", "mac get retx", "mac get gwnb", "mac set deveui 0004A30B001BEF52", "mac set appeui 70B3D57EF00061F7", "mac set appkey 2B64FCDE4FEB9FF535E8CB7B89C59D2A", "mac save", "mac join otaa",NULL};
char buffer[128];

void setupLoRaABP(){  
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, false))
  {
    debugSerial.println("Communication to LoRaBEE successful.");
  }
  else
  {
    debugSerial.println("Communication to LoRaBEE failed!");
  }
}

void setupLoRaOTAA(){
  if (LoRaBee.initOTA(loraSerial, DevEUI, AppEUI, AppKey, false))
  {
    debugSerial.println("Communication to LoRaBEE successful.");
  }
  else
  {
    debugSerial.println("OTAA Setup failed!");
  }
}

void send(const char *s, bool wait = true) {
  //
  Serial1.println(s);
  //
  if (wait) {
    SerialUSB.print(s);
    SerialUSB.write(" ");
  }
  else
    SerialUSB.println(s);
  if (wait) {
    *buffer = 0;
    int i = Serial1.readBytesUntil('\n', buffer, sizeof(buffer));
    if (i) buffer[i - 1] = 0;
    SerialUSB.println((char *) buffer);
  }
}

void setup() {
  //Power up the LoRaBEE
  pinMode(ENABLE_PIN_IO, OUTPUT);
  digitalWrite(ENABLE_PIN_IO, HIGH);
  #if defined(ARDUINO_AVR_SODAQ_MBILI) || defined(ARDUINO_SODAQ_AUTONOMO)
  pinMode(BEE_VCC, OUTPUT);
  digitalWrite(BEE_VCC, HIGH);
  #endif
  delay(3000);

  while ((!SerialUSB) && (millis() < 10000)){
    // Wait 10 seconds for the Serial Monitor
  }

  //Set baud rate
  debugSerial.begin(57600);
  loraSerial.begin(LoRaBee.getDefaultBaudRate());
 

  //connect to the LoRa Network
  
  setupLoRa();

  for (int x = 0; diag[x] != NULL; x++){
    send(diag[x]);
    delay(1000);
  }
}

void setupLoRa(){
  // ABP
  //setupLoRaABP();
  // OTAA
  //setupLoRaOTAA();
  LoRaBee.setSpreadingFactor(7);
}

void sendPacket(String packet){
  switch (LoRaBee.sendReqAck(1, (uint8_t*)packet.c_str(), packet.length(), 3))
    {
    case NoError:
      debugSerial.println("Successful transmission.");
      break;
    case NoResponse:
      debugSerial.println("There was no response from the device.");
      setupLoRa();
      break;
    case Timeout:
      debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
      delay(20000);
      break;
    case PayloadSizeError:
      debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
      break;
    case InternalError:
      debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! Try restarting the device!\r\nThe network connection will reset.");
      setupLoRa();
      break;
    case Busy:
      debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
      delay(10000);
      break;
    case NetworkFatalError:
      debugSerial.println("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe network connection will reset.");
      setupLoRa();
      break;
    case NotConnected:
      debugSerial.println("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe network connection will reset.");
      setupLoRa();
      break;
    case NoAcknowledgment:
      debugSerial.println("There was no acknowledgment sent back!");
      // When you this message you are probaly out of range of the network.
      break;
    default:
      break;
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  String packet = "SODAQ";
  sendPacket(packet);
  delay(50000);
}
