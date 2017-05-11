#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>



#ifndef ORCHESTRATOR_H_
#define ORCHESTRATOR_H_


#include <SPI.h>

#define EOF 3 //end of function
#define EOT 4 //end of transmission
#define SOR 5 //start of result
#define EOR 6 //end of result
#define WFA 7 //wait for answer

bool debug = 1;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
IPAddress serverMT(10,160,238,15); 
// Setup server with (port)
EthernetServer server(80);
EthernetClient clientAdmin;
EthernetClient clientMT;

//Ethernet
int rcvdpayloadlen = 0;
char rcvdpayload[255];
byte nextbyte;

boolean clientConnected = false;
int ConnectionCheck = 0;

bool sanitizepayload(int nextbyte);
void handlepayload(byte *payload, int payload_len);
bool connectToMT();
void disconnectFromMT();
bool execMTFunctionCall(char *functionCall, size_t len);


#include <mcp_can.h>
#include <mcp_can_dfs.h>

// CAN0 INT and CS
#define CAN0_INT 6                             // Set INT to pin 6

MCP_CAN CAN0(2);                               // Set CS to pin 12

// CAN RX Variables
long unsigned int rxId;
unsigned char len;
unsigned char rxBuf[8];

// Serial Output String Buffer
char msgString[128];
size_t msgString_len;
byte msg[] = {0x01, 0x06, 0x03, 0x04, 0x05};

int rcvCan();
bool sndCan(byte *msg, int msg_len, int dest_id);
void canInit();

#endif
