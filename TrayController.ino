/*
 * This programm is designed to run on an arduino portenta H7 Lite
 * This programm should run on the M7 Core
 * 

 */
#include "Arduino.h"
#include "clrc663.h"
#include "clrc663-defs.h"
#include "PCF8575.h"
#include "pca.h"
#include <Adafruit_MAX31865.h>
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
// Temp control related
#define PIN_HEATINGENABLE      5      //Input for heating enable
#define PIN_SSRCONTROL         6       //Output for SSR COntrol
#define PIN_ANALOGTEMP         4     //Output to give the temp as an analog measure
// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(7, 8, 10, 9);
float tempDoor;
float lastTemp;
long lastMeasureTime = 0;
long error_count = 0;
 //Ethernet related ---------------------
byte mac[] = {0xDE, 0x03, 0x33, 0x13, 0x59, 0x99};  //Mac adress
IPAddress ip(192,168,1,82);   //Adresse IP
String StringIP = "192.168.1.82";
EthernetServer server = EthernetServer(80);  // (port 80 is default for HTTP) 52 is the number of the lab

//Pins Portenta
// SDA  D11
// SCL  D12
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
#define SCAN_TIMEOUT  60000
#define SSR_TIMEOUT   10000

unsigned long scan_millis; /* time between scans */
unsigned long last_scan = 0;   /* last time scanned */

#define MUX_COUNT 2
#define MUX1_ADDRESS 0x70
#define MUX2_ADDRESS 0x77
#define MUX_CHANNEL_TOTAL 10
#define MUX1_CHANNEL_COUNT 8
#define MUX2_CHANNEL_COUNT 2

uint8_t muxAddress[] = {MUX1_ADDRESS, MUX2_ADDRESS};
uint8_t muxChannelMax[] = {MUX1_CHANNEL_COUNT,MUX2_CHANNEL_COUNT};
bool readers[40];
bool tagPresent[40]; //Tags that are detected -> does not mean that we can read the memory
bool tagReadable[40]; //Tags that can be read -> should be the same as tag present
bool tagUpdate[40]; //Tags that were updated
bool tagToTransmit[40]; //Tags data to transmit the next time client asks for new info
uint8_t tagContents[40][256];
uint8_t uidList[40][10];
uint8_t tagContentsStable[40][256];
uint8_t readTag[144];
uint8_t writeTag[144];
long count0 = 1;
/* Leds */
#define LED_ON 1
#define LED_OFF 0

//RFID Reader/Writer
Clrc663 clrc(CLRC_DEFAULT_ADDRESS);
//I2C Multiplexer for RFID Tags
PCA95 pcaMux(MUX1_ADDRESS,MUX2_ADDRESS,MUX_CHANNEL_TOTAL,MUX1_CHANNEL_COUNT);

//SSR relay control
PCF8575 pcf8575(PCF8575_ADDR);
bool bSSRON = false;
long start_SSR =0;
//Led indications
bool bLedAlive = false;
//reset function -- Call it to reset the arduino
void resetFunc(void) {
  unsigned long *registerAddr;
  registerAddr = (unsigned long *)0xE000ED0C; //Writes to the AIRCR register of the stm32h747 to software restet the arduino
  //It is a 32 bit register set bit 2 to request a reset and write 0x05FA to enable the write
  //See Arm® v7-M Architecture Reference Manual for more information
  *registerAddr = (unsigned long) 0x05FA0304;
}

//EIP global variables
int kuhner_temp_current = 0;
int kuhner_humidity_current = 0;
int kuhner_CO2_current = 0;
int kuhner_shaking_current = 0;
int kuhner_temp_setpoint = 0;
int kuhner_humidity_setpoint = 0;
int kuhner_CO2_setpoint = 0;
int kuhner_shaking_setpoint = 0;
unsigned long last_EIP_contact =0;

void setup() {
  Serial.begin(baud);
  /*
  while(!Serial){
    ;
  }
  */
  Serial.println("NfcReaderCLRC663Portenta.ino");
  Wire.begin();
  //Init all RFID positions - check which positions are occupied or not
  long time_1 = millis();
  scan_readers(readers);
  int sumReaders = 0;
  for(int i = 0;i<5;i++){
    for(int j = 0;j<8;j++){
      Serial.print(readers[j+i*8]);Serial.print("  ");
      sumReaders += readers[j+i*8];
    }
    Serial.println("");
  }
  if(sumReaders<40){
    Serial.println("Error reader missing");
  }
  Serial.print("all the init took ");Serial.println(millis()-time_1);
  //Init the tags array to be sure they are empty
  for(int i=0;i<40;i++){
    tagPresent[i] = 0;
    tagReadable[i] = 0;
    for(int j =0;j<144;j++){
      tagContents[i][j] = 0;
      tagContentsStable[i][j] = 0;
    }
    for(int j = 0;j<10;j++){
      uidList[i][j] = 0;
    }
    tagToTransmit[i] = false;
  }
  
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
  
  //Init temperature control
  thermo.begin(MAX31865_4WIRE);
  pinMode(PIN_SSRCONTROL,OUTPUT);
  pinMode(PIN_ANALOGTEMP,OUTPUT);
  pinMode(PIN_HEATINGENABLE,INPUT);
  readAllPresent();
}

void loop() {
  //Check if a client is making a request as often as possible //every 2s
  bLedAlive = !bLedAlive;
  digitalWrite(LEDB, bLedAlive);
  ethernetClientCheck();
  //long time_1 = millis();
  //Update tag list as often as possible
  ScanPresent(); //Scan to see where are occupied positions, if the list of occupied positions change, read the new tags and put their data in the array
  //Serial.println("Scan all tags took : " + String(millis()-time_1));
  //read all positions every minutes, to correct potential errors might be unnecessary
  if(millis()-last_scan > SCAN_TIMEOUT){
    last_scan = millis();
    digitalWrite(LEDG,LOW);
    readAllPresent();
    digitalWrite(LEDG,HIGH);
  }
  //Disable all ssr after one was activated
  if(millis()-start_SSR > SSR_TIMEOUT && bSSRON){
    Serial.println("SSR turning off");
    bSSRON = false;
    start_SSR = 0;
    for(int i=0;i<8;i++){
      disable_SSR(i);
    }
  }
  if(millis()-lastMeasureTime>1000 && digitalRead(PIN_HEATINGENABLE)){
    SlidingDoorTempControl();
  }
  //Connect to the Net-Z and read all current values
  //Put the results in global variable
  
  if(millis()-last_EIP_contact>15000){
    get_attribute_single_allPV();
    last_EIP_contact = millis();
  }
  
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
        //Read the first Line it should be GET /url HTTP/1.1\r\n
        char c = client.read();
        while(!(c== '\n' || c==' ')){
          currentLine += c;
          c = client.read();
        }
        Serial.println(currentLine);
        //if((currentLine.indexOf("GET")>=0)){ //Only proceed if the command is a get
        
        if(currentLine.indexOf("home")>=0){
          homePage(client_pntr);
          Serial.println("Received command to reload home page");
        }
        else if(currentLine.indexOf("SSRON")>=0){
          //answerHTTP(client_pntr,currentLine);
          SSRONCommand(client_pntr,currentLine);
        }
        else if(currentLine.indexOf("SSROFF")>=0){
          SSROFFCommand(client_pntr,currentLine);
        }
        else if(currentLine.indexOf("showTagList")>=0){
          readAllPresent();
          showTagList(client_pntr);
          Serial.println("Received command to show tag list");
        } 
        else if(currentLine.indexOf("getTagList")>=0){
          //readAllPresent();
          getTagList(client_pntr);
          readAllPresent();
        }
        //ajax Request for auto update of home page
        else if(currentLine.indexOf("updateStartPage")>=0){
          updateStartPage(client_pntr);
          //Serial.println("Command to update the home page");
        }
        //ajax Request for auto update of tag list
        else if(currentLine.indexOf("updateTagList")>=0){
          updateTagList(client_pntr);
          //Serial.println("Command to update tag list");
        }
        else if(currentLine.indexOf("readPosition")>=0){
          readPosition(client_pntr,currentLine);
          //answerHTTP(client_pntr,currentLine);
        }
        else if(currentLine.indexOf("writePosition")>=0){
          writePosition(client_pntr,currentLine);
          //answerHTTP(client_pntr,currentLine);
        }
        else if(currentLine.indexOf("getTagUpdate")>=0){
          Serial.println("Received command getTagUpdate");
          getTagUpdate(client_pntr);
          Serial.println("Sent new tag data for positions : ");
          for(int i=0;i<40;i++){
            if(tagToTransmit[i]){
              Serial.println(i+1);
            }
            tagToTransmit[i] = 0;
          }
        }
        else if(currentLine.indexOf("getPV")>=0){
          Serial.println("Received command getPV");
          getKuhner_PV(client_pntr);
        }
        else if(currentLine.indexOf("resetController")>=0){
          Serial.println("Received reset command");
          answerHTTP(client_pntr,currentLine);
          resetFunc();
        }
        //}//if(!currentLine.indexOf("GET")>0)
      }//if(client.available())
      else if(millis()-time_start > 5000){
        Serial.println("Client timed out");
        answerHTTPNo(client_pntr,currentLine,0);
        client.stop();
      }
    }//if(client.connected())
    client.stop();
  }//if(client) 
}

//This function scans all channel and all addresses and find where tags are present or not
void ScanPresent()
{
  bool tagPresentOld[40]; //Temporary array to compare if  a tag was added
  //bool tagPresentNew[40]; //This array is used to tell a client if new tags were added since the last scan
  PRINTLN("Full Scan in position order");
  PRINTLN("****************************************************");
  for(int positionKuhner = 1; positionKuhner <=40; positionKuhner++){
    //Get the channel and position from the position in the Kuhner (1-40)
    uint8_t calcChannel = ceil(float(positionKuhner)/8)*2 - (positionKuhner%2?1:0)-1;//Gives the channel index based on the position 1-40
    uint8_t calcAddr = (ceil(float((positionKuhner%8))/2) + (positionKuhner%8?0:4))-1;//Gives the offset from 0x28 that should be applied to the reader addresses
    uint8_t posKuhner = positionKuhner-1;
    tagPresentOld[posKuhner] = tagPresent[posKuhner];
    //tagUpdate[posKuhner] = false;
    if(readers[posKuhner]){
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
        //uint8_t len = clrc.iso14443a_select(uidList[posKuhner], &sak);
        if(len<5){
          PRINTLN("Error selecting PICC");
        }     
        //Serial.print("\nUID : "); clrc.print_buffer_hex((uint8_t*) uidList[positionKuhner-1], len);
        tagPresent[posKuhner] = true;
        if(tagPresentOld[posKuhner])//Si un tag était déjà présent
        {
          //If the UID changed
          bool uidChanged = false;
          for(int k=0;k<10;k++){ //Check if the uid changed and change it if yes
            if(uid[k]!=uidList[posKuhner][k]){
              uidChanged = true;
              uidList[posKuhner][k] = uid[k];
            }
          }
          if(uidChanged)
          { //Tag was updated
            tagUpdate[posKuhner] = true;
          }
          else{ //Same tag than last time
            tagUpdate[posKuhner] = false;
          }
        }
        else{ //No tag was present so it must be a new tag
          for(int k=0;k<10;k++){ //Update the UID
              uidList[posKuhner][k] = uid[k];
            }
          tagUpdate[posKuhner] = true;
        }
        if(tagUpdate[posKuhner]) //if the tag was updated, read it and put it in memory
        {
          Serial.println("new tag detected at position " + String(positionKuhner) + ", will add it to the database");
          if(readCurrentTag(posKuhner)){
            Serial.println("New tag successfully read");
            tagToTransmit[posKuhner] = true;
            //PRINTLN("Error reading tag");
          }
          else{
            Serial.println("Error reading the tag the first time, will retry 5 times");
            long time_loop = millis();
            for(int k = 1;k<5;k++){

              clrc.transmit_enable(false);
              delay(20);
              clrc.iso14443a_init();
              clrc.transmit_enable(true);
              if(clrc.iso14443a_REQA()==2){
                len = clrc.iso14443a_select(uid, &sak);
                if(len<5){
                  Serial.println("Error Reading UID");
                }
                if(readCurrentTag(posKuhner)){
                  Serial.println("Managed to properly read the tag at loop " + String(k));
                  k=6;
                }
              }    
        
            }
            Serial.println("Retry loop took " + String(millis()-time_loop));
          }
          
        }
      }
      else{
        PRINTLN("No tag found");
        tagPresent[posKuhner] = false;
        tagReadable[posKuhner] = false;
        //Clear UID because if the tag was read while it was being removed it could still read the UID but not the data
        for(int i=0;i<10;i++){
            uidList[posKuhner][i] = 0;
        }
        if(tagPresentOld[posKuhner]){ //there was a tag, update lists
          tagUpdate[posKuhner] = true;
          tagToTransmit[posKuhner] = true;
          for(int i=0;i<144;i++){
            tagContentsStable[posKuhner][i]= 0;
          }
          for(int i=0;i<10;i++){
            uidList[posKuhner][i] = 0;
          }
        }
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
  /*
  for(int i = 0;i<5;i++){
    for(int j = 0;j<8;j++){
      Serial.print(tagPresent[j+i*8]);Serial.print("  ");
    }
    Serial.println("");
  }
  */
}

void readAllPresent(){
  //This function reads all baskets present in the incubator and store the read data in an array
  PRINTLN("Reading all tags present in the incubator");
  PRINTLN("****************************************************");
  PRINTLN("* mux\t| ch\t| add\t| ID");
  long time_1 = millis();
  for(int positionKuhner = 1; positionKuhner <=40; positionKuhner++){
    //Get the channel and position from the position in the Kuhner (1-40)
    uint8_t calcChannel = ceil(float(positionKuhner)/8)*2 - (positionKuhner%2?1:0)-1;//Gives the channel index based on the position 1-40
    uint8_t calcAddr = (ceil(float((positionKuhner%8))/2) + (positionKuhner%8?0:4))-1;//Gives the offset from 0x28 that should be applied to the reader addresses
    uint8_t posKuhner = positionKuhner-1;
    if(tagPresent[posKuhner]){
      pcaMux.channelSelect(calcChannel);
      
      if(clrc.init(CLRC663_BASE_ADDRESS + calcAddr)) {
      //Init Transmitter
      clrc.iso14443a_init();
      clrc.transmit_enable(true);
      
      // send request of iso14443 version A device
      if(clrc.iso14443a_REQA() == 2){
        tagPresent[posKuhner] = true;
        uint8_t data[16] = {0};
        uint8_t uid[10] = {0};
        uint8_t sak;
        uint8_t len = clrc.iso14443a_select(uidList[posKuhner], &sak);
        if(len<1){
          PRINTLN("Error selecting PICC");
        }     
        //Serial.print("\nUID : "); clrc.print_buffer_hex((uint8_t*) uid, len);

        readCurrentTag(posKuhner);
      }
      else{
        PRINTLN("No tag found");
        tagPresent[posKuhner] = false;
        for(int i = 0;i<144;i++){
            tagContentsStable[posKuhner][i] = 0;
        }
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
  Serial.println("Read all tags took : " + String(millis()-time_1) + " ms");
}
void readBasket(uint8_t positionKuhner){
  //This function read 1 tag at the position given in arg and put the result in tagContents and readTag
  PRINT("Reading basket at position ");PRINTLN(positionKuhner)
  PRINTLN("****************************************************");
  uint8_t calcChannel = ceil(float(positionKuhner)/8)*2 - (positionKuhner%2?1:0)-1;//Gives the channel index based on the position 1-40
  uint8_t calcAddr = (ceil(float((positionKuhner%8))/2) + (positionKuhner%8?0:4))-1;//Gives the offset from 0x28 that should be applied to the reader addresses
  uint8_t posKuhner = positionKuhner-1;
  pcaMux.channelSelect(calcChannel);
  
  if(clrc.init(CLRC663_BASE_ADDRESS + calcAddr)) {
    //Init Transmitter
    clrc.iso14443a_init();
    clrc.transmit_enable(true);
    
    // send request of iso14443 version A device
    if(clrc.iso14443a_REQA() == 2){
      tagPresent[posKuhner] = true;
      uint8_t data[16] = {0};
      uint8_t uid[10] = {0};
      uint8_t sak;
      uint8_t len = clrc.iso14443a_select(uidList[posKuhner], &sak);
      if(len<1){
        PRINTLN("Error selecting PICC");
      }     

      readCurrentTag(posKuhner);
    }
    else{
      PRINTLN("No tag found");
      tagPresent[posKuhner] = false;
      for(int i = 0;i<144;i++){
          tagContentsStable[posKuhner][i] = 0;
      }
      for(int i = 0;i<10;i++){
        uidList[posKuhner][i] = 0;
      }
    }
    
    clrc.transmit_enable(false);
  }//if(clrc.init) check that readers is responding
  else{
    PRINT("Reader didn't respond at position");PRINTLN(positionKuhner);
  }
    
  pcaMux.closeAll();
  PRINTLN("\r\n----------------------------------------------------");   
  PRINTLN("Done.");
}
void writeBasket(uint8_t positionKuhner){
  //This function write the content of 1 basket at the position given in arg with the data in writeBuff
  PRINT("Reading basket at position ");PRINTLN(positionKuhner)
  PRINTLN("****************************************************");
  uint8_t calcChannel = ceil(float(positionKuhner)/8)*2 - (positionKuhner%2?1:0)-1;//Gives the channel index based on the position 1-40
  uint8_t calcAddr = (ceil(float((positionKuhner%8))/2) + (positionKuhner%8?0:4))-1;//Gives the offset from 0x28 that should be applied to the reader addresses

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
      uint8_t len = clrc.iso14443a_select(uidList[positionKuhner-1], &sak);
      if(len<1){
        PRINTLN("Error selecting PICC");
      }     
      
      //Read 100% of user memory
      //Read user memory and place the bytes in an array
      //We have 144 byte of memory
      uint8_t FFkey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      clrc.cmd_load_key(FFkey); // load into the key buffer
      uint8_t readBuf[16];
      len = 0;
      //Write is done 4 bytes by 4 bytes
      for(uint8_t pageAdr = 0; pageAdr < 36; pageAdr++){
        len = clrc.MF_write_block(pageAdr+4, writeTag+pageAdr*4);
        if(len != 16){
          PRINTLN("Couldn't write block " + String(pageAdr));
        }
        delay(5);
      }
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
  PRINTLN("\r\n----------------------------------------------------");   
  PRINTLN("Done.");  
}

bool readCurrentTag(uint8_t posKuhner){
  //Read 100% of user memory
  //Read user memory and place the bytes in an array
  //We have 144 byte of memory
  uint8_t FFkey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  clrc.cmd_load_key(FFkey); // load into the key buffer
  uint8_t readBuf[16];

  uint8_t len = 0;
  //A block is 4 byte -> we need to read every 4 blocks not to create copy
  for(uint8_t blockAddr = 0; blockAddr < 9; blockAddr++){
    len += clrc.MF_read_block((blockAddr*4)+4, readBuf);
    if(len == 0){
      PRINT("Couldn't read block ");PRINTLN(blockAddr);
    }
    for(uint8_t byteAddr = 0; byteAddr < 16; byteAddr++){
      tagContents[posKuhner][blockAddr*16 + byteAddr] = readBuf[byteAddr]; //mux*32 + muxChannel*4+address
    }
    delay(5);
  }
  //If the read is correct then we write it in the Stable tagContents
  if(len == 144){
    PRINTLN("Managed to read the 144 bytes of User memory");
    for(int i = 0;i<144;i++){
      tagContentsStable[posKuhner][i] = tagContents[posKuhner][i];
    }
    tagReadable[posKuhner] = true;
    return true;
  }
  else{
    PRINTLN("Error reading memory");
    for(int i = 0;i<144;i++){
      //Serial.print(char(tagContents[posKuhner][i]));
      tagContentsStable[posKuhner][i] = i+33; //10 is \n and is used as separator, start at 33 (!) so that the char are all visible
    }
    tagReadable[posKuhner] = false;
    //Serial.println("");
    return false;
  }
}

//This function is called at most every 500 ms
void SlidingDoorTempControl() {
  float temp = 0;
  float setpoint = 40;
  float error = 0;
  float Kp = 1.0;
  float Kd = 3.0; //PID parameters to be tuned
  float Ki = 0.01;
  float ratio = 0;
  uint16_t rtd = thermo.readRTD();

  ratio = rtd;
  temp = thermo.temperature(1000,4300);
  //Serial.print("temperature = ");Serial.println(temp);
  error = setpoint-temp;
  long interval = millis()-lastMeasureTime;
  lastMeasureTime = millis();

    // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    error_count++;
    thermo.clearFault();
  }
  float derror = (error-(setpoint-lastTemp))/interval*1000;
  //PD Controller
  float PDResult = Kp*error + Kd*derror;
  //Serial.print("PDResult : ");Serial.println(PDResult);
  if(PDResult > 0 && !fault && temp > 20 && temp < 50){
    //Serial.println("Heating ON");
    digitalWrite(PIN_SSRCONTROL,HIGH);
  }
  else{
    //Serial.println("Heating OFF");
    digitalWrite(PIN_SSRCONTROL,LOW);
  }
  analogWrite(PIN_ANALOGTEMP,int(ceil((temp-4)*255/42.5))); //Between 12.5 and 55 C -> 3.4V when not plugged by USB at 37.5 -> Check with portenta
  if(error_count > 30){
    resetFunc();
  }
  lastTemp = temp;

}
