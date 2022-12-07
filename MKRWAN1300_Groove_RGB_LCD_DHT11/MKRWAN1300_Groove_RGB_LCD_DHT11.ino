#include <DHT.h>
#include <Wire.h>
#include <MKRWAN.h>
#include "rgb_lcd.h"
#include "arduino_secrets.h"

#define DHTTYPE DHT11
#define DHTPIN 0           //Setting the D0 pin as input pin to take data from the temperature sensor
DHT dht(DHTPIN, DHTTYPE);

rgb_lcd lcd;

LoRaModem modem;

String appEui = SECRET_APP_EUI;   // Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appKey = SECRET_APP_KEY;

const int colorR = 64;
const int colorG = 64;
const int colorB = 64;
const int pinTemp = A0;      // pin of temperature sensor

float humidity;  //Stores humidity value
float temperature; //Stores temperature value
float temp;

void setup() 
{
  lcd.begin(16, 2);       // set up the LCD's number of columns and rows:
  lcd.setRGB(colorR, colorG, colorB);    
  Serial.begin(9600);     //Baud rate for the serial communication of Arduino
  //while (!Serial);      // wait for serial port to connect. Needed for native USB port only
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    lcd.setCursor(0, 0);
    lcd.print("Failed to start module");
    while (1) {}
  };
  Serial.println();
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());
  Serial.println();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  
  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  } else {
    Serial.println("Connected to LoRaWAN");
    lcd.setCursor(0, 0);
    lcd.print("Connected to   ");
    lcd.setCursor(0, 1);
    lcd.print("LoRaWAN");
    delay(750);
  }
  modem.minPollInterval(60);  // Set poll interval to 60 secs.
//    delay(1000);
    dht.begin();
}


void color(float temp)
{
  if(temp > 21)
  {
    lcd.setRGB(64, 0, 0);
  }
  else if(temp < 20)
  {
    lcd.setRGB(0, 0, 64);
  }
  else
  {
    lcd.setRGB(0, 64, 0);
  }
}

void loop() {
  uint16_t humidity = dht.readHumidity(false);
  uint16_t temperature = dht.readTemperature(false) * 10;       

  // Split both words (16 bits) into 2 bytes of 8
  byte payload[5];
  payload[0] = 01;    // I use Elsys payload encoding and 0x01 means Temperature https://www.elsys.se/en/elsys-payload/
  payload[1] = highByte(temperature);
  payload[2] = lowByte(temperature);
  payload[3] = 02;    // I use Elsys payload encoding and 0x01 means Humidity https://www.elsys.se/en/elsys-payload/
  payload[4] = lowByte(humidity);

  for (int i = 0; i <= 600; i++) {
    // Read sensor values and multiply by 100 to effictively have 2 decimals
    uint16_t humidity = dht.readHumidity(false); 
    float floattemp = dht.readTemperature(false); // false: Celsius (default), true: Farenheit
    uint16_t temperature = dht.readTemperature(false) * 10;       
    Serial.print("Temperature: ");
    Serial.println(floattemp);
    Serial.print("Humidity: ");
    Serial.println(humidity);
  
    color(floattemp);                                      //decide which colour
    lcd.setCursor(0, 0);
    lcd.print("Temperatur: ");                                    //Print on LCD
    lcd.setCursor(12, 0);
    lcd.print(floattemp);
    lcd.setCursor(0, 1);
    lcd.print("Luftfukt  : ");                                    //Print on LCD
    lcd.setCursor(12, 1);
    lcd.print(humidity);
      delay(1000);
  }
  int i =0;
  int err;
  modem.beginPacket();
  modem.write(payload[0]);
  modem.write(payload[1]);
  modem.write(payload[2]);
  modem.write(payload[3]);
  modem.write(payload[4]);
  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  }
}
