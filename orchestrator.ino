#include "orchestrator.h"

bool sanitizepayload(byte nextbyte){
  //check if number, uppercase or lowercase
  return ((48 <= nextbyte & nextbyte <= 57 ) | (65 <= nextbyte & nextbyte <= 90) | (97 <= nextbyte & nextbyte <= 122));
}
/*
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
  *//*
  //execMTFunctionCall(function, strlen(function));
  //start test on module
  payload[payload_len + 1] = EOF;
  sndCan(payload, payload_len + 1, 0x1);
  while(transmitting){
    while(!(len = rcvCan()));
    for(int i = 0; i < len; i++){
      if(rxBuf[i] == EOT){
        transmitting = false;
        break;
      } else if(rxBuf[i] == WFA){
        while(!clientMT.available()){};
        sndCan(clientMT.read(), 1, 1);
        break;
      } else if(rxBuf[i] == SOR){
        transmitting = false;
        break;
      } else{
        clientMT.write(rxBuf[i]);  
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
    }
  }
  
  while(clientAdmin.connected()){
    clientAdmin.flush();
    clientAdmin.stop();
  }
}
*/

void emptyRead(EthernetClient client){
  Serial.println("Emptying buffert!");
  while(client.read() != -1){}
}

bool connectToMT(){
  if (clientMT.connect(serverMT, 6340)) {
    clientMT.write(1);
    while(!clientMT.available()){}
    Serial.print("Connectioncheck returned: ");
    int c;
    c = clientMT.read();
    Serial.println(c);
    if((char)c == 1){
      Serial.println(F("C server"));
    }
  }
  return false;
}

bool execMTFunctionCall(char *functionCall, size_t len){
    len -= 1;
    clientMT.write(len);
    /*
    if(debug)
      Serial.println();
      Serial.print("Functioncall size of  ");
      Serial.print(functionCall);
      Serial.print(": ");
      Serial.print(len);
      Serial.println();
    */
    clientMT.print(functionCall);
    while(!clientMT.available()){}
    if((char)clientMT.read() == len){  
      return true;
    }
    return false;
}

void testDI(){
  //TESTDI START
  byte di_sw_on = "\45";
  byte di_24_fb = "\10";
  Serial.println("DI_SW_ON is HIGH");
  execMTFunctionCall("setDO", sizeof("setDO"));
  delay(200);
  clientMT.write((byte)0x2D);
  delay(200);
  clientMT.write((byte)0x01);
  while(!clientMT.available());
  Serial.print("DI_SW_ON state: ");
  byte lol;
  lol = clientMT.read();
  while(lol != 255){
    if(sanitizepayload(lol)){
      Serial.println((char)lol);   
    }
    lol = clientMT.read();
  }

  delay(300);
  execMTFunctionCall("readA", sizeof("readA"));
  delay(600);
  clientMT.write((byte)0xA);
  delay(300);
  while(!clientMT.available());
  Serial.print("DI_24_FB: ");
  lol = clientMT.read();
  while(lol != 255){
    if(sanitizepayload(lol)){
      Serial.print((char)lol);   
    }
    lol = clientMT.read();
  }
  /*
  Serial.println("Press enter to continue to DI_SW_ON is LOW");
  while(Serial.available() == 0){}
  Serial.read();
  execMTFunctionCall("setDO", sizeof("setDO"));
  clientMT.write(di_sw_on);
  clientMT.write("\0");
  while(!clientMT.available());
  Serial.println("DI_SW_ON status: ");
  Serial.println(clientMT.read());
  delay(300);
  execMTFunctionCall("readA", sizeof("readA"));
  delay(300);
  clientMT.write(di_24_fb);
  delay(300);
  while(!clientMT.available());
  Serial.println("DI_24_FB: ");
  Serial.println(clientMT.read());
  
  */
  //TESTDI END
}

void testDO(){
  char do_fb_1 = "\8";
  for(int i = 0; i < 8; i++){
    Serial.println();
    Serial.print(i);
    execMTFunctionCall("readA", sizeof("readA") );
    delay(400);
    clientMT.write(do_fb_1);
    while(!clientMT.available());
    Serial.print(F(" : "));
    Serial.print(clientMT.read());
  }  
  Serial.println();
}

void test20mAO(){
  #define DIO32 32
  #define DIO33 33
  #define DIO26 26
  #define DIO34 34
  const byte DIO_array[4] = {0x20, 0x21, 0x1A, 0x22};
  for(int i = 0;i < 50; i++){
    i = 3;
    //open current output
    execMTFunctionCall("setDO", sizeof("setDO"));
    delay(300);
    clientMT.write(DIO_array[i]);
    delay(300);
    clientMT.write(byte(0x01));
    while(!clientMT.available());
    Serial.print("Pin: ");
    Serial.print(DIO_array[i]);
    Serial.print(" status: ");
    byte lol;
    lol = clientMT.read();
    while(lol != 255){
      if(sanitizepayload(lol)){
        Serial.print((char)lol);   
      }
      lol = clientMT.read();
    }    
    Serial.println();
    delay(15000);
    //close the current output
    execMTFunctionCall("setDO", sizeof("setDO"));
    delay(300);
    clientMT.write(DIO_array[i]);
    delay(300);
    clientMT.write((byte)0x00);
    Serial.println();
    Serial.print("Pin: ");
    Serial.print(DIO_array[i]);
    Serial.print(" status: ");    
    while(!clientMT.available());
    lol = clientMT.read();
    while(lol != 255){
      if(sanitizepayload(lol)){
        Serial.print((char)lol);   
      }
      lol = clientMT.read();
    }
    Serial.println();
    delay(400);
    Serial.println("Openening next current output");
  }
}

void loop() {
  
  /*
  testDO();
  testDI();
  delay(300);
  while(Serial.available() == 0){}
  Serial.read();
  test20mAO();
  /*
  while(Serial.available() == 0){}
  Serial.read();
  clientMT.write("quit", sizeof("quit"));
  */
  execMTFunctionCall("quit", sizeof("quit"));
  while(true){};
}

void setup() {
  // initialize the ethernet device
  Ethernet.begin(mac, ip);
  Serial.begin(115200);  // CAN is running at 500,000BPS; 115,200BPS is SLOW, not FAST, thus 9600 is crippling.
  // start listening for clients
  //server.begin();

  while(!Serial){};
  Serial.println(Ethernet.localIP());
  //Serial.println(F("C MT"));
  //canInit();
  enableMTPower();
}

void enableMTPower(){
  
  connectToMT();
  Serial.println("IO_DCU_ON is HIGH");
  execMTFunctionCall("setDO", sizeof("setDO"));
  delay(200);
  clientMT.write((byte)0x15);
  delay(200);
  clientMT.write((byte)0x01);
  while(!clientMT.available());
  Serial.print("IO_DCU_ON state: ");
  byte lol;
  lol = clientMT.read();
  while(lol != 255){
    if(sanitizepayload(lol)){
      Serial.println((char)lol);   
    }
    lol = clientMT.read();
  }
}

//#################### CAN #################################################
/*
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
        Serial.println("CAN - Message Sent Successfully!");
      else
        Serial.println("Error Sending CAN - Message...");
      
      q++;
    }
    canbuffer[i%8] = msg[i];
  }
  
  canbuffer[msg_len % 8] = EOT;
  if(CAN0.sendMsgBuf(q, (msg_len)%8 + 1, canbuffer) == CAN_OK)
    Serial.println("CAN - Message Sent Successfully!");
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
*/

//#################### CAN #################################################
  /*    
  if(debug):
    Serial.print("PayloadLen: ");
    Serial.print(payloadlen);
    Serial.println();
    Serial.println("Payload:");
    Serial.println(payload);
  */  
  
