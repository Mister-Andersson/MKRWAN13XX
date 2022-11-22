#include <OneWire.h>
#include <DallasTemperature.h>
#include <MKRWAN.h>
 
  // Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
 
  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
  // Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

LoRaModem modem;
 
  // Uncomment if using the Murata chip as a module
  // LoRaModem modem(Serial1);
 
#include "arduino_secrets.h"
  // Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;
 
void setup() {
  // Initiate sensors
  sensors.begin();
  
  Serial.begin(9600);
  while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.println();
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());
  Serial.println();
  
  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  } else {
    Serial.println("Connected to LoRaWAN");
  }
 
  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independently by this setting the modem will
  // not allow to send more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed. */
}
 
void loop() {
  int temp;
  sensors.requestTemperatures();
  temp = 10 * sensors.getTempCByIndex(0);
  float temp_float = temp * 0.1;
  
  Serial.println(); 
  Serial.print("Temperature: ");
  Serial.println(temp_float);
  byte payload[3];      // Split both words (16 bits) into 2 bytes of 8
  payload[0] = 01;
  payload[1] = highByte(temp);
  payload[2] = lowByte(temp);

  Serial.print("Payload: ");
  for(int i = 0; i < 3; i++)
{
  Serial.print(payload[i], HEX);
  Serial.print(" ");
}
  Serial.println();
  
  int err;
  modem.beginPacket();
//  modem.print(temp);
  modem.write(payload[0]);
  modem.write(payload[1]);
  modem.write(payload[2]);
  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  }
delay(600000);
}
