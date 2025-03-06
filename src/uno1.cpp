#include <Arduino.h>
#include <math.h>

#define pinANALOG A5                        //Configura o pino das ondas
#define pinPOT A4                        //Configura o pino do potenciometro

volatile uint32_t time_delay_analog_ms = 10;
uint32_t previousTimeAnalogMS = 0;

#define TIME_DELAY_SPEED_MS 100
uint32_t previousTimePrintMS = 0;

void setup() {                              // Codigo de configuração
  Serial.begin(19200);
  pinMode(pinANALOG, INPUT);
  pinMode(pinPOT, INPUT);  
}

void loop()
{
  uint32_t currentMilis = millis();
  if ((currentMilis - previousTimePrintMS) >= TIME_DELAY_SPEED_MS)
  {
    previousTimePrintMS = currentMilis;
    time_delay_analog_ms = map(analogRead(pinPOT),0,1023,10,50);
  }  
  if ((currentMilis - previousTimeAnalogMS) >= time_delay_analog_ms)
  {
    previousTimeAnalogMS = currentMilis;
    Serial.print(">graf:");
    Serial.print(currentMilis);
    Serial.print(":");
    Serial.print(analogRead(pinANALOG));
    Serial.println("|g");
  }
}
