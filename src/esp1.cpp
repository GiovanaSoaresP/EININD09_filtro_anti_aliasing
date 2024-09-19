// https://forum.arduino.cc/t/how-to-make-tasks-and-determine-stack-size-in-freertos/978325/13
// https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/02-vTaskDelayUntil
#include <Arduino.h>
#include "IiKit.h"

#define FREQ 5
#define TIME_DELAY 1.0 // Time em milisegundos
#define CILCE_FREQ (FREQ * TIME_DELAY / 1000.0)
#define CILCE_PERIODO (1.0 / CILCE_FREQ)

static uint32_t time_delay_analog_ms = 1;

void plotWave(void *)
{
  uint16_t timeWave = 0;                          // Índice para percorrer a tabela de formas de onda
  AsyncDelay_c delayPlotWave(TIME_DELAY, ISMILI); // time in micro second
  for (;;)
  {
    if (delayPlotWave.isExpired())
    {
      delayPlotWave.repeat();
      const double aux = 1.0 + sin(2.0 * PI * CILCE_FREQ * timeWave);
      ledcWrite(0, uint16_t(511.0 * aux));
      dacWrite(def_pin_DAC1, uint8_t(127.0 * aux));
      if (++timeWave >= CILCE_PERIODO) timeWave = 0;
    }
  }
}

void readTime(void *)
{ 
  for (;;)
  {
    IIKit.WSerial.plot("var1",analogRead(def_pin_ADC1));
    vTaskDelay(time_delay_analog_ms / portTICK_PERIOD_MS);
  }
}

void setup()
{
  IIKit.setup();
  ledcAttachPin(def_pin_PWM, 0);
  ledcSetup(0, 1000, 10);
  xTaskCreate(plotWave, "Task Wave", 4096, NULL, 1, NULL);
  xTaskCreate(readTime, "Read Time", 4096, NULL, 1, NULL);  
}

AsyncDelay_c delayReadtWave(500, ISMILI); // time in micro second
void loop()
{
  IIKit.loop();
  if (delayReadtWave.isExpired())
  {
    delayReadtWave.repeat();
    time_delay_analog_ms = map(analogRead(def_pin_POT1), 0, 4095, 0, 500);
  }
}