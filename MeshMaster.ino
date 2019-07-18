#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>

#define SIGNAL_PIN   6

/***** Configure the chosen CE,CS pins *****/
RF24 radio(8,10);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

uint32_t displayTimer = 0;

struct Data {
  uint8_t id;
  unsigned long theTime;
  float theValue;
};
Data data;

void setup() {
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
  mesh.setNodeID(0);  // Set the nodeID to 0 for the master node
  mesh.begin();  // Connect to the mesh
  //Serial.println(mesh.getNodeID());
}


void loop() {    

  // Call mesh.update to keep the network updated
  mesh.update();
  
  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();
  
  
  // Check for incoming data from the sensors
  if(network.available()){
    //digitalWrite(SIGNAL_PIN,HIGH);
    //delay(50);
    //digitalWrite(SIGNAL_PIN, LOW);    
    //delay(50);
    RF24NetworkHeader header;
    network.peek(header);
    switch(header.type){
      case 'M': 
        network.read(header,&data,sizeof(data));
        //Serial.print(header.from_node); Serial.print(" "); 
        //Serial.print(header.to_node); Serial.print(" ");
        //Serial.print(header.id); Serial.println("");
        Serial.print (long(data.id));
        Serial.print(","); 
        Serial.print(data.theTime/1000);
        Serial.print(",");
        Serial.println(data.theValue); 
        
        break;
      default: network.read(header,0,0); 
        //Serial.print("default");Serial.println(header.type);
        break;
    }
  }
  
  if(millis() - displayTimer > 5000){
    displayTimer = millis();
    Serial.println(" ");
    Serial.println(F("********Assigned Addresses********"));
     for(int i=0; i<mesh.addrListTop; i++){
       Serial.print("NodeID: ");
       Serial.print(mesh.addrList[i].nodeID);
       Serial.print(" RF24Network Address: 0");
       Serial.println(mesh.addrList[i].address,OCT);
     }
    Serial.println(F("**********************************"));
  }
}  
