int scan_i2c(int addresses[128]){
  //Perform an i2c scan and print the results
  int nDevices = 0;

  Serial.println("Scanning...");

  for (byte address = 1; address < 127; ++address) {
    // The i2c_scanner uses the return value of
    // the Wire.endTransmission to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      addresses[nDevices] = address;
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println("  !");

      ++nDevices;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }
  delay(100); // Wait 5 seconds for next scan
  return nDevices;
}

void scan_readers(bool readers[40]){
  //Scan all possible addresses for the readers and return which positions are occupied
  int base_addr = 0x28;
  int mux_1 = 0x70;
  int mux_2 = 0x77;
  for(int positionKuhner = 1; positionKuhner <= 40; positionKuhner++){
    //Get the channel and position from the position in the Kuhner (1-40)
    uint8_t calcChannel = ceil(float(positionKuhner)/8)*2 - (positionKuhner%2?1:0) -1;//Gives the channel index based on the position 1-40
    uint8_t calcAddr = (ceil(float((positionKuhner%8))/2) + (positionKuhner%8?0:4))-1;//Gives the offset from 0x28 that should be applied to the reader addresses
    pcaMux.channelSelect(calcChannel);
    if(clrc.init(CLRC663_BASE_ADDRESS + calcAddr))    readers[positionKuhner-1]=true;
    else readers[positionKuhner-1]=false;
    pcaMux.closeAll();

    #if DEBUG
    Serial.print("Position ");Serial.println(positionKuhner);
    Serial.print("Channel ");Serial.println(calcChannel);
    Serial.print("Addr ");Serial.println(CLRC663_BASE_ADDRESS + calcAddr, HEX);
    Serial.println("-----------------------------");
    #endif

  }
  /*
  for(int i = 0; i<10;i++){ //Increment channel number
    clrc
    for(int j= 0;j<4;j++){
      int positionKuhner = 
      Wire.beginTransmission(base_addr+j);
      byte error = Wire.endTransmission();
      if(error == 0){ //Reader found
        readers[positionKuhner] = true;
      }
      else{
        readers[positionKuhner] = false;
      }
    }
  }
  */
}
