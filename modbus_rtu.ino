#include <ModbusRtu.h>
#include "DHT.h"

#define DHTPIN 2

#define DHTTYPE DHT11

Modbus slave(4,Serial,0);
DHT dht(DHTPIN,DHTTYPE);

void setup()
{
  Serial.begin(9600, SERIAL_8N1);
  slave.start();
  dht.begin();
  digitalWrite(3, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(9, HIGH);
  delay(5000);
  digitalWrite(3, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(9, LOW);
    
}

void loop()
{
  static uint16_t temp,hum,v_w, v_v, f_w, w_q, data[500];
  hum=dht.readHumidity();
  temp=dht.readTemperature();
  data[0]=temp;
  data[1]=hum;
  slave.poll(data,500);
  v_w = data[365];
  if (v_w == 1)
  {
    digitalWrite(3, HIGH);
  }
  else
    digitalWrite(3, LOW);
  v_v = data[374];
  if (v_v == 1)
  {
    digitalWrite(5, HIGH);
  }
  else
    digitalWrite(5, LOW);
  f_w = data[383];
  if (f_w == 1)
  {
    digitalWrite(6, HIGH);
  }
  else
    digitalWrite(6, LOW);
  w_q = data[395];
  if (w_q == 1)
  {
    digitalWrite(9, HIGH);
  }
  else
    digitalWrite(9, LOW);
 
}
