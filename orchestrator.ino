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
  // Wait for a new client:
  EthernetClient client = server.available();
  int payloadlen = 0;
  byte plbuffert[8];
  int nextbyte = 0;//client.read();
  int rcvdbytes = 1;
  
  while (client) 
  {
    // Check if data is available    
    if(server.available())
    {   
      client.println("LOL");
      while(!Serial);
      Serial.println("Waiting for answer");
      while(!server.available());
      Serial.println(client.read());
    }/*
    while(nextbyte != -1) {
      Serial.println(nextbyte);
      if(rcvdbytes <= 1){
        payloadlen = nextbyte;
        Serial.println((byte)payloadlen);
      } 
      else if(rcvdbytes != (payloadlen + 1)){
        Serial.println((byte)nextbyte);
        //plbuffert[(rcvdbytes - 4) % 8)] = nextbyte;
        if(!((rcvdbytes - 4) % 8)){
          //sendCanAndEmptyBuffert(plbuffert);
        }
      }
      else {
        break;
      }
      
      //read next byte
      nextbyte = client.read();
      rcvdbytes += 1;
    }
    if(rcvdbytes > 1){
      client.write(nextbyte);
      Serial.println("RCVDBYTES");
      
    }*/
  }

  /*    
  if(debug):
    Serial.print("PayloadLen: ");
    Serial.print(payloadlen);
    Serial.println();
    Serial.println("Payload:");
    Serial.println(payload);
  */
  while(client.connected())
  {
    client.flush();
    client.stop();
  }
  
}
