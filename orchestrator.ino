#include "orchestrator.h"

bool sanitizepayload(int nextbyte){
  //check if number, uppercase or lowercase
  return ((48 <= nextbyte & nextbyte <= 57 ) | (65 <= nextbyte & nextbyte <= 90) | (97 <= nextbyte & nextbyte <= 122));
}

void handlepayload(byte *payload, int payload_len){  
  //String function = "";
  char canbuffer[8];
  bool canrcv = false;
  bool cansnd = true;
  bool transmitting = true;

  //start test on MT
  connectToMT();
  /*
  for(int i = 0; i < payload_len; i++){
    function += (char)payload[i];
  }
  */
  //execMTFunctionCall(function, strlen(function));
  //start test on module
  payload[payload_len + 1] = EOF;
  sndCan(msg, 7, 0x1);
  uint8_t buffert[15];
  int buf_pos = 0;
  while(transmitting){
    while(!(len = rcvCan()));
    for(int i = 0; i < len; i++){
      if(rxBuf[i] == EOT){
        Serial.println("EOT");
        break;
      } else if(rxBuf[i] == WFA){
        Serial.println("WFA");
        if(buf_pos) clientMT.write(&buffert[0], (size_t)(buf_pos));
        int timeout_c = 0;
        while(!clientMT.available()){
          delay(10);
          timeout_c++;
          if(timeout_c > 20) break;
        };
        memset(buffert, 0, 30);
        buf_pos = 0;
        byte rcvdMT;
        Serial.print("WFA okso: ");
        while(clientMT.available()){
          buffert[buf_pos++] = clientMT.read();
        }
        sndCan(buffert, ++buf_pos, 1);
        memset(buffert, 0, 30);
        buf_pos = 0;
        Serial.println();
        break;
      } else if(rxBuf[i] == SOR){
        Serial.println("SOR");
        transmitting = false;
        break;
      } else if(rxBuf[i] < 200){
        buffert[buf_pos++] = rxBuf[i];
        if(sanitizepayload(rxBuf[i])){
          Serial.print("MT(char): ");
          Serial.println((char)rxBuf[i]);
        } else{
          Serial.print("MT: ");
          Serial.println(rxBuf[i]);
        }
      }
    }
  }
  
  execMTFunctionCall("quit", sizeof("quit"));
  bool resultCollecting = true;
  
  while(resultCollecting){
    while(!(len = rcvCan()));
    for(int i = 0; i < len; i++){
      if(rxBuf[i] == EOR){
        resultCollecting = false;
        break;
      }
      clientAdmin.write(rxBuf[i]);
      if(sanitizepayload(rxBuf[i]) | (rxBuf[i] > 31 && rxBuf[i] < 200)){
        Serial.print((char)rxBuf[i]);  
      } else if(rxBuf[i] < 200){
        Serial.print(rxBuf[i]);  
      }
      
    }
  }
  
  while(clientAdmin.connected()){
    clientAdmin.flush();
    clientAdmin.stop();
  }
}

bool connectToMT(){
  if (clientMT.connect(serverMT, 6340)) {
    clientMT.write(1);
    while(!clientMT.available()){}
    if(clientMT.read() == 1){
      Serial.println("connected to server");
      Serial.print("clearing buffert: ");
      while(clientMT.available()){
        Serial.print(clientMT.read());
      }
      Serial.println();
      return true;
    }
  }
  return false;
}

void disconnectFromMT(){
    
}

bool execMTFunctionCall(char *functionCall, size_t len){
    len -= 1;
    clientMT.write(len);
    
    if(debug)
      Serial.print("Functioncall size of  ");
      Serial.print(functionCall);
      Serial.print(": ");
      Serial.print(len);
      Serial.println();
      
    clientMT.print(functionCall);
    while(!clientMT.available()){}
    if(clientMT.read() == len){        
      Serial.println("Successfull functionCall");
      return true;
    }
    return false;
}

void loop() {
  while(!Serial);
  delay(3000);
  /*
  while(true){
    delay(10000);
    while(!(len = rcvCan()));
    Serial.println("Rcvd CAN WOOHOO");
    for(int i = 0; i < len; i++){
      Serial.print((int)msgString[i]);
    }
    Serial.println();
  }
  */
  Serial.println("Waiting for answer");
  EthernetClient clientAdmin;
  handlepayload(rcvdpayload, rcvdpayloadlen);
  while(true){};
  /*
  sndCan(msg, 18, 1);
  while(!(len = rcvCan()));
  */
  
  //waiting for someone to send
  /*
  while(!clientAdmin.connected()){
    clientAdmin = server.available();
  }
  while(!clientAdmin.available() & clientAdmin.connected());
  if(!clientAdmin.connected()) return;
  
  nextbyte = clientAdmin.read();
  Serial.println(nextbyte);
  //clientAdmin.write(nextbyte);
  
  if(debug)
    Serial.print("Nextbyte: ");
    Serial.print(nextbyte);
    Serial.println();
    
  if(sanitizepayload(nextbyte)){
    rcvdpayload[rcvdpayloadlen] = nextbyte;    
    rcvdpayloadlen += 1;
  }
  else if(nextbyte == 4){
    Serial.print("END OF TRANSMISSION");
    Serial.print("Payload length: ");
    Serial.print(rcvdpayloadlen);
    Serial.print(", PAYLOAD: ");
    
    for(int i = 0; i < rcvdpayloadlen; i++){
      Serial.print(rcvdpayload[i]);
    }
    Serial.println();
    handlepayload(rcvdpayload, rcvdpayloadlen);
    rcvdpayloadlen = 0;
  } else{
    Serial.println("FAULTY PAYLOAD, DISCONNECTING...");
    //negative transmission code sent, faulty input
    clientAdmin.write(21);
    while(clientAdmin.connected()){
      clientAdmin.flush();
      clientAdmin.stop();
    }
  }
  */

}

void setup() {
  // initialize the ethernet device
  Ethernet.begin(mac, ip);
  Serial.begin(115200);  // CAN is running at 500,000BPS; 115,200BPS is SLOW, not FAST, thus 9600 is crippling.
  // start listening for clients
  server.begin();

  while(!Serial){};
  Serial.println(Ethernet.localIP());

  canInit();
}

//#################### CAN #################################################

int rcvCan(){
  if(!digitalRead(CAN0_INT)){                          // If CAN0_INT pin is low, read receive buffer
    CAN0.readMsgBuf(&rxId, &len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)             // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
    //Serial.print(len);
    
    if((rxId & 0x40000000) == 0x40000000){            // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      //Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        msgString[i] = rxBuf[i];
        //Serial.print("RCV CAN: ");
        //Serial.println(rxBuf[i]);
      }
    }    
    return len;
  }
  return 0;
}

bool sndCan(byte *msg, int msg_len, int dest_id){
  byte canbuffer[8];
  int i = 0;
  int q = dest_id;
  for(i = 0;i < msg_len; i++){
    if(!(i%8) && i > 0){
      if(CAN0.sendMsgBuf(q, 8, canbuffer) == CAN_OK)        
        //Serial.println("CAN - Message Sent Successfully!");
        Serial.println(" - Sent Can");
      else
        Serial.println("Error Sending CAN - Message...");
    }
    canbuffer[i%8] = msg[i];
    Serial.print(msg[i]);
  }
  
  canbuffer[msg_len % 8] = EOT;
  if(CAN0.sendMsgBuf(q, (msg_len)%8 + 1, canbuffer) == CAN_OK)
    Serial.println(" - Sent CAN");
    //Serial.println("CAN - Message Sent Successfully!");
  else
    Serial.println("Error Sending CAN - Message...");
}

void canInit(){
  while(!Serial){};
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  // Since we do not set NORMAL mode, we are in loopback mode by default.
  CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);                           // Configuring pin for /INT input
}

//#################### CAN #################################################
  /*    
  if(debug):
    Serial.print("PayloadLen: ");
    Serial.print(payloadlen);
    Serial.println();
    Serial.println("Payload:");
    Serial.println(payload);
  */  
