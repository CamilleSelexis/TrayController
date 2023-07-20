
#define Eprint(a)  client_pntr->print(a)
#define EprintFlag(a,flag) client_pntr->print(a,flag)
#define Eprintln(a) client_pntr->println(a)
#define Eprint2(a,b) client_pntr->print(a);client_pntr->print(b)
#define Ewrite(str,len) client_pntr->write(str,len)
#define cell_green  "<td style=\"width: 40px;background-color: lightgreen;\">"
#define cell_red    "<td style=\"width: 40px;background-color: red;\">"
#define cell_orange "<td style=\"width: 40px;background-color: orange;\">"

#define CELL_COLOR(i)  readers[i-1]? (tagPresent[i-1]?cell_green:cell_orange ): cell_red
void endConnection(EthernetClient* client_pntr){
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

//This function is called when the decapper can perform the asked action
void answerHTTP(EthernetClient* client_pntr,String currentLine){
  Eprint(Header);

  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println("<p><a href=\"http://" + StringIP + "/home\">Home</a></p>");
  client_pntr->println();
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

//This function is called when the call won't be performed
void answerHttpNo(EthernetClient* client_pntr,String currentLine, int state){
  Eprint(Header);

  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println();
  client_pntr->print("The decapper cannot perform this action now, status=");client_pntr->println(state);
  client_pntr->println("<p><a href=\"http://" + StringIP + "/home\">Home</a></p>");
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
//Display the home page
void homePage(EthernetClient* client_pntr){
  client_pntr->print(Header);
  client_pntr->print(homePageStr);
  
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
//This function is automatically called by the ajax script on the home page to update it every 2s
void updateHome(EthernetClient* client_pntr){
  String updateValues = "";
  for(int i = 0;i<40;i++){
    updateValues += String(tagPresent[i]+readers[i]); //gives 0 1 or 2 -> 0 : no reader // 1 : no tag //2 : tag present
    updateValues += "\r|\r";
  }
  client_pntr->print( Header );
  client_pntr->print(updateValues);
  //client_pntr->print( tagPresent[0] );   client_pntr->print( "|" );  client_pntr->print( tagPresent[1] );   client_pntr->print( "|" );  client_pntr->print( tagPresent[2] ); 
  Serial.println("data sent");
}
void showTagList(EthernetClient* client_pntr){
  client_pntr->print(Header);
  client_pntr->print(showTagListStr);
  
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
void updateTagList(EthernetClient* client_pntr){
  String updateValues = "";
  for(int i = 0;i<40;i++){
    for(int j=0;j<7;j++){
      updateValues += "0x"; //gives 0 1 or 2 -> 0 : no reader // 1 : no tag //2 : tag present
      updateValues += String(uidList[i][j],HEX); //gives 0 1 or 2 -> 0 : no reader // 1 : no tag //2 : tag present
      updateValues += " "; //gives 0 1 or 2 -> 0 : no reader // 1 : no tag //2 : tag present
    }
    updateValues += "\r|\r";
  }
  for(int i = 0;i<40;i++){
    for(int j=0;j<144;j++){
      updateValues += char(tagContents[i][j]); //gives 0 1 or 2 -> 0 : no reader // 1 : no tag //2 : tag present
    }
    updateValues += "\r|\r";
  }
  client_pntr->print( Header );
  client_pntr->print(updateValues);
  //client_pntr->print( tagPresent[0] );   client_pntr->print( "|" );  client_pntr->print( tagPresent[1] );   client_pntr->print( "|" );  client_pntr->print( tagPresent[2] ); 
  Serial.println("data sent");
  Serial.print("Total length of data = ");Serial.println(updateValues.length());
}

void SSRONCommand(EthernetClient* client_pntr,String currentLine) {
  int index = currentLine[6]-'0'; //Gives the index of the SSR to activate
  Serial.print("SSRON at index : ");Serial.println(index);
  enable_SSR(index-1); //The pcf takes index from 0 to 7
  client_pntr->print( Header );
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println("<p><a href=\"http://" + StringIP + "/home\">Home</a></p>");
  client_pntr->println();
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());

  while (client_pntr->read() != -1);
  client_pntr->stop();
}
void SSROFFCommand(EthernetClient* client_pntr,String currentLine) {
  int index = currentLine[7]-'0'; //Gives the index of the SSR to activate
  Serial.print("SSROFF at index : ");Serial.println(index);
  disable_SSR(index-1); //The pcf takes index from 0 to 7
  client_pntr->print( Header );
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println("<p><a href=\"http://" + StringIP + "/home\">Home</a></p>");
  client_pntr->println();
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());

  while (client_pntr->read() != -1);
  client_pntr->stop();
}
//This function is called to retrieve the arrays giving the full list of the tags
void getTagList(EthernetClient* client_pntr){
  client_pntr->print(Header);

  for(int positionKuhner = 0;positionKuhner<40;positionKuhner++){
    if(positionKuhner<9){
      Eprint("0");
    }
    Eprint2(positionKuhner+1,";");
    //Ewrite gives raw data
    Ewrite(uidList[positionKuhner],7); //UID are 7 byte long
    Eprint(";");
    Ewrite(tagContents[positionKuhner],144);
    client_pntr->println("|");
  }
  client_pntr->println("</body>");
  client_pntr->println("</html>");
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
//this function retrieves the tag datas from the basket at the given position
void readPosition(EthernetClient* client_pntr,String currentLine){
  int indexPosition = currentLine.length()-3; // Null char and 2 digits for the position
  uint8_t positionKuhner = 10* (currentLine[indexPosition]-'0') + currentLine[indexPosition+1]-'0';
  readBasket(positionKuhner);
  client_pntr->print(Header);
  client_pntr->write(readTag,144);
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
//this function needs a way to receive more data
void writePosition(EthernetClient* client_pntr,String currentLine){
  int indexPosition = 13; // /writePosition = 14 char -1 because array start at 0
  uint8_t positionKuhner = 10* (currentLine[indexPosition]-'0') + currentLine[indexPosition+1]-'0';
  writeBasket(positionKuhner);
  client_pntr->print(Header);
  client_pntr->write(writeTag,144);
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
