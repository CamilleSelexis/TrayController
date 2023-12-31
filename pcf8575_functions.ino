void init_pcf8575(int adr){
  //PCF8575 pcf8575(adr); //Define the pcf8575 with its adr
  pcf8575.begin();
  pcf8575.pinMode(P0,OUTPUT);
  pcf8575.pinMode(P1,OUTPUT);
  pcf8575.pinMode(P2,OUTPUT);
  pcf8575.pinMode(P3,OUTPUT);
  pcf8575.pinMode(P4,OUTPUT);
  pcf8575.pinMode(P5,OUTPUT);
  pcf8575.pinMode(P6,OUTPUT);
  pcf8575.pinMode(P7,OUTPUT);
  
  //Disable all the SSR
  pcf8575.digitalWrite(P0,HIGH);
  pcf8575.digitalWrite(P1,HIGH);
  pcf8575.digitalWrite(P2,HIGH);
  pcf8575.digitalWrite(P3,HIGH);
  pcf8575.digitalWrite(P4,HIGH);
  pcf8575.digitalWrite(P5,HIGH);
  pcf8575.digitalWrite(P6,HIGH);
  pcf8575.digitalWrite(P7,HIGH);
}
//Enable the magnet on the given row - row 0 is on the left
bool enable_SSR(byte row){
  pcf8575.pinMode(row,OUTPUT);
  pcf8575.digitalWrite(row,LOW);
  bSSRON = true;
  Serial.println("Starting the SSR on row" + String(row));
  if(start_SSR == 0){ //Only change the SSR time if it was at 0 to avoid overheating the EM
    start_SSR = millis();
  }
}

//rows is 0 or 4
void disable_SSR(byte row){
  pcf8575.digitalWrite(row,HIGH);
}
