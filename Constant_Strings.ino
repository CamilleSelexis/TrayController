
String red_dot = "iVBORw0KGgoAAAANSUhEUgAAAAsAAAALCAYAAACprHcmAAAAAXNSR0IArs4c6QAAAERJREFUKFNjZEADb2VU/oOEhJ/cYUSXgwvAFKErQNYEVoxLIUwjTANpigmZimw6I0mKaedmmJuIDjrksCUqUtAjAxsfAG6/KAyiiHWSAAAAAElFTkSuQmCC";
String green_dot = "iVBORw0KGgoAAAANSUhEUgAAAAsAAAALCAYAAACprHcmAAAAAXNSR0IArs4c6QAAAERJREFUKFNjZEADcs/C/oOEHkmtYkSXgwvAFKErQNYEVoxLIUwjTANpigmZimw6I0mKaedmmJuIDjrksCUqUtAjAxsfAJ6PKAznF8FZAAAAAElFTkSuQmCC";
String yellow_dot = "iVBORw0KGgoAAAANSUhEUgAAAAsAAAALCAYAAACprHcmAAAAAXNSR0IArs4c6QAAAERJREFUKFNjZEAD/0/y/QcJMZp/YkSXgwvAFGEoQNIEVoxLIUwjzBbSFBMyFdl0RpIU087NMDcRHXTIYUtUpKBHBjY+ACJeKAzktf86AAAAAElFTkSuQmCC";

String Header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
String HeaderBad = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
String HeaderRefresh = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nRefresh: 5\r\n\r\n";
String HeaderRefresh2 = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nRefresh: 10\r\n\r\n";
String HeaderOk = "HTTP/1.1 200 OK\r\n";
String homePageStr = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body {font-size:100%;} 
    #main {display: table; margin: auto;  padding: 10px 10px 10px 10px; } 
    #content { border: 5px solid blue; border-radius: 15px; padding: 10px 0px 10px 0px;}
    #commands { border: 3px solid yellow; border-radius: 9px; padding: 5px 0px 5px 0px;}
    h2 {text-align:center; margin: 10px 0px 10px 0px;} 
    p { text-align:center; margin: 5px 0px 10px 0px; font-size: 120%;}
    td { text-align:center; font-size: 120%;}
    #time_P { margin: 10px 0px 15px 0px;}
  </style>
 
  <script> 
    function updateTime() 
    {  
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('P_time').innerHTML = t;
    }
 
    function updateTray()
    {
      ajaxLoad('updateStartPage');
    }
    
    var ajaxRequest = null;
    if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
    else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }
 
    function ajaxLoad(ajaxURL)
    {
      if(!ajaxRequest){ alert('AJAX is not supported.'); return; }
 
      ajaxRequest.open('GET',ajaxURL,true);
      ajaxRequest.onreadystatechange = function()
      {
        if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
        {
          var ajaxResult = ajaxRequest.responseText;
          var tmpArray = ajaxResult.split("|");
          if(tmpArray.length>39){
          document.getElementById('Tag1').style.backgroundColor = (tmpArray[0]>0)?((tmpArray[0]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag2').style.backgroundColor = (tmpArray[1]>0)?((tmpArray[1]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag3').style.backgroundColor = (tmpArray[2]>0)?((tmpArray[2]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag4').style.backgroundColor = (tmpArray[3]>0)?((tmpArray[3]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag5').style.backgroundColor = (tmpArray[4]>0)?((tmpArray[4]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag6').style.backgroundColor = (tmpArray[5]>0)?((tmpArray[5]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag7').style.backgroundColor = (tmpArray[6]>0)?((tmpArray[6]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag8').style.backgroundColor = (tmpArray[7]>0)?((tmpArray[7]>1)?'lightgreen':'orange'):'red';

          document.getElementById('Tag9').style.backgroundColor = (tmpArray[8]>0)?((tmpArray[8]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag10').style.backgroundColor = (tmpArray[9]>0)?((tmpArray[9]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag11').style.backgroundColor = (tmpArray[10]>0)?((tmpArray[10]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag12').style.backgroundColor = (tmpArray[11]>0)?((tmpArray[11]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag13').style.backgroundColor = (tmpArray[12]>0)?((tmpArray[12]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag14').style.backgroundColor = (tmpArray[13]>0)?((tmpArray[13]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag15').style.backgroundColor = (tmpArray[14]>0)?((tmpArray[14]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag16').style.backgroundColor = (tmpArray[15]>0)?((tmpArray[15]>1)?'lightgreen':'orange'):'red';

          document.getElementById('Tag17').style.backgroundColor = (tmpArray[16]>0)?((tmpArray[16]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag18').style.backgroundColor = (tmpArray[17]>0)?((tmpArray[17]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag19').style.backgroundColor = (tmpArray[18]>0)?((tmpArray[18]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag20').style.backgroundColor = (tmpArray[19]>0)?((tmpArray[19]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag21').style.backgroundColor = (tmpArray[20]>0)?((tmpArray[20]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag22').style.backgroundColor = (tmpArray[21]>0)?((tmpArray[21]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag23').style.backgroundColor = (tmpArray[22]>0)?((tmpArray[22]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag24').style.backgroundColor = (tmpArray[23]>0)?((tmpArray[23]>1)?'lightgreen':'orange'):'red';

          document.getElementById('Tag25').style.backgroundColor = (tmpArray[24]>0)?((tmpArray[24]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag26').style.backgroundColor = (tmpArray[25]>0)?((tmpArray[25]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag27').style.backgroundColor = (tmpArray[26]>0)?((tmpArray[26]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag28').style.backgroundColor = (tmpArray[27]>0)?((tmpArray[27]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag29').style.backgroundColor = (tmpArray[28]>0)?((tmpArray[28]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag30').style.backgroundColor = (tmpArray[29]>0)?((tmpArray[29]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag31').style.backgroundColor = (tmpArray[30]>0)?((tmpArray[30]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag32').style.backgroundColor = (tmpArray[31]>0)?((tmpArray[31]>1)?'lightgreen':'orange'):'red';

          document.getElementById('Tag33').style.backgroundColor = (tmpArray[32]>0)?((tmpArray[32]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag34').style.backgroundColor = (tmpArray[33]>0)?((tmpArray[33]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag35').style.backgroundColor = (tmpArray[34]>0)?((tmpArray[34]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag36').style.backgroundColor = (tmpArray[35]>0)?((tmpArray[35]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag37').style.backgroundColor = (tmpArray[36]>0)?((tmpArray[36]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag38').style.backgroundColor = (tmpArray[37]>0)?((tmpArray[37]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag39').style.backgroundColor = (tmpArray[38]>0)?((tmpArray[38]>1)?'lightgreen':'orange'):'red';
          document.getElementById('Tag40').style.backgroundColor = (tmpArray[39]>0)?((tmpArray[39]>1)?'lightgreen':'orange'):'red';

          document.getElementById('Temp').innerHTML = tmpArray[40];
          document.getElementById('Temp').style.backgroundColor = (tmpArray[40]>40)?'orange':'lightblue';
          
          var TimeAliveDay = Math.floor(tmpArray[41]/86400);
          tmpArray[41] = tmpArray[41]-86400*TimeAliveDay;
          var TimeAliveHour = Math.floor(tmpArray[41]/3600);
          tmpArray[41] = tmpArray[41]-3600*TimeAliveHour;
          var TimeAliveMin = Math.floor(tmpArray[41]/60);
          tmpArray[41] = tmpArray[41]-60*TimeAliveMin;
          var TimeAliveSec = Math.floor(tmpArray[41]);

          document.getElementById('Time_Alive_d').innerHTML = TimeAliveDay;
          document.getElementById('Time_Alive_h').innerHTML = TimeAliveHour;
          document.getElementById('Time_Alive_m').innerHTML = TimeAliveMin;
          document.getElementById('Time_Alive_s').innerHTML = TimeAliveSec;
          
          }
        }
      }
      ajaxRequest.send();
    }
 
   function ajaxCommand(ajaxURL){
      ajaxRequest.open('GET',ajaxURL,true);
        ajaxRequest.onreadystatechange = function()
        {
          if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
          {
            var ajaxResult = ajaxRequest.responseText;
          }
        }
        ajaxRequest.send();
        alert(ajaxURL);
    }
    var myVar1 = setInterval(updateTray, 4000);  
    var myVar2 = setInterval(updateTime, 1000);  
 
  </script>

    <title>Kuhner RFID Controller</title>
 </head>
 
 <body>
   <div id='main'>
     <h2>Kuhner RFID Controller</h2>
      <p id='P_time'>Time</p>
      <p> Time Alive : <b id ='Time_Alive_d'>Day</b> / <b id ='Time_Alive_h'>Hour</b> : <b id ='Time_Alive_m'>Min</b> : <b id ='Time_Alive_s'>Sec</b></p>
      <p> Side door Temperature : <b id ='Temp'>Temp</b></p>
     <div id='content'> 
     <table style="height:420px; width: 640px;border-collapse: collapse; margin-left; auto; margin-right: auto;" border="1"><tbody>
        <tr style="height: 80px;">
          <td id = 'Tag33'> 33</td>
          <td id = 'Tag34'> 34</td>
          <td id = 'Tag35'> 35</td>
          <td id = 'Tag36'> 36</td>
          <td id = 'Tag37'> 37</td>
          <td id = 'Tag38'> 38</td>
          <td id = 'Tag39'> 39</td>
          <td id = 'Tag40'> 40</td>
        </tr>
        <tr style="height: 80px;">
          <td id = 'Tag25'> 25</td>
          <td id = 'Tag26'> 26</td>
          <td id = 'Tag27'> 27</td>
          <td id = 'Tag28'> 28</td>
          <td id = 'Tag29'> 29</td>
          <td id = 'Tag30'> 30</td>
          <td id = 'Tag31'> 31</td>
          <td id = 'Tag32'> 32</td>
        </tr>
        <tr style="height: 80px;">
          <td id = 'Tag17'> 17</td>
          <td id = 'Tag18'> 18</td>
          <td id = 'Tag19'> 19</td>
          <td id = 'Tag20'> 20</td>
          <td id = 'Tag21'> 21</td>
          <td id = 'Tag22'> 22</td>
          <td id = 'Tag23'> 23</td>
          <td id = 'Tag24'> 24</td>
        </tr>
        <tr style="height: 80px;">
          <td id = 'Tag9'> 9</td>
          <td id = 'Tag10'> 10</td>
          <td id = 'Tag11'> 11</td>
          <td id = 'Tag12'> 12</td>
          <td id = 'Tag13'> 13</td>
          <td id = 'Tag14'> 14</td>
          <td id = 'Tag15'> 15</td>
          <td id = 'Tag16'> 16</td>
        </tr>
        <tr style="height: 80px;">
          <td id = 'Tag1'> 1 </td>
          <td id = 'Tag2'> 2 </td>
          <td id = 'Tag3'> 3 </td>
          <td id = 'Tag4'> 4 </td>
          <td id = 'Tag5'> 5 </td>
          <td id = 'Tag6'> 6 </td>
          <td id = 'Tag7'> 7 </td>
          <td id = 'Tag8'> 8 </td>
        </tr>
        <tr style="height: 20px;">
          <td><input type="button"onclick = "ajaxCommand('/SSRON1')" value = "EM OFF"></td>
          <td><input type="button"onclick = "ajaxCommand('/SSRON2')" value = "EM OFF"></td>
          <td><input type="button"onclick = "ajaxCommand('/SSRON3')" value = "EM OFF"></td>
          <td><input type="button"onclick = "ajaxCommand('/SSRON4')" value = "EM OFF"></td>
          <td><input type="button"onclick = "ajaxCommand('/SSRON5')" value = "EM OFF"></td>
          <td><input type="button"onclick = "ajaxCommand('/SSRON6')" value = "EM OFF"></td>
          <td><input type="button"onclick = "ajaxCommand('/SSRON7')" value = "EM OFF"></td>
          <td><input type="button"onclick = "ajaxCommand('/SSRON8')" value = "EM OFF"></td>
        </tr>
    </table>
     </div>
  <p><span style="background-color: lightgreen;">Basket Present</span><br />
  <span style="background-color: orange;">Reader Present</span><br />
  <span style="background-color: red;">No Reader Detected</span><br /></p>
  <div id='commands'> 
  <p>
    <a href="http://192.168.1.81/showTagList">showTagList</a><br />
  </p>
   </div> 
 </body>
</html>
)=====";

String showTagListStr = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body {font-size:100%;} 
    #main {display: table; margin: auto;  padding: 10px 10px 10px 10px; } 
    #content { border: 5px solid blue; border-radius: 15px; padding: 10px 0px 10px 0px;}
    #commands { border: 3px solid yellow; border-radius: 9px; padding: 5px 0px 5px 0px;}
    h2 {text-align:center; margin: 10px 0px 10px 0px;}
    h3 {text-align:center; margin: 10px 0px 10px 0px;}  
    p { text-align:center; margin: 5px 0px 10px 0px; font-size: 120%;}
    td { text-align:left; font-size: 120%;}
    #time_P { margin: 10px 0px 15px 0px;}
  </style>
 
  <script> 
    function updateTime() 
    {  
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('P_time').innerHTML = t;
    }
 
    function updateTagList()
    {
      ajaxLoad('updateTagList');
    }
    
    var ajaxRequest = null;
    if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
    else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }
 
    function ajaxLoad(ajaxURL)
    {
      if(!ajaxRequest){ alert('AJAX is not supported.'); return; }
 
      ajaxRequest.open('GET',ajaxURL,true);
      ajaxRequest.onreadystatechange = function()
      {
        if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
        {
          var ajaxResult = ajaxRequest.responseText;
          var tmpArray = ajaxResult.split("|");
          if(tmpArray.length>79){
            document.getElementById('uid1').innerHTML = tmpArray[0];
            document.getElementById('uid2').innerHTML = tmpArray[1];
            document.getElementById('uid3').innerHTML = tmpArray[2];
            document.getElementById('uid4').innerHTML = tmpArray[3];
            document.getElementById('uid5').innerHTML = tmpArray[4];
            document.getElementById('uid6').innerHTML = tmpArray[5];
            document.getElementById('uid7').innerHTML = tmpArray[6];
            document.getElementById('uid8').innerHTML = tmpArray[7];

            document.getElementById('uid9').innerHTML = tmpArray[8];
            document.getElementById('uid10').innerHTML = tmpArray[9];
            document.getElementById('uid11').innerHTML = tmpArray[10];
            document.getElementById('uid12').innerHTML = tmpArray[11];
            document.getElementById('uid13').innerHTML = tmpArray[12];
            document.getElementById('uid14').innerHTML = tmpArray[13];
            document.getElementById('uid15').innerHTML = tmpArray[14];
            document.getElementById('uid16').innerHTML = tmpArray[15];

            document.getElementById('uid17').innerHTML = tmpArray[16];
            document.getElementById('uid18').innerHTML = tmpArray[17];
            document.getElementById('uid19').innerHTML = tmpArray[18];
            document.getElementById('uid20').innerHTML = tmpArray[19];
            document.getElementById('uid21').innerHTML = tmpArray[20];
            document.getElementById('uid22').innerHTML = tmpArray[21];
            document.getElementById('uid23').innerHTML = tmpArray[22];
            document.getElementById('uid24').innerHTML = tmpArray[23];

            document.getElementById('uid25').innerHTML = tmpArray[24];
            document.getElementById('uid26').innerHTML = tmpArray[25];
            document.getElementById('uid27').innerHTML = tmpArray[26];
            document.getElementById('uid28').innerHTML = tmpArray[27];
            document.getElementById('uid29').innerHTML = tmpArray[28];
            document.getElementById('uid30').innerHTML = tmpArray[29];
            document.getElementById('uid31').innerHTML = tmpArray[30];
            document.getElementById('uid32').innerHTML = tmpArray[31];

            document.getElementById('uid33').innerHTML = tmpArray[32];
            document.getElementById('uid34').innerHTML = tmpArray[33];
            document.getElementById('uid35').innerHTML = tmpArray[34];
            document.getElementById('uid36').innerHTML = tmpArray[35];
            document.getElementById('uid37').innerHTML = tmpArray[36];
            document.getElementById('uid38').innerHTML = tmpArray[37];
            document.getElementById('uid39').innerHTML = tmpArray[38];
            document.getElementById('uid40').innerHTML = tmpArray[39];
            
            
            document.getElementById('content1').innerHTML = tmpArray[40];
            document.getElementById('content2').innerHTML = tmpArray[41];
            document.getElementById('content3').innerHTML = tmpArray[42];
            document.getElementById('content4').innerHTML = tmpArray[43];
            document.getElementById('content5').innerHTML = tmpArray[44];
            document.getElementById('content6').innerHTML = tmpArray[45];
            document.getElementById('content7').innerHTML = tmpArray[46];
            document.getElementById('content8').innerHTML = tmpArray[47];

            document.getElementById('content9').innerHTML = tmpArray[48];
            document.getElementById('content10').innerHTML = tmpArray[49];
            document.getElementById('content11').innerHTML = tmpArray[50];
            document.getElementById('content12').innerHTML = tmpArray[51];
            document.getElementById('content13').innerHTML = tmpArray[52];
            document.getElementById('content14').innerHTML = tmpArray[53];
            document.getElementById('content15').innerHTML = tmpArray[54];
            document.getElementById('content16').innerHTML = tmpArray[55];

            document.getElementById('content17').innerHTML = tmpArray[56];
            document.getElementById('content18').innerHTML = tmpArray[57];
            document.getElementById('content19').innerHTML = tmpArray[58];
            document.getElementById('content20').innerHTML = tmpArray[59];
            document.getElementById('content21').innerHTML = tmpArray[60];
            document.getElementById('content22').innerHTML = tmpArray[61];
            document.getElementById('content23').innerHTML = tmpArray[62];
            document.getElementById('content24').innerHTML = tmpArray[63];

            document.getElementById('content25').innerHTML = tmpArray[64];
            document.getElementById('content26').innerHTML = tmpArray[65];
            document.getElementById('content27').innerHTML = tmpArray[66];
            document.getElementById('content28').innerHTML = tmpArray[67];
            document.getElementById('content29').innerHTML = tmpArray[68];
            document.getElementById('content30').innerHTML = tmpArray[69];
            document.getElementById('content31').innerHTML = tmpArray[70];
            document.getElementById('content32').innerHTML = tmpArray[71];

            document.getElementById('content33').innerHTML = tmpArray[72];
            document.getElementById('content34').innerHTML = tmpArray[73];
            document.getElementById('content35').innerHTML = tmpArray[74];
            document.getElementById('content36').innerHTML = tmpArray[75];
            document.getElementById('content37').innerHTML = tmpArray[76];
            document.getElementById('content38').innerHTML = tmpArray[77];
            document.getElementById('content39').innerHTML = tmpArray[78];
            document.getElementById('content40').innerHTML = tmpArray[79];

          }
        }
      }
      ajaxRequest.send();
    }
 
    var myVar1 = setInterval(updateTagList, 10000);  
    var myVar2 = setInterval(updateTime, 1000);  
 
  </script>

    <title>Kuhner RFID Controller</title>
 </head>
 
 <body>
   <div id='main'>
     <h2>Kuhner RFID Controller</h2>
     <h3>Current Tag List</h3>
      <p id='P_time'>Time</p>
     <div id='content'> 
      <table style="height:1000px; width: 100%; font-size: 14px; border-collapse: collapse; margin-left; auto; margin-right: auto;" border="1"><tbody>
        <tr>  <th> Position </th> <th> UID </th> <th> Tag Content</th> </tr>
        <tr style="height: 25px;">
          <th> 1 </th>
          <td id ='uid1'> 0x </td>
          <td id ='content1'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 2 </th>
          <td id ='uid2'> 0x </td>
          <td id ='content2'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 3 </th>
          <td id ='uid3'> 0x </td>
          <td id ='content3'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 4 </th>
          <td id ='uid4'> 0x </td>
          <td id ='content4'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 5 </th>
          <td id ='uid5'> 0x </td>
          <td id ='content5'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 6 </th>
          <td id ='uid6'> 0x </td>
          <td id ='content6'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 7 </th>
          <td id ='uid7'> 0x </td>
          <td id ='content7'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 8 </th>
          <td id ='uid8'> 0x </td>
          <td id ='content8'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 9 </th>
          <td id ='uid9'> 0x </td>
          <td id ='content9'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 10 </th>
          <td id ='uid10'> 0x </td>
          <td id ='content10'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 11 </th>
          <td id ='uid11'> 0x </td>
          <td id ='content11'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 12 </th>
          <td id ='uid12'> 0x </td>
          <td id ='content12'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 13 </th>
          <td id ='uid13'> 0x </td>
          <td id ='content13'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 14 </th>
          <td id ='uid14'> 0x </td>
          <td id ='content14'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 15 </th>
          <td id ='uid15'> 0x </td>
          <td id ='content15'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 16 </th>
          <td id ='uid16'> 0x </td>
          <td id ='content16'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 17 </th>
          <td id ='uid17'> 0x </td>
          <td id ='content17'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 18 </th>
          <td id ='uid18'> 0x </td>
          <td id ='content18'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 19 </th>
          <td id ='uid19'> 0x </td>
          <td id ='content19'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 20 </th>
          <td id ='uid20'> 0x </td>
          <td id ='content20'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 21 </th>
          <td id ='uid21'> 0x </td>
          <td id ='content21'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 22 </th>
          <td id ='uid22'> 0x </td>
          <td id ='content22'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 23 </th>
          <td id ='uid23'> 0x </td>
          <td id ='content23'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 24 </th>
          <td id ='uid24'> 0x </td>
          <td id ='content24'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 25 </th>
          <td id ='uid25'> 0x </td>
          <td id ='content25'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 26 </th>
          <td id ='uid26'> 0x </td>
          <td id ='content26'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 27 </th>
          <td id ='uid27'> 0x </td>
          <td id ='content27'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 28 </th>
          <td id ='uid28'> 0x </td>
          <td id ='content28'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 29 </th>
          <td id ='uid29'> 0x </td>
          <td id ='content29'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 30 </th>
          <td id ='uid30'> 0x </td>
          <td id ='content30'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 31 </th>
          <td id ='uid31'> 0x </td>
          <td id ='content31'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 32 </th>
          <td id ='uid32'> 0x </td>
          <td id ='content32'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 33 </th>
          <td id ='uid33'> 0x </td>
          <td id ='content33'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 34 </th>
          <td id ='uid34'> 0x </td>
          <td id ='content34'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 35 </th>
          <td id ='uid35'> 0x </td>
          <td id ='content35'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 36 </th>
          <td id ='uid36'> 0x </td>
          <td id ='content36'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 37 </th>
          <td id ='uid37'> 0x </td>
          <td id ='content37'> 00 </td>
        </tr> 
        <tr style="height: 25px;">
          <th> 38 </th>
          <td id ='uid38'> 0x </td>
          <td id ='content38'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 39 </th>
          <td id ='uid39'> 0x </td>
          <td id ='content39'> 00 </td>
        </tr>
        <tr style="height: 25px;">
          <th> 40 </th>
          <td id ='uid40'> 0x </td>
          <td id ='content40'> 00 </td>
        </tr>   
      </table>
     </div>
  <div id='commands'> 
  <p>
    <a href="http://192.168.1.81/home">Home page</a><br />
  </p>
   </div> 
 </body>
</html>
  )=====";
/*
  <p><span style="background-color: lightgreen;">Basket Present</span><br />
  <span style="background-color: orange;">Reader Present</span><br />
  <span style="background-color: red;">No Reader Detected</span><br /></p>
  <p><a href="http://" + StringIP + "/showTagList">showTagList</a></p>
          */
