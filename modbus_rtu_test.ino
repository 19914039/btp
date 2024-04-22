#include <ModbusRtu.h>
#include "DHT.h"

#define DHTPIN 2

#define DHTTYPE DHT11

Modbus slave(1,Serial,0);
DHT dht(DHTPIN,DHTTYPE);

void setup()
{
  Serial.begin(19200, SERIAL_8E1);
  slave.start();
  dht.begin();  
}

void loop()
{
  static uint16_t temp,hum,data[2];
  hum=dht.readHumidity();
  temp=dht.readTemperature();
  data[0]=temp;
  data[1]=hum;
  slave.poll(data,2);
 
}
