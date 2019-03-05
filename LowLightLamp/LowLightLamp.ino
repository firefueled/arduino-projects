#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

/*
Connection:
 VCC-5v
 GND-GND
 SCL-SCL(analog pin 5)
 SDA-SDA(analog pin 4)
 ADD-NC or GND
*/

int relayOutputPin = 12;

const int lowLightCountTrigger = 10; // minimum low light events needed to trigger the lamp
const int lowLightLevel = 3; // analog read level at which low light events are triggered

int lowLightCount = 0; // count for the low light events

void setup(){
  // Serial.begin(9600);
  lightMeter.begin();
  lightMeter.configure(0x10);

  pinMode(relayOutputPin, OUTPUT);
  digitalWrite(relayOutputPin, HIGH); // reverse
}

void loop() {
  uint16_t lux = lightMeter.readLightLevel();

  //count low light level events
  if (lux < lowLightLevel) {
    if (lowLightCount <= lowLightCountTrigger) {
      lowLightCount = lowLightCount + 1;
    }
  } else {
    if (lowLightCount > 0) {
      lowLightCount = lowLightCount - 1;
    }
  }

  // turn on light when enough low light level events have been detected
  if (lowLightCount >= lowLightCountTrigger) {
    digitalWrite(relayOutputPin, LOW); // reverse
  } else if (lowLightCount == 0) {
    digitalWrite(relayOutputPin, HIGH); // reverse
  }

  delay(1000);
}
