  // Requires MKRWAN Library
#include <MKRWAN.h>

void setup() {
  //Initialize modem
  LoRaModem modem;
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());
}

void loop() {
  // put your main code here, to run repeatedly:

}
