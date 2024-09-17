#include <Arduino.h>
#include <math.h>

#define pinANALOG A5                        //Configura o pino de leitura do LUXÍMETRO
#define WINSIZE 200
#define A 1
#define B 0

typedef struct {
  double rmsValue;
  double *history;
  uint16_t last_index;
  uint16_t rmsOrder;
} RmsStruct;

void RMS_init(RmsStruct* f,const uint16_t rmsOrder) {
  f->history = (double *) malloc(rmsOrder*sizeof(double));
  for(uint16_t i = 0; i < rmsOrder; ++i) f->history[i] = 0;  
  f->rmsOrder = rmsOrder;
  f->last_index = 0;
  f->rmsValue = 0;
}
  
void RMS_put(RmsStruct* f, double input) {
  f->history[f->last_index++] = input;
  if(f->last_index == f->rmsOrder) f->last_index = 0;
}
  
double RMS_get(RmsStruct* f) {
  double rms = 0;
  double samples_avg = 0;
  
  //average calc
  for(uint16_t index = f->last_index, i=0; i < f->rmsOrder; ++i) {
    index = index != 0 ? index-1 : f->rmsOrder-1;
    samples_avg += f->history[index];
  }
  samples_avg = samples_avg / f->rmsOrder;

  //Offset average and power 2
  double val;
  for(uint16_t index = f->last_index, i=0; i < f->rmsOrder; ++i) {
    index = index != 0 ? index-1 : f->rmsOrder-1;
    val = f->history[index] - samples_avg;
    rms += val*val;
  }

  //Average and sqrt
  rms = sqrt(rms / f->rmsOrder);
  f->rmsValue = abs(f->rmsValue - rms) < 0.3 ? f->rmsValue: rms;
  return f->rmsValue;
}

RmsStruct rms;

void printSerial(const uint32_t &currentMilis) {
  Serial.print(">graf:");
  Serial.print(currentMilis);
  Serial.print(":");
  Serial.print(A*RMS_get(&rms)+B);
  Serial.println("|g");
}

void analogReadFunc() {                      // Faz a leitura do sinal Analógico
  RMS_put(&rms,abs(analogRead(pinANALOG)-512));
}

void setup() {                              // Codigo de configuração
  Serial.begin(19200);
  pinMode(pinANALOG, INPUT);
  RMS_init(&rms,WINSIZE);  
}

#define TIME_DELAY_ANALOG_MS 1
uint32_t previousTimeAnalogMS = 0;

#define TIME_DELAY_PRINT_MS 100
uint32_t previousTimePrintMS = 0;

void loop()
{
  uint32_t currentMilis = millis();
  if ((currentMilis - previousTimeAnalogMS) >= TIME_DELAY_ANALOG_MS)
  {
    previousTimeAnalogMS = currentMilis;
    analogReadFunc();
  }
  if ((currentMilis - previousTimePrintMS) >= TIME_DELAY_PRINT_MS)
  {
    previousTimePrintMS = currentMilis;
    printSerial(currentMilis);
  }  
}
