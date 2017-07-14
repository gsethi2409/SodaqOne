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
#define loraSerial Serial1

// ABPs
const uint8_t devAddr[4] = { 0x26, 0x01, 0x10, 0xDB };
const uint8_t appSKey[16] = { 0xDB, 0x5A, 0x43, 0x97, 0x6C, 0xFD, 0x07, 0x9E, 0xF2, 0x9C, 0xC4, 0xE2, 0xB2, 0x3E, 0x76, 0xF0 };
const uint8_t nwkSKey[16] = { 0x69, 0x34, 0x91, 0xE5, 0x10, 0xF4, 0x02, 0x41, 0x01, 0x49, 0x6A, 0x52, 0xAD, 0x76, 0x06, 0x2D };

// OTAA
uint8_t DevEUI[8] = { 0x00, 0x04, 0xA3, 0x0B, 0x00, 0x1B, 0xEF, 0x52 };
uint8_t AppEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xF0, 0x00, 0x61, 0xF7 };
uint8_t AppKey[16] = { 0x2B, 0x64, 0xFC, 0xDE, 0x4F, 0xEB, 0x9F, 0xF5, 0x35, 0xE8, 0xCB, 0x7B, 0x89, 0xC5, 0x9D, 0x2A };


const char *diag[] = {"sys get hweui", "mac get dr", "radio get sf", "mac get adr","mac get devaddr", "mac get deveui", "mac get band", "mac set pwridx 1", "mac get pwridx", "mac get retx", "mac get gwnb",NULL}; 
//"mac set deveui 0004A30B001BEF52", "mac set appeui 70B3D57EF00061F7", "mac set appkey 2B64FCDE4FEB9FF535E8CB7B89C59D2A", "mac save", "mac join otaa",NULL};
char buffer[128];

void RED() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

void GREEN() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, HIGH);
}

void BLUE() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
}

void WHITE(){
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
}

void setupLoRaABP(){  
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, false))
  {
    debugSerial.println("Communication to LoRaBEE successful.");
    BLUE();
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
    BLUE();
  }
  else
  {
    debugSerial.println("OTAA Setup failed!");
  }
}

void send(const char *s, bool wait = true) {
  Serial1.println(s);
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
  pinMode(ENABLE_PIN_IO, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  
  delay(3000);
  
  digitalWrite(ENABLE_PIN_IO, HIGH);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

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
  }
}

void setupLoRa(){
  // ABP
  //setupLoRaABP();
  // OTAA
  setupLoRaOTAA();
  //LoRaBee.setSpreadingFactor(7);
}

void sendPacket(String packet){
  LoRaBee.setSpreadingFactor(12);
  switch (LoRaBee.sendReqAck(1, (uint8_t*)packet.c_str(), packet.length(), 3))
    {
    case NoError:
      debugSerial.println("Successful transmission.");
      GREEN();
      break;
    case NoResponse:
      debugSerial.println("There was no response from the device.");
      RED();
      setupLoRa();
      break;
    case Timeout:
      debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
      RED();
      delay(20000);
      break;
    case PayloadSizeError:
      debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
      RED();
      break;
    case InternalError:
      debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! Try restarting the device!\r\nThe network connection will reset.");
      RED();
      setupLoRa();
      break;
    case Busy:
      debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
      RED();
      delay(10000);
      break;
    case NetworkFatalError:
      debugSerial.println("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe network connection will reset.");
      RED();
      setupLoRa();
      break;
    case NotConnected:
      debugSerial.println("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe network connection will reset.");
      RED();
      setupLoRa();
      break;
    case NoAcknowledgment:
      debugSerial.println("There was no acknowledgment sent back!");
      RED();
      // When you this message you are probaly out of range of the network.
      break;
    default:
      break;
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  String packet = "SODAQ";
  WHITE();
  sendPacket(packet);
  delay(60000);
}
