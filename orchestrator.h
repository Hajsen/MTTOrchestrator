#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>



#ifndef ORCHESTRATOR_H_
#define ORCHESTRATOR_H_


#include <SPI.h>

byte eof[1] = {255}; //end of function
byte eot[1] = {254}; //end of transmission
byte sor[1] = {253}; //start of result
byte eor[1] = {252}; //end of result
byte wfa[1] = {251}; //wait for answer

#define EOF 255 //end of function
#define EOT 254 //end of transmission
#define SOR 253 //start of result
#define EOR 252 //end of result
#define WFA 251 //wait for answer

bool debug = 1;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10,160,238,20);
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
bool resultCollecting;


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
//byte msg[] = {'t', 'e', 's', 't', 'D', 'I', 255};
uint8_t buffert[30];

int rcvCan();
bool sndCan(byte *msg, int msg_len, int dest_id);
void canInit();
void enableMTPower();

#endif
