#ifndef __WSERIAL_H
#define __WSERIAL_H
#include <Arduino.h>
#include <ESPTelnet.h>

#define BAUD_RATE 115200
#define NEWLINE "\r\n"
ESPTelnet _telnet;

class WSerial_c
{
  typedef void (*CallbackFunction)(String str);

protected:
  uint16_t server_port = 0;
  bool isClientConnected;
  CallbackFunction on_input;
  void start(uint16_t port, unsigned long baudrate = BAUD_RATE);
  void update();

public:
  uint16_t serverPort() { return (server_port); }
  friend inline void startWSerial(WSerial_c *ws, uint16_t port, unsigned long baudrate = BAUD_RATE);
  friend inline void updateWSerial(WSerial_c *ws);

  template <typename T>
  void print(const T &data);
  template <typename T>
  void println(const T &data);
  void println();

  template <typename T>
  void plot(const char *varName, TickType_t x, T y, const char *unit = NULL);
  template <typename T>
  void plot(const char *varName, T y, const char *unit = NULL);

  void onInputReceived(CallbackFunction callback);
};

inline void startWSerial(WSerial_c *ws, uint16_t port, unsigned long baudrate) { ws->start(port, baudrate); }
void WSerial_c::start(uint16_t port, unsigned long baudrate)
{
  if (isClientConnected)
  {
    _telnet.stop();
  }
  isClientConnected = false;
  server_port = port;
  Serial.begin(baudrate);
  Serial.println();
  // passing on functions for various telnet events
  _telnet.onConnect([](String ip)
                    {
    Serial.print("- Telnet: ");
    Serial.print(ip);
    Serial.println(" connected");

    _telnet.println("\nWelcome " + _telnet.getIP());
    _telnet.println("(Use ^] + q  to disconnect.)"); });
  _telnet.onConnectionAttempt([](String ip)
                              {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" tried to connected"); });
  _telnet.onReconnect([](String ip)
                      {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" reconnected"); });
  _telnet.onDisconnect([](String ip)
                       {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" disconnected"); });

  Serial.print("- Telnet: ");
  if (_telnet.begin(port))
  {
    Serial.println("running");
  }
  else
  {
    Serial.println("error.");
  }
}

inline void updateWSerial(WSerial_c *ws) { ws->update(); }
void WSerial_c::update()
{
  _telnet.loop();
  if (!isClientConnected)
  {
    if (Serial.available())
    {
      on_input((Serial.readStringUntil('\n')).c_str());
    }
  }
}

template <typename T>
void WSerial_c::plot(const char *varName, T y, const char *unit)
{
  plot(varName, (TickType_t)xTaskGetTickCount(), y, unit);
}
template <typename T>
void WSerial_c::plot(const char *varName, TickType_t x, T y, const char *unit)
{
  String str(">");
  str.concat(varName);
  str.concat(":");
  str.concat(x);
  str.concat(":");
  str.concat(y);
  str.concat(unit != NULL ? "§" : "");
  str.concat(unit != NULL ? unit : "");
  str.concat("|g");
  println(str);
}

template <typename T>
void WSerial_c::print(const T &data)
{
  if (isClientConnected)
    _telnet.println(data);
  else
    Serial.println(data);
}

template <typename T>
void WSerial_c::println(const T &data)
{
  if (isClientConnected)
    _telnet.println(data);
  else
    Serial.println(data);
}
void WSerial_c::println()
{
  if (isClientConnected)
    _telnet.println();
  else
    Serial.println();
}

void WSerial_c::onInputReceived(CallbackFunction callback)
{
  _telnet.onInputReceived(callback);
  on_input = callback;
}

#endif