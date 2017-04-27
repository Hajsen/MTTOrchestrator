#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet2.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>
#include <Twitter.h>
#include <util.h>

#include <SPI.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
// Setup server with (port)
EthernetServer server(80);

int rcvdpayloadlen = 0;
char rcvdpayload[255];
byte nextbyte;

void setup() {
  // initialize the ethernet device
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  // start listening for clients
  server.begin();

  while(!Serial){};
  Serial.println(Ethernet.localIP());
}

boolean clientConnected = false;
int ConnectionCheck = 0;

void loop() {
  while(!Serial);
  Serial.println("Waiting for answer");
  EthernetClient client;
  //waiting for someone to send
  while(!client.connected()){
    client = server.available();
  }
  while(!server.available() & client.connected());
  if(!client.connected()) return;
  
  nextbyte = client.read();
  Serial.println(nextbyte);
  client.write(nextbyte);
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
    //handlepayload(rcvdpayload, rcvdpayloadlen);
    rcvdpayloadlen = 0;
  } else{
    Serial.println("FAULTY PAYLOAD, DISCONNECTING...");
    //negative transmission code sent, faulty input
    client.write(21);
    while(client.connected()){
      client.flush();
      client.stop();
    }
  }

}

bool sanitizepayload(int nextbyte){
  //check if number, uppercase or lowercase
  return ((48 <= nextbyte & nextbyte <= 57 ) | (65 <= nextbyte & nextbyte <= 90) | (97 <= nextbyte & nextbyte <= 122));
}

void handlepayload(byte payload, int payload_len){
  #define EOT 4 //end of transmission
  #define SOR 5 //start of result
  #define EOR 6 //end of result
  
  String function = "";
  char canbuffer[8];
  bool canrcv = false;
  bool cansnd = true;
  
}

String rcvFunc(){
  while(cansnd){
    for(int i = 0; i < 8; i++){
      if(canbuffer[i] == EOF){
        execMTFunctionCall(function, strlen(function));
        function = "";
      }
    }
  }
}
  /*    
  if(debug):
    Serial.print("PayloadLen: ");
    Serial.print(payloadlen);
    Serial.println();
    Serial.println("Payload:");
    Serial.println(payload);
  */  
