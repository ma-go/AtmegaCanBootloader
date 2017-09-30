// CAN Receive Example
//

#include "mcp_can.h"
#include <SPI.h>
#include <avr/pgmspace.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxHead[4];
unsigned char rxData[8];
char msgString[128];                        // Array to store serial string

MCP_CAN CAN0(4);                               // Set CS to pin 10
uint8_t whoIam;

void setup()
{
  Serial.begin(115200);
  delay(100);
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
  
  pinMode(0, OUTPUT);
  Serial.println("MCP2515 Library Receive Example...");

  uint16_t address = 0xfFFF;
  whoIam = pgm_read_byte_near(address);
  Serial.print(whoIam, HEX);
}

void(* resetFunc) (void) = 0x7C00; //declare reset function @ address 0

void loop()
{
  if(CAN0.checkReceive() == CAN_MSGAVAIL)                       
  {
    CAN0.readMsgBuf(&rxId, &len, rxData);  // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
    
    rxHead[0] = (rxId >> 24) & 0xFF;
    rxHead[1] = (rxId >> 16) & 0xFF;
    rxHead[2] = (rxId >> 8) & 0xFF;
    rxHead[3] = rxId & 0xFF;

   
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxData[i]);
        Serial.print(msgString);
      }
    }
    
    if (rxHead[3] == whoIam){
       resetFunc();  //call reset
    }
    Serial.println();
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
