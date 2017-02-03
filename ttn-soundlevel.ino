#include <rn2xx3.h>
#include <SoftwareSerial.h>

/* Copy and fill in the lines from TTN Console -> Devices -> Overview tab -> "EXAMPLE CODE"
.. And add this to a file in the same folder as this sketch: */
#include "ttn-config.h"

const char *devAddr = config_devAddr;
const char *nwkSKey = config_nwkSKey;
const char *appSKey = config_appSKey;

SoftwareSerial mySerial(7, 8); // RX, TX
#define RST  2

// Configure sound sensor
int sound_din=2;
int sound_ain=A0;
int ad_value;

// Configure transmission cycle
const uint16_t cycle_frequency = 200; // how often do we wish to measure?
const uint16_t cycle_length = 30000; // how often do we transmit?
const uint16_t cycle_over = cycle_length / cycle_frequency;

rn2xx3 myLora(mySerial);

// Setup routine runs once when you press reset
void setup() {
  pinMode(13, OUTPUT);
  led_on();

  // Activate sound sensor
  pinMode(sound_din,INPUT);
  pinMode(sound_ain,INPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Startup");

  // Reset rn2483
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
  digitalWrite(RST, LOW);
  delay(500);
  digitalWrite(RST, HIGH);

  // Initialise the rn2483 module
  myLora.autobaud();

  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.print("RN2483 version number: ");
  Serial.println(myLora.sysver());

  myLora.initABP(devAddr, appSKey, nwkSKey);

  led_off();
  delay(2000);

  Serial.println("Starting to transmit at cycles of (seconds):");
  Serial.println(cycle_length / 1000);
}


// transmit the average values to TTN
void sendLora(uint16_t value) {
  led_on();
  byte payload[2];
  payload[0] = highByte(value);
  payload[1] = lowByte(value);
  Serial.println("Transmitting:");
  Serial.println(value);
  myLora.txBytes(payload, sizeof(payload));
  led_off();
}

int cycle = 0;
int loudCount = 0;
float loudAvg = 0;

// the loop routine runs over and over again forever:
void loop() {

  ad_value=analogRead(sound_ain);

  if (digitalRead(sound_din) == LOW) {
    loudCount++;
  }
  
  loudAvg = (loudAvg == 0) ? ad_value : (loudAvg + ad_value) / 2;

  cycle++;
  if (cycle >= cycle_over) {
    //Serial.println(ad_value);
    //Serial.println(loudAvg);
    //Serial.println(loudCount);
    //Serial.println("---");
    sendLora((uint16_t)loudAvg);
    cycle = 0;
    loudCount = 0;
    loudAvg = 0;
  }
  
  delay(cycle_frequency);
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}
