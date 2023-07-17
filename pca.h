#ifndef _PCA_H
#define _PCA_H

#include "api/Print.h"
#include <Arduino.h>
#include <Wire.h>
#include "clrc663.h"

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


class PCA95 {
  private :

  public :
    uint8_t _mux1 = 0x70;
    uint8_t _mux2 = 0x77;
    uint8_t _channelTotal = 10;
    uint8_t _channelCount1 = 8;
    PCA95(uint8_t mux1, uint8_t mux2, uint8_t channelTotal, uint8_t channelCount1);
    bool channelSelect(uint8_t channel);
    bool channelClose(uint8_t channel);
    bool closeAll();
};

#endif
