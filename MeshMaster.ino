 
 
 /** RF24Mesh_Example_Master.ino by TMRh20
  * 
  *
  * This example sketch shows how to manually configure a node via RF24Mesh as a master node, which
  * will receive all data from sensor nodes.
  *
  * The nodes can change physical or logical position in the network, and reconnect through different
  * routing nodes as required. The master node manages the address assignments for the individual nodes
  * in a manner similar to DHCP.
  *
  */
  
  
#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>


//#define LED_PIN      7
#define SIGNAL_PIN   6
//#define RESET_BUTTON 5

/***** Configure the chosen CE,CS pins *****/
RF24 radio(8,10);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

uint32_t displayTimer = 0;
struct Data {
  unsigned long theTime;
  uint8_t id;
  unsigned long randNumber;
};
Data data;

void setup() {
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  mesh.begin();



}


void loop() {    

  // Call mesh.update to keep the network updated
  mesh.update();
  
  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();
  
  
  // Check for incoming data from the sensors
  if(network.available()){
    digitalWrite(SIGNAL_PIN,HIGH);
    delay(50);
    digitalWrite(SIGNAL_PIN, LOW);    
    delay(50);
    RF24NetworkHeader header;
    network.peek(header);
    switch(header.type){
      case 'M': 
        network.read(header,&data,sizeof(data)); 
        Serial.print(data.theTime); Serial.print(" ");
        Serial.print(data.id); Serial.print(" "); 
        Serial.println(data.randNumber); 
        break;
      default: network.read(header,0,0); 
        Serial.print("default");Serial.println(header.type);
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
