#include "Arduino.h"
#include <Sodaq_RN2483.h>

#define loraSerial Serial1


void setup() {
  SerialUSB.begin(9600);
  //Power up the LoRaBEE
  SerialUSB.println("Setting up SODAQ!");

  //Set baud rate
  
  loraSerial.begin(LoRaBee.getDefaultBaudRate());

  // Debug output from LoRaBee
  // LoRaBee.setDiag(debugSerial); // optional

  //connect to the LoRa Network
  setupLoRa();
}

void setupLoRa(){
  SerialUSB.print("In setupLoRa()");
  LoRaBee.setSpreadingFactor(7);
}

void sendPacket(String packet){
  SerialUSB.print("In sendPacket()");
  switch (LoRaBee.send(1, (uint8_t*)packet.c_str(), packet.length()))
    {
    case NoError:
      SerialUSB.println("Successful transmission.");
      break;
    case NoResponse:
      SerialUSB.println("There was no response from the device.");
      setupLoRa();
      break;
    case Timeout:
      SerialUSB.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
      delay(20000);
      break;
    case PayloadSizeError:
      SerialUSB.println("The size of the payload is greater than allowed. Transmission failed!");
      break;
    case InternalError:
      SerialUSB.println("Oh No! This shouldn't happen. Something is really wrong! Try restarting the device!\r\nThe network connection will reset.");
      setupLoRa();
      break;
    case Busy:
      SerialUSB.println("The device is busy. Sleeping for 10 extra seconds.");
      delay(10000);
      break;
    case NetworkFatalError:
      SerialUSB.println("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe network connection will reset.");
      setupLoRa();
      break;
    case NotConnected:
      SerialUSB.println("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe network connection will reset.");
      setupLoRa();
      break;
    case NoAcknowledgment:
      SerialUSB.println("There was no acknowledgment sent back!");
      // When you this message you are probaly out of range of the network.
      break;
    default:
      break;
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  SerialUSB.print("In loop()");
  String packet = "SODAQ";
  sendPacket(packet);

  delay(5000);
}

