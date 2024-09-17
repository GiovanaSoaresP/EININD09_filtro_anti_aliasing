#include <Arduino.h>
#include "IiKit.h"

#define FREQ 50.0
#define TIME_DELAY 100.0 // Time em microsegundos
#define CILCE_FREQ (FREQ * TIME_DELAY / 1000000.0)
#define CILCE_PERIODO (1.0 / CILCE_FREQ)

void plotWave(void *)
{
  uint16_t timeWave = 0;                          // Índice para percorrer a tabela de formas de onda
  AsyncDelay_c delayPlotWave(TIME_DELAY, ISMICROS); // time in micro second
  for (;;)
  {
    if (delayPlotWave.isExpired())
    {
      delayPlotWave.repeat();
      const double aux = 1.0 + sin(2.0 * PI * CILCE_FREQ * timeWave);
      ledcWrite(0, uint16_t(511.0*aux));
      dacWrite(def_pin_DAC1, uint8_t(127.0*aux));
      if (++timeWave >= CILCE_PERIODO) timeWave = 0;
    }
  }
}

static uint32_t time_delay_analog_ms = 1;
void readWave(void *)
{
  uint32_t previousTimeAnalogMS = 0;  
  for (;;)
  {
    uint32_t currentMilis = millis();
    if ((currentMilis - previousTimeAnalogMS) >= time_delay_analog_ms)
    {
      previousTimeAnalogMS = currentMilis;
      IIKit.WSerial.plot("var1",currentMilis,analogRead(def_pin_DAC1));
    }
  }
}

void readTime(void *)
{ 
  for (;;)
  {
    time_delay_analog_ms = map(analogRead(def_pin_POT1), 0, 4095, 0, 20);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  IIKit.setup();
  ledcAttachPin(def_pin_PWM, 0);
  ledcSetup(0, 1000, 10);
  xTaskCreate(plotWave, "Task Wave", 5000, NULL, 1, NULL);
  xTaskCreate(readWave, "Task Wave", 5000, NULL, 1, NULL);  
  xTaskCreate(readTime, "Task Wave", 5000, NULL, 1, NULL); 
}

void loop()
{
  IIKit.loop();
}