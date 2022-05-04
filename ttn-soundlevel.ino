#include <TheThingsNetwork.h>

/* Copy and fill in the lines from TTN Console -> Devices -> Overview tab -> "EXAMPLE CODE"
.. And add this to a file in the same folder as this sketch: */
const char *appEui = "70B3D57ED00503BC";
const char *appKey = "EDF58A46E1D792FA6406FE8ED44291D3";
//const char *nwkSKey = "...not used"; // if not OTAA 
// devEUI = 0004A30B001BF88B

#include <SoftwareSerial.h>

//define AnalogPin for sensor
// Configure sound sensor
int sound_din=9;
int sound_ain=A0;
int light_pin=13;
int ad_value;

// Configure transmission cycle
const uint16_t cycle_frequency = 200; // how often do we wish to measure?
const uint16_t cycle_length = 30000; // how often do we transmit?
const uint16_t cycle_over = cycle_length / cycle_frequency;

SoftwareSerial Serial1(4, 5); // RX, TX
#define RST  2
#define loraSerial Serial1
#define debugSerial Serial

#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(57600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  pinMode(13, OUTPUT);
  led_on();

  // Activate sound sensor
  pinMode(sound_din,INPUT);
  pinMode(sound_ain,INPUT);

  //reset RN2xx3
  pinMode(RST, OUTPUT);
  digitalWrite(RST, LOW);
  delay(100);
  digitalWrite(RST, HIGH);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  debugSerial.println("Setup for The Things Network complete");
  ttn.showStatus();

  debugSerial.println("Transmitting Sound level ... ");
}


int cycle = 0;
int loudCount = 0;
float loudAvg = 0;

void loop()
{

  uint16_t ad_value = analogRead(sound_ain);

  if (digitalRead(sound_din) == LOW) {
    loudCount++;
  }
  
  loudAvg = (loudAvg == 0) ? ad_value : (loudAvg + ad_value) / 2;

  cycle++;
  if (cycle >= cycle_over) {
  
    // Split word (16 bits) into 2 bytes of 8
    byte payload[2];
    payload[0] = highByte((uint16_t)loudAvg);
    payload[1] = lowByte((uint16_t)loudAvg);
  
    led_on();
  
    debugSerial.println(loudAvg);
  
    ttn.sendBytes(payload, sizeof(payload));
  
    led_off();

    
    //Serial.println(ad_value);
    //Serial.println(loudAvg);
    //Serial.println(loudCount);
    //Serial.println("---");
    cycle = 0;
    loudCount = 0;
    loudAvg = 0;
  }
  
  delay(cycle_frequency);
}


void led_on()
{
  digitalWrite(light_pin, 1);
}

void led_off()
{
  digitalWrite(light_pin, 0);
}

