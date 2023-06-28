#include "Arduino.h"
#include "clrc663.h"
#include "clrc663-defs.h"
#include "PCF8575.h"

//Pins Portenta
// SDA  D11
// SCL  D12
#define NANO 1
#define PORTENTA 2
#define SCAN_MUX_ENABLED 1

/* i2c addresses */
#define CLRC_DEFAULT_ADDRESS 0x2B
#define CLRC663_BASE_ADDRESS 0x28
#define CLRC663_MUX_MAX_DEVICES 4
#define PCF8575_ADDR         0x20

/* scan defs */
#define SCAN_DELAY 10 // in millis
#define PORT_MUX_MAX 10

/* scan  infos */


unsigned long scan_millis; /* time between scans */
unsigned long last_scan;   /* last time scanned */

#define MUX_COUNT 2
#define MUX1_ADDRESS 0x70
#define MUX2_ADDRESS 0x77
#define MUX1_CHANNEL_COUNT 8
#define MUX2_CHANNEL_COUNT 2

uint8_t muxAdrress[] = {MUX1_ADDRESS, MUX2_ADDRESS};
uint8_t muxChannelMax[] = {MUX1_CHANNEL_COUNT,MUX2_CHANNEL_COUNT};
uint8_t tagContents[40][256];
/* Leds */
#define LED_ON 1
#define LED_OFF 0

Clrc663 clrc(CLRC_DEFAULT_ADDRESS);

PCF8575 pcf8575(PCF8575_ADDR);

void setup() {
  Serial.begin(115200);
  while(!Serial){
    ;
  }
  Serial.println("NfcReaderCLRC663Portenta.ino");
  Wire.begin();
  Wire.setTimeout(5);
  //Wire.setClock(200000);
int adr[128];
int a = scan_i2c(adr);
#if SCAN_MUX_ENABLED
  clrc.pcaselect(0x70,0);
#endif
  clrc.pcaselect(0x70,0);

  clrc.init(CLRC_DEFAULT_ADDRESS);
  init_pcf8575(PCF8575_ADDR);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*Serial.println("Enable SSR");
  enable_SSR(0);
  enable_SSR(1);
  enable_SSR(2);
  enable_SSR(3);
  delay(5000);
  Serial.println("Disable SSr");
  
  disable_SSR(0);
  disable_SSR(1);
    disable_SSR(2);
  disable_SSR(3);
  delay(5000);*/
  doScan();
  delay(500);
}

//tester mux avec 3.3V et PU à 3.3V
//Tester avec 3.3V et PU 5V
//Tester 5V et PU à 3.3V
void doScan()
{
#if SCAN_MUX_ENABLED
  Serial.println("Scanning ....");
  Serial.println("****************************************************");
  Serial.println("* mux\t| ch\t| add\t| ID");
  for(int mux = 0; mux< MUX_COUNT; mux++){
    for(int muxChannel = 0; muxChannel< muxChannelMax[mux]; muxChannel++){
      // select mux and channel to scan 
      clrc.pcaselect(muxAdrress[mux], muxChannel);
      for(int address = 0; address < CLRC663_MUX_MAX_DEVICES; address++ ){ //Scan all address on this mux channel
        //Serial.print("MUX : ");Serial.print(mux);Serial.print(" // Channel : ");Serial.print(muxChannel);Serial.print(" // ADDR : ");Serial.println(address);
#else
        int mux = 0;
        int muxChannel = 0;
        int address = 0;
#endif
        //clrc.pcaselect(muxAdrress[0], 0);
        // Initialize reader with its address, only continue if the reader respond to its version
        if(clrc.init(CLRC663_BASE_ADDRESS + address)){
          clrc.iso14443a_init();
          // enable transmitter
          clrc.transmit_enable(true);
          //Serial.println(CLRC663_BASE_ADDRESS + address,HEX);
          // send request of iso14443 version A device
          if(clrc.iso14443a_REQA() != 2){
            Serial.println("No tag found");
          }else{
            uint8_t data[16] = {0};
            uint8_t uid[10] = {0};
            uint8_t sak;
            uint8_t len = clrc.iso14443a_select(uid, &sak);
             if(len<1){
              Serial.println("Error selecting PICC");
             }
            //--------------------------------------
            Serial.print("*  ");Serial.print(mux); 
            Serial.print("\t| "); Serial.print(muxChannel);
            Serial.print("\t| "); Serial.print(CLRC663_BASE_ADDRESS + address,HEX);
            Serial.print("\t| "); clrc.print_buffer_hex((uint8_t*) uid, len);   
            Serial.print("\n");       
  
            //Read user memory and place the bytes in an array
            //We have 144 byte of memory
            uint8_t FFkey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            clrc.cmd_load_key(FFkey); // load into the key buffer
            uint8_t readBuf[16];
            len = 0;
            //A block is 4 byte -> we need to read every 4 blocks not to create copy
            for(uint8_t blockAdr = 0; blockAdr < 9; blockAdr++){
              len += clrc.MF_read_block((blockAdr*4)+4, readBuf);
              if(len == 0){
                Serial.println("Couldn't read block " + String(blockAdr));
              }
              for(uint8_t byteAdr = 0; byteAdr < 16; byteAdr++){
                tagContents[mux*32 + muxChannel*4+address][blockAdr*16 + byteAdr] = readBuf[byteAdr];
              }
              delay(5);
            }
            if(len == 144){
              Serial.println("Managed to read the 144 bytes of User memory");
              for(int i = 0;i<144;i++){
                Serial.print(" 0x");Serial.print(tagContents[mux*32 + muxChannel*4+address][i],HEX);
                if(!((i+1)%16)) Serial.println("");
              }
            }
            //Write is done 4 bytes by 4 bytes
            uint8_t writeBuf[144] = {0};
            for(int i=0;i<144;i++){
              writeBuf[i] = tagContents[mux*32 + muxChannel*4+address][i] + 1;
            }
            for(uint8_t pageAdr = 0; pageAdr < 36; pageAdr++){
              len = clrc.MF_write_block(pageAdr+4, writeBuf+pageAdr*4);
              if(len != 16){
                Serial.println("Couldn't write block " + String(pageAdr));
              }
              delay(5);
            }
          }
          clrc.transmit_enable(false);
        }
        else{
          //Serial.println("No reader here");
        }
#if SCAN_MUX_ENABLED
      }
      clrc.pcaclose(muxAdrress[mux], muxChannel);
    }
  }
#endif
Serial.println("\r\n----------------------------------------------------");   
Serial.println("Done.");
}
