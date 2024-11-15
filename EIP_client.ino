//Connect to the Net-Z controller and send a get_attribute_single request
// returns the requested attribute as a raw value that needs to be scaled down (/100 for temp,humidity and CO2, /10 for RPM
int get_attribute_single(byte instance){
  //Connect to the Net-Z interface and read the attribute
  IPAddress ip_server_EIP(192,168,1,62);
  int port_EthernetIP = 44818;
  EthernetClient client_EIP;
  byte register_session[50] = {0x65, 0x00, 0x04, 0x00, 0x00, 0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
     ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00};
  byte unregister_session[50] = {0x66 ,0x00 ,0x00 ,0x00 ,0x04 ,0x00 ,0x02 ,0xe0 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
   ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
  //get_attribute single message
  byte get_attribute_message[50] = {0x6f, 0x00, 0x18, 0x00, 0x04, 0x00, 0x02, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
     0x00, 0x00, 0x00, 0x00, 0xb2, 0x00, 0x08, 0x00, 0x0e, 0x03, 0x20, 0xa2, 0x24, 0x02, 0x30, 0x05}; //byte[45] is the instance, 1=shaker, 2= temp, 3=humidity, 4=CO2
  byte buff_CIP[100];
  int value=0;
  if(instance >0 && instance < 30){
    get_attribute_message[45] = instance;
  }
  
  if(client_EIP.connect(ip_server_EIP,44818)) { // Port used for EthernetIP
    Serial.println("connected !");
    Serial.println("Register session");
    client_EIP.write(register_session,28 );
    delay(200);
    int count = 0;
    while(client_EIP.available()){
      byte c  = client_EIP.read();
      buff_CIP[count] = c;
      Serial.print("0x");
      Serial.print(c,HEX);
      Serial.print("  ");
      count++;
    }
    //Set session in get_attribute_single message
    get_attribute_message[4] = buff_CIP[4];
    get_attribute_message[5] = buff_CIP[5];
    get_attribute_message[6] = buff_CIP[6];
    get_attribute_message[7] = buff_CIP[7];
    //Set session in unregister session msg
    unregister_session[4] = buff_CIP[4];
    unregister_session[5] = buff_CIP[5];
    unregister_session[6] = buff_CIP[6];
    unregister_session[7] = buff_CIP[7];
    Serial.println();
    Serial.println("Send get_attribute single request");
    client_EIP.write(get_attribute_message,48);
    delay(1000);
    count = 0;
    while(client_EIP.available()){
      byte c  = client_EIP.read();
      buff_CIP[count] = c;
      Serial.print("0x");
      Serial.print(c,HEX);
      Serial.print("  ");
      count++;
    }
    //Compute value from 2 last bytes
    value = buff_CIP[45]*256 + buff_CIP[44];
    Serial.println();
    Serial.print("Read attribute ");Serial.print(instance);Serial.print(" returned value ");Serial.println(value);
    client_EIP.write(unregister_session,24);
    delay(500);
    count = 0;
    while(client_EIP.available()){
      byte c  = client_EIP.read();
      Serial.print("0x");
      Serial.print(c,HEX);
      Serial.print("  ");
    }
    client_EIP.stop();
    Serial.println("client disconnected from server");
  }
  else{
    Serial.println("Unable to connect to client");
  }
  
  return value;
}

//Connect to the Net-Z controller and send a get_attribute_single request
// returns the requested attribute as a raw value that needs to be scaled down (/100 for temp,humidity and CO2, /10 for RPM
void get_attribute_single_allPV(){
  //Connect to the Net-Z interface and read the attribute
  IPAddress ip_server_EIP(192,168,1,62);
  int port_EthernetIP = 44818;
  EthernetClient client_EIP;
  byte register_session[50] = {0x65, 0x00, 0x04, 0x00, 0x00, 0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
     ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00};
  byte unregister_session[50] = {0x66 ,0x00 ,0x00 ,0x00 ,0x04 ,0x00 ,0x02 ,0xe0 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
   ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
  //get_attribute single message
  byte get_attribute_message[50] = {0x6f, 0x00, 0x18, 0x00, 0x04, 0x00, 0x02, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
     0x00, 0x00, 0x00, 0x00, 0xb2, 0x00, 0x08, 0x00, 0x0e, 0x03, 0x20, 0xa2, 0x24, 0x02, 0x30, 0x05}; //byte[45] is the instance, 1=shaker, 2= temp, 3=humidity, 4=CO2
  byte buff_CIP[100];
  
  if(client_EIP.connect(ip_server_EIP,44818)) { // Port used for EthernetIP
    Serial.println("connected !");
    Serial.println("Register session");
    client_EIP.write(register_session,28 );
    delay(200);
    int count = 0;
    while(client_EIP.available()){
      byte c  = client_EIP.read();
      buff_CIP[count] = c;
      Serial.print("0x");
      Serial.print(c,HEX);
      Serial.print("  ");
      count++;
    }
    //Set session in get_attribute_single message
    get_attribute_message[4] = buff_CIP[4];
    get_attribute_message[5] = buff_CIP[5];
    get_attribute_message[6] = buff_CIP[6];
    get_attribute_message[7] = buff_CIP[7];
    //Set session in unregister session msg
    unregister_session[4] = buff_CIP[4];
    unregister_session[5] = buff_CIP[5];
    unregister_session[6] = buff_CIP[6];
    unregister_session[7] = buff_CIP[7];
    Serial.println();
    Serial.println("Send get_attribute single request");
    int value_array[4];
    for(int j=1;j<5;j++){
      get_attribute_message[45] = j;
      if(j>4){
        get_attribute_message[45] = j+1;
      }
      client_EIP.write(get_attribute_message,48);
      delay(1000);
      count = 0;
      while(client_EIP.available()){
        byte c  = client_EIP.read();
        buff_CIP[count] = c;
        Serial.print("0x");
        Serial.print(c,HEX);
        Serial.print("  ");
        count++;
      }
      //Compute value from 2 last bytes
      value_array[j-1] = buff_CIP[45]*256 + buff_CIP[44];
      Serial.println();
      Serial.print("Read attribute ");Serial.print(j);Serial.print(" returned value ");Serial.println(value_array[j-1]);
    }
    kuhner_shaking_current = value_array[0];
    kuhner_temp_current = value_array[1];
    kuhner_humidity_current = value_array[2];
    kuhner_CO2_current = value_array[3];
    
    client_EIP.write(unregister_session,24);
    delay(500);
    count = 0;
    while(client_EIP.available()){
      byte c  = client_EIP.read();
      Serial.print("0x");
      Serial.print(c,HEX);
      Serial.print("  ");
    }
    client_EIP.stop();
    Serial.println("client disconnected from server");
  }
  else{
    Serial.println("Unable to connect to client");
  }
}

//Connect to the Net-Z controller and send a get_attribute_single request
// returns the requested attribute as a raw value that needs to be scaled down (/100 for temp,humidity and CO2, /10 for RPM
void get_attribute_single_allSetpoint(){
  //Connect to the Net-Z interface and read the attribute
  IPAddress ip_server_EIP(192,168,1,62);
  int port_EthernetIP = 44818;
  EthernetClient client_EIP;
  byte register_session[50] = {0x65, 0x00, 0x04, 0x00, 0x00, 0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
     ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00};
  byte unregister_session[50] = {0x66 ,0x00 ,0x00 ,0x00 ,0x04 ,0x00 ,0x02 ,0xe0 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
   ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
  //get_attribute single message
  byte get_attribute_message[50] = {0x6f, 0x00, 0x18, 0x00, 0x04, 0x00, 0x02, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
     0x00, 0x00, 0x00, 0x00, 0xb2, 0x00, 0x08, 0x00, 0x0e, 0x03, 0x20, 0xa2, 0x24, 0x02, 0x30, 0x05}; //byte[45] is the instance, 1=shaker, 2= temp, 3=humidity, 4=CO2
  byte buff_CIP[100];
  
  if(client_EIP.connect(ip_server_EIP,44818)) { // Port used for EthernetIP
    Serial.println("connected !");
    Serial.println("Register session");
    client_EIP.write(register_session,28 );
    delay(200);
    int count = 0;
    while(client_EIP.available()){
      byte c  = client_EIP.read();
      buff_CIP[count] = c;
      Serial.print("0x");
      Serial.print(c,HEX);
      Serial.print("  ");
      count++;
    }
    //Set session in get_attribute_single message
    get_attribute_message[4] = buff_CIP[4];
    get_attribute_message[5] = buff_CIP[5];
    get_attribute_message[6] = buff_CIP[6];
    get_attribute_message[7] = buff_CIP[7];
    //Set session in unregister session msg
    unregister_session[4] = buff_CIP[4];
    unregister_session[5] = buff_CIP[5];
    unregister_session[6] = buff_CIP[6];
    unregister_session[7] = buff_CIP[7];
    Serial.println();
    Serial.println("Send get_attribute single request");
    int value_array[4];
    for(int j=1;j<5;j++){
      get_attribute_message[45] = j+5;
      client_EIP.write(get_attribute_message,48);
      delay(1000);
      count = 0;
      while(client_EIP.available()){
        byte c  = client_EIP.read();
        buff_CIP[count] = c;
        Serial.print("0x");
        Serial.print(c,HEX);
        Serial.print("  ");
        count++;
      }
      //Compute value from 2 last bytes
      value_array[j-1] = buff_CIP[45]*256 + buff_CIP[44];
      Serial.println();
      Serial.print("Read attribute ");Serial.print(j);Serial.print(" returned value ");Serial.println(value_array[j-1]);
    }
    kuhner_shaking_setpoint = value_array[0];
    kuhner_temp_setpoint = value_array[1];
    kuhner_humidity_setpoint = value_array[2];
    kuhner_CO2_setpoint = value_array[3];
    
    client_EIP.write(unregister_session,24);
    delay(500);
    count = 0;
    while(client_EIP.available()){
      byte c  = client_EIP.read();
      Serial.print("0x");
      Serial.print(c,HEX);
      Serial.print("  ");
    }
    client_EIP.stop();
    Serial.println("client disconnected from server");
  }
  else{
    Serial.println("Unable to connect to client");
  }
}
