
/*
 * void HTTPTagLists(EthernetClient* client_pntr,String currentLine) {
  int state = getStatus();
  long current_time = millis();
  int seconds = (int) (current_time / 1000) % 60 ;
  int minutes = (int) ((current_time / (1000*60)) % 60);
  int hours   = (int) ((current_time / (1000*60*60)) % 24);
  char c[30];
  int l = sprintf(c, "%02d:%02d:%02d",hours,minutes,seconds);
  Eprint(HeaderRefresh2);
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  client_pntr->println("Refresh: 10");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->println("<!DOCTYPE HTML>");
  client_pntr->println("<html>");
  Eprint("<style> table, th, td { border:1px solid black; } </style>");
  client_pntr->println("<body>");
  client_pntr->println("<h1 style=\"text-align:center\">RFID Controller Kuhner </h1>");
  client_pntr->print("<p>Current Status : ");client_pntr->print(state);
  client_pntr->print("<img src=\"data:image/png;base64," + (state==3 ? yellow_dot:(state<2 ?green_dot:red_dot)) + "\" alt=\"Red dot\" />");
  client_pntr->println("</p>");
  Eprint("\n Current IP Address: ");Eprint(StringIP);
  //client_pntr->println("</p>");
  Eprint("<p> RFID tags list : </p>");
  Eprint("<table style=\"height:1000px; width: 100%; font-size: 14px; border-collapse: collapse; margin-left; auto; margin-right: auto;\" border=\"1\"><tbody>");
  Eprint("<tr>  <th> Position </th> <th> UID </th> <th> Tag Content</th> </tr>  ");
  for(int positionKuhner = 0; positionKuhner<40; positionKuhner++){
    Eprint("<tr style=\"height: 25px;\"> <th>" + String(positionKuhner+1) + "</th> <td>");
    for(int i = 0;i<7;i++){
      Eprint(" 0x");
      EprintFlag(uidList[positionKuhner][i],HEX);
    }
    //Ewrite(uidList[positionKuhner],10);
    Eprint("</td> <td>");
    Ewrite(tagContents[positionKuhner],144);
    Eprint(" </td></tr>");    
  }
  Eprint("</tbody>"
          "</table>" );         
  client_pntr->print("<p style=\"text-align:right\">");

  client_pntr->println("</body>");
  client_pntr->println("</html>");
  
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

void homePage(EthernetClient* client_pntr){

  int state = getStatus();
  long current_time = millis();
  int seconds = (int) (current_time / 1000) % 60 ;
  int minutes = (int) ((current_time / (1000*60)) % 60);
  int hours   = (int) ((current_time / (1000*60*60)) % 24);
  char c[30];
  int l = sprintf(c, "%02d:%02d:%02d",hours,minutes,seconds);
  Eprint(HeaderRefresh);

  Eprint("<!DOCTYPE HTML>"
          "<html>"
          "<style> table, th, td { border:1px solid black; } </style>"
          "<body>"
          "<h1 style=\"text-align:center\">RFID Controller Kuhner </h1>"
          "<p>Current Status : ");
  Eprint(state);
  client_pntr->print("<img src=\"data:image/png;base64," + (state==3 ? yellow_dot:(state<2 ?green_dot:red_dot)) + "\" alt=\"Red dot\" />");
  client_pntr->println("</p>");
  Eprint("\n Current IP Address: ");Eprint(StringIP);
  //client_pntr->println("</p>");
  Eprint("<p> Current Kuhner Tray : </p>");
  Eprint("<table style=\"height:200px; width: 320px; border-collapse: collapse; margin-left; auto; margin-right: auto;\" border=\"1\"><tbody>");

  //client_pntr->print("<tr style=\"height: 18px;\">" readers[32]?cell_orange:cell_red "33 </td>");
  //client_pntr->print(readers[32]?cell_orange:cell_red);
  
  Eprint("<tr style=\"height: 40px;\">"); 
  Eprint2(CELL_COLOR(33),"33 </td>");
  Eprint2(CELL_COLOR(34),"34 </td>");
  Eprint2(CELL_COLOR(35),"35 </td>");
  Eprint2(CELL_COLOR(36),"36 </td>");
  Eprint2(CELL_COLOR(37),"37 </td>");
  Eprint2(CELL_COLOR(38),"38 </td>");
  Eprint2(CELL_COLOR(39),"39 </td>");
  Eprint2(CELL_COLOR(40),"40 </td>");
  Eprint("</tr> ");
  Eprint("<tr style=\"height: 40px;\">"); 
  Eprint2(CELL_COLOR(25),"25 </td>");
  Eprint2(CELL_COLOR(26),"26 </td>");
  Eprint2(CELL_COLOR(27),"27 </td>");
  Eprint2(CELL_COLOR(28),"28 </td>");
  Eprint2(CELL_COLOR(29),"29 </td>");
  Eprint2(CELL_COLOR(30),"30 </td>");
  Eprint2(CELL_COLOR(31),"31 </td>");
  Eprint2(CELL_COLOR(32),"32 </td>");
  Eprint("</tr> ");
  Eprint("<tr style=\"height: 40px;\">"); 
  Eprint2(CELL_COLOR(17),"17 </td>");
  Eprint2(CELL_COLOR(18),"18 </td>");
  Eprint2(CELL_COLOR(19),"19 </td>");
  Eprint2(CELL_COLOR(20),"20 </td>");
  Eprint2(CELL_COLOR(21),"21 </td>");
  Eprint2(CELL_COLOR(22),"22 </td>");
  Eprint2(CELL_COLOR(23),"23 </td>");
  Eprint2(CELL_COLOR(24),"24 </td>");
  Eprint("</tr> ");
  Eprint("<tr style=\"height: 40px;\">"); 
  Eprint2(CELL_COLOR(9),"9 </td>");
  Eprint2(CELL_COLOR(10),"10 </td>");
  Eprint2(CELL_COLOR(11),"11 </td>");
  Eprint2(CELL_COLOR(12),"12 </td>");
  Eprint2(CELL_COLOR(13),"13 </td>");
  Eprint2(CELL_COLOR(14),"14 </td>");
  Eprint2(CELL_COLOR(15),"15 </td>");
  Eprint2(CELL_COLOR(16),"16 </td>");
  Eprint("</tr> ");
  Eprint("<tr style=\"height: 40px;\">"); 
  Eprint2(CELL_COLOR(1),"1 </td>");
  Eprint2(CELL_COLOR(2),"2 </td>");
  Eprint2(CELL_COLOR(3),"3 </td>");
  Eprint2(CELL_COLOR(4),"4 </td>");
  Eprint2(CELL_COLOR(5),"5 </td>");
  Eprint2(CELL_COLOR(6),"6 </td>");
  Eprint2(CELL_COLOR(7),"7 </td>");
  Eprint2(CELL_COLOR(8),"8 </td>");
  Eprint("</tr> ");
  Eprint("</tbody>"
          "</table>" );
  client_pntr->println("<p><a href=\"http://" + StringIP + "/showTagList\">showTagList</a></p>");
  Eprint("<p><span style=\"background-color: lightgreen;\">Basket Present<br />"
  "<span style=\"background-color: orange;\">Reader Present<br />"
  "<span style=\"background-color: red;\">No Reader Detected</span></span><br /></span></p>");
  Eprint2("<p style=\"text-align:right\">"
          "Connection closed by the server at internal time : "
          ,String(millis()));
  Eprint("</body>"
          "</html>"
          );      
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

*/
