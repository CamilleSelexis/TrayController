#include "api/Print.h"
#include <Arduino.h>
#include <Wire.h>
#include "pca.h"

#define DEBUG_CLR 0

#if DEBUG_CLR
#define PRINTLN(str);        Serial.println(str);
#define PRINT(str);          Serial.print(str);
#define PRINTHEX(str,flag);  Serial.print(str,flag);
#define PRINT_BLOCK(a,b);    print_block(a,b);
#else
#define PRINTLN(str);
#define PRINT(str); 
#define PRINTHEX(str,flag);
#define PRINT_BLOCK(a,b); 
#endif

PCA95::PCA95(uint8_t mux1, uint8_t mux2, uint8_t channelTotal, uint8_t channelCount1)
{
  _mux1 = mux1;
  _mux2 = mux2;
  _channelTotal = channelTotal;
  _channelCount1 = channelCount1;

}

bool PCA95::channelSelect(uint8_t channel){
  if(channel > _channelTotal-1) return false;
  if(channel < _channelCount1-1){ //First MUX
    Wire.beginTransmission(_mux1);
    Wire.write(1<< (channel));
    Wire.endTransmission();
  }
  else{
    Wire.beginTransmission(_mux2);
    Wire.write(1<< (channel-8));
    Wire.endTransmission();
  }
  return true;
}

bool PCA95::channelClose(uint8_t channel)
{
  PRINTLN("PCA select ");
  PRINT("device address: ");
  PRINTHEX(_addr,HEX);
  if (channel > _channelTotal-1) return false;
    if(channel < _channelCount1-1){ //First MUX
    Wire.beginTransmission(_mux1);
    Wire.write(channel>>channel);
    Wire.endTransmission();
  }
  else{
    Wire.beginTransmission(_mux2);
    Wire.write((channel-8)>>(channel-8));
    Wire.endTransmission();
  }
  PRINTLN("port close ");
  return true;
}

bool PCA95::closeAll()
{
  //Same addresses on all channel so we want to close all channel when we call this function
  Wire.beginTransmission(_mux1);
  Wire.write(0);
  Wire.endTransmission();
  Wire.beginTransmission(_mux2);
  Wire.write(0);
  Wire.endTransmission();

  return true; 
}

//Old functions
/*
void PCA95::channelSelect(uint8_t channel)
{
  
  if (channel > _channelCount) return;
  Wire.beginTransmission(_addr);
  Wire.write(1 << channel);
  Wire.endTransmission(); 

}

void PCA95::channelClose(uint8_t channel)
{
  PRINTLN("PCA select ");
  PRINT("device address: ");
  PRINTHEX(_addr,HEX);
  if (channel > _channelCount) return;
  Wire.beginTransmission(_addr);
  Wire.write(channel >> channel);
  Wire.endTransmission(); 
  PRINTLN("port close "); 
}
*/
