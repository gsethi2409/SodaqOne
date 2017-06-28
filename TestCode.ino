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

// ABP
//const uint8_t devAddr[4] = { 0x00, 0x00, 0x00, 0x00 };
//const uint8_t appSKey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//const uint8_t nwkSKey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// OTAA
uint8_t DevEUI[8] = { 0x00, 0x04, 0xA3, 0x0B, 0x00, 0x1B, 0xEF, 0x52 };
uint8_t AppEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xF0, 0x00, 0x5E, 0xC1 };
uint8_t AppKey[16] = { 0x84, 0xF7, 0x4C, 0x3D, 0x32, 0x70, 0x2F, 0xC3, 0x66, 0x53, 0x9D, 0x5F, 0xB6, 0x50, 0xC8, 0x94 };

/*void setupLoRaABP(){  
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, false))
  {
    debugSerial.println("Communication to LoRaBEE successful.");
  }
  else
  {
    debugSerial.println("Communication to LoRaBEE failed!");
  }
}*/

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

void setup() {
  //Power up the LoRaBEE
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

  // Debug output from LoRaBee
  // LoRaBee.setDiag(debugSerial); // optional

  //connect to the LoRa Network
  setupLoRa();
}

void setupLoRa(){
  // ABP
//  setupLoRaABP();
  // OTAA
  setupLoRaOTAA();
  LoRaBee.setSpreadingFactor(11);
  LoRaBee.setPowerIndex(1);
}

void sendPacket(String packet){
  switch (LoRaBee.send(1, (uint8_t*)packet.c_str(), packet.length()))
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

  delay(5000);
}

