#include "Arduino.h"
#include "clrc663.h"
#include "clrc663-defs.h"
#include "PCF8575.h"
#include "pca.h"

#define _TIMERINTERRUPT_LOGLEVEL_     4

#include "Portenta_H7_TimerInterrupt.h"

#include <stdint.h>
#include <Portenta_Ethernet.h>
#include <Ethernet.h>
#include <math.h>
#include <stdio.h>

#define DEBUG 0
#define RST_PIN 13

#if DEBUG
#define PRINT(str) Serial.print(str);
#define PRINTLN(str) Serial.println(str);
#define PRINTHEX(str,flag) Serial.print(str,flag);
#else
#define PRINT(str) ;
#define PRINTLN(str) ;
#define PRINTHEX(str,flag) ;
#endif
const int LON = LOW;
const int LOFF = HIGH;
const int baud = 115200;

 //Ethernet related ---------------------
byte mac[] = {0xDE, 0x03, 0x33, 0x13, 0x59, 0x99};  //Mac adress
IPAddress ip(192,168,1,81);   //Adresse IP
String StringIP = "192.168.1.81";
EthernetServer server = EthernetServer(80);  // (port 80 is default for HTTP) 52 is the number of the lab

//Timer Related ---------------------------
volatile bool toggle0 = true;
volatile bool toggle1 = true;

#define TIMER0_INTERVAL_MS        60000 //Every minutes
#define TIMER1_INTERVAL_MS        10000

// Init timer TIM15
Portenta_H7_Timer ITimer0(TIM15);
// Init  timer TIM16
Portenta_H7_Timer ITimer1(TIM16);


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
#define MUX_CHANNEL_TOTAL 10
#define MUX1_CHANNEL_COUNT 8
#define MUX2_CHANNEL_COUNT 2

uint8_t muxAddress[] = {MUX1_ADDRESS, MUX2_ADDRESS};
uint8_t muxChannelMax[] = {MUX1_CHANNEL_COUNT,MUX2_CHANNEL_COUNT};
bool readers[40];
bool tagPresent[40];
uint8_t tagContents[40][256];
uint8_t tagContentsStable[40][256];
char readBuff[144];
/* Leds */
#define LED_ON 1
#define LED_OFF 0

Clrc663 clrc(CLRC_DEFAULT_ADDRESS);
PCA95 pcaMux(MUX1_ADDRESS,MUX2_ADDRESS,MUX_CHANNEL_TOTAL,MUX1_CHANNEL_COUNT);


PCF8575 pcf8575(PCF8575_ADDR);
//reset function -- Call it to reset the arduino
void resetFunc(void) {
  unsigned long *registerAddr;
  registerAddr = (unsigned long *)0xE000ED0C; //Writes to the AIRCR register of the stm32h747 to software restet the arduino
  //It is a 32 bit register set bit 2 to request a reset and write 0x05FA to enable the write
  //See Arm® v7-M Architecture Reference Manual for more information
  *registerAddr = (unsigned long) 0x05FA0304;
}

void setup() {
  Serial.begin(baud);
  while(!Serial){
    ;
  }
  Serial.println("NfcReaderCLRC663Portenta.ino");
  Wire.begin();
  //Init all RFID positions - check which positions are occupied or not
  long time_1 = millis();
  scan_readers(readers);
  for(int i = 0;i<5;i++){
    for(int j = 0;j<8;j++){
      Serial.print(readers[j+i*8]);Serial.print("  ");
    }
    Serial.println("");
  }
  Serial.print("all the init took ");Serial.println(millis()-time_1);
  //Init the tags array to be sure they are empty
  for(int i=0;i<40;i++){
    for(int j =0;j<144;j++){
      tagContents[i][j] = 0;
      tagContentsStable[i][j] = 0;
    }
  }
  init_timers();
  /*
  for(int positionKuhner = 1; positionKuhner <= 40; positionKuhner++){
    //Get the channel and position from the position in the Kuhner (1-40)
    int calcChannel = ceil(float(positionKuhner)/8)*2 - (positionKuhner%2?1:0);//Gives the channel index based on the position 1-40
    int calcAddr = (ceil(float((positionKuhner%8))/2) + (positionKuhner%8?0:4))-1;//Gives the offset from 0x28 that should be applied to the reader addr
    
  }
  clrc.pcaselect(muxAddress[0],0);
  */
  //clrc.init(CLRC_DEFAULT_ADDRESS);
  init_pcf8575(PCF8575_ADDR);

  //----------------------ETHERNET INITIALIZATION------------------------------------------------
  Serial.println("Initialize the ethernet connection");
  //Ethernet setup
  Ethernet.begin(mac,ip);  //Start the Ethernet coms
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1500); // do nothing, no point running without Ethernet hardware
      Serial.println("No ethernet shield connected");
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  // Start the server
  server.begin();           //"server" is the name of the object for comunication through ethernet
  Serial.print("Ethernet server connected. Server is at ");
  Serial.println(Ethernet.localIP());         //Gives the local IP through serial com
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
  long time_1 = millis();
  long time_2 = millis();
  long time_3 = millis();
  /*int positionTag = random(1,40);
  Serial.print("Random position is ");Serial.println(positionTag);
  int calcChannel = ceil(float(positionTag)/8)*2 - (positionTag%2?1:0);//Gives the channel index based on the position 1-40
  Serial.print("This position should address mux channel ");Serial.println(calcChannel);
  int calcAddr = (ceil(float((positionTag%8))/2) + (positionTag%8?0:4))-1;//Gives the offset from 0x28 that should be applied to the reader addr
  
  Serial.print("And the offset of the address of the PCB should be ");Serial.println(calcAddr);
  Serial.print("Thus the address ");Serial.println((0x28+calcAddr),HEX);
  Serial.println("-----------------------------");*/
  ethernetClientCheck();
  time_2 = millis();
  ScanPresent(); 
  time_3 = millis();
  if(toggle_0){
    readAllPresent();
  }
  /*
  Serial.print("Ethernet check took ");
  Serial.println(time_2-time_1);
  Serial.print("RFID chip check took "); //Take 6 seconds
  Serial.println(time_3-time_2);
  */
}

void ethernetClientCheck(){

  EthernetClient client = server.available();
  EthernetClient* client_pntr = &client;
  if(client){ //A client tries to connect 
    unsigned long time_start = millis();
    String currentLine = "";
    while(client.connected()) { //Loop while the client is connected
      int state = getStatus();
      if(client.available()){
        time_start = millis(); //reset time_start
        currentLine = ""; //reset currentLine
        //Read the first Line
        char c = client.read();
        while(!(c== '\n' || c == ' ')){
          currentLine += c;
          c = client.read();
        }
        
        if(currentLine.endsWith("home")){
          homePage(client_pntr);
        }
        if(currentLine.endsWith("SSRON")){
          answerHTTP(client_pntr,currentLine);
        }
        if(currentLine.endsWith("SSROFF")){
          answerHTTP(client_pntr,currentLine);
        }
        if(currentLine.endsWith("ReadTag")){
          answerHTTP(client_pntr,currentLine);
        }
        if(currentLine.endsWith("WriteTag")){
          answerHTTP(client_pntr,currentLine);
        }
        if(currentLine.endsWith("ReadAllTags")){
          readAllPresent();
          answerHTTP(client_pntr,currentLine);
        }
        if(currentLine.endsWith("getTags")){
          readAllPresent();
          answerHTTP(client_pntr,currentLine);
        }
        
      }//if(client.available())
      else if(millis()-time_start > 5000){
        Serial.println("Client timed out");
        client.stop();
      }
      
    }//if(client.connected())
    client.stop();
    
  }//if(client)

  
}
//tester mux avec 3.3V et PU à 3.3V
//Tester avec 3.3V et PU 5V
//Tester 5V et PU à 3.3V
//This function scans all channel and all addresses and find where tags are present or not
void ScanPresent()
{

  PRINTLN("Full Scan in position order");
  PRINTLN("****************************************************");
  PRINTLN("* mux\t| ch\t| add\t| ID");
  long time_1 = millis();
  for(int positionKuhner = 1; positionKuhner <=40; positionKuhner++){
    long time_2 = millis();
    //Get the channel and position from the position in the Kuhner (1-40)
    uint8_t calcChannel = ceil(float(positionKuhner)/8)*2 - (positionKuhner%2?1:0)-1;//Gives the channel index based on the position 1-40
    uint8_t calcAddr = (ceil(float((positionKuhner%8))/2) + (positionKuhner%8?0:4))-1;//Gives the offset from 0x28 that should be applied to the reader addresses
    
    if(readers[positionKuhner-1]){
      pcaMux.channelSelect(calcChannel);
      
      if(clrc.init(CLRC663_BASE_ADDRESS + calcAddr)) {
      //Init Transmitter
      clrc.iso14443a_init();
      clrc.transmit_enable(true);
      
      // send request of iso14443 version A device
      if(clrc.iso14443a_REQA() == 2){
        uint8_t data[16] = {0};
        uint8_t uid[10] = {0};
        uint8_t sak;
        uint8_t len = clrc.iso14443a_select(uid, &sak);
        if(len<1){
          PRINTLN("Error selecting PICC");
        }     
        Serial.print("\nUID : "); clrc.print_buffer_hex((uint8_t*) uid, len);
        tagPresent[positionKuhner-1] = true;
      }
      else{
        PRINTLN("No tag found");
        tagPresent[positionKuhner-1] = false;
      }
      
      clrc.transmit_enable(false);
      }//if(clrc.init) check that readers is responding
      else{
        PRINT("Reader didn't respond at position");PRINTLN(positionKuhner);
      }
      
      pcaMux.closeAll();
    } //if(readers[positionKuhner])
    else{
       PRINT("Reader missing at position");PRINTLN(positionKuhner);
    }
    #if DEBUG
    Serial.print("Position ");Serial.println(positionKuhner);
    Serial.print("Channel ");Serial.println(calcChannel);
    Serial.print("Addr ");Serial.println(CLRC663_BASE_ADDRESS + calcAddr, HEX);
    Serial.println("-----------------------------");
    #endif

  } //for positionKuhner 1->40
  PRINTLN("\r\n----------------------------------------------------");   
  PRINTLN("Done.");
}

void readAllPresent(){
  //This function reads all baskets present in the incubator and store the read data in an array
  PRINTLN("Reading all tags present in the incubator");
  PRINTLN("****************************************************");
  PRINTLN("* mux\t| ch\t| add\t| ID");
  long time_1 = millis();
  for(int positionKuhner = 1; positionKuhner <=40; positionKuhner++){
    long time_2 = millis();
    //Get the channel and position from the position in the Kuhner (1-40)
    uint8_t calcChannel = ceil(float(positionKuhner)/8)*2 - (positionKuhner%2?1:0)-1;//Gives the channel index based on the position 1-40
    uint8_t calcAddr = (ceil(float((positionKuhner%8))/2) + (positionKuhner%8?0:4))-1;//Gives the offset from 0x28 that should be applied to the reader addresses
    
    if(tagPresent[positionKuhner-1]){
      pcaMux.channelSelect(calcChannel);
      
      if(clrc.init(CLRC663_BASE_ADDRESS + calcAddr)) {
      //Init Transmitter
      clrc.iso14443a_init();
      clrc.transmit_enable(true);
      
      // send request of iso14443 version A device
      if(clrc.iso14443a_REQA() == 2){
        tagPresent[positionKuhner-1] = true;
        uint8_t data[16] = {0};
        uint8_t uid[10] = {0};
        uint8_t sak;
        uint8_t len = clrc.iso14443a_select(uid, &sak);
        if(len<1){
          PRINTLN("Error selecting PICC");
        }     
        //Serial.print("\nUID : "); clrc.print_buffer_hex((uint8_t*) uid, len);
        
        //Read 100% of user memory
        //Read user memory and place the bytes in an array
        //We have 144 byte of memory
        uint8_t FFkey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        clrc.cmd_load_key(FFkey); // load into the key buffer
        uint8_t readBuf[16];
        len = 0;
        //A block is 4 byte -> we need to read every 4 blocks not to create copy
        for(uint8_t blockAddr = 0; blockAddr < 9; blockAddr++){
        len += clrc.MF_read_block((blockAddr*4)+4, readBuf);
        if(len == 0){
          PRINT("Couldn't read block ");PRINTLN(blockAddr);
        }
        for(uint8_t byteAddr = 0; byteAddr < 16; byteAddr++){
          tagContents[positionKuhner-1][blockAddr*16 + byteAddr] = readBuf[byteAddr]; //mux*32 + muxChannel*4+address
        }
        delay(5);
        }
        //If the read is correct then we write it in the Stable tagContents
        if(len == 144){
          PRINTLN("Managed to read the 144 bytes of User memory");
          for(int i = 0;i<144;i++){
            tagContentsStable[positionKuhner-1][i] = tagContents[positionKuhner-1][i]
            PRINTLN(" 0x");PRINTHEX(tagContents[positionKuhner-1][i],HEX);
            if(!((i+1)%16)) PRINTLN("");
          }
        }
        /*
        //Write is done 4 bytes by 4 bytes
        uint8_t writeBuf[144] = {0};
        for(int i=0;i<144;i++){
          writeBuf[i] = tagContents[positionKuhner-1][i] + 1;
        }
        for(uint8_t pageAdr = 0; pageAdr < 36; pageAdr++){
          len = clrc.MF_write_block(pageAdr+4, writeBuf+pageAdr*4);
          if(len != 16){
            PRINTLN("Couldn't write block " + String(pageAdr));
          }
          delay(5);
        }
        */
      }
      else{
        PRINTLN("No tag found");
        tagPresent[positionKuhner-1] = false;
      }
      
      clrc.transmit_enable(false);
      }//if(clrc.init) check that readers is responding
      else{
        PRINT("Reader didn't respond at position");PRINTLN(positionKuhner);
      }
      
      pcaMux.closeAll();
    } //if(readers[positionKuhner])
    else{
       PRINT("Reader missing at position");PRINTLN(positionKuhner);
    }
    #if DEBUG
    Serial.print("Position ");Serial.println(positionKuhner);
    Serial.print("Channel ");Serial.println(calcChannel);
    Serial.print("Addr ");Serial.println(CLRC663_BASE_ADDRESS + calcAddr, HEX);
    Serial.println("-----------------------------");
    #endif

  } //for positionKuhner 1->40
  PRINTLN("\r\n----------------------------------------------------");   
  PRINTLN("Done.");
}
void readBasket(uint8_t positionKuhner){
  
}
void writeBasket(uint8_t positionKuhner){
  
}
