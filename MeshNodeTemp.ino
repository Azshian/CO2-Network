/** RF24Mesh_Example.ino by TMRh20

   This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
   master node.
   The nodes will refresh their network address as soon as a single write fails. This allows the
   nodes to change position in relation to each other and the master node.
*/


#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
//#include <printf.h>

#define TRANSMIT_PIN   2
#define RECEIVE_PIN    3

#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 4 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);



/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(8, 10);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**
   User Configuration: nodeID - A unique identifier for each radio. Allows addressing
   to change dynamically with physical changes to the mesh.

   In this example, configuration takes place below, prior to uploading the sketch to the device
   A unique value from 1-255 must be configured for each node.
   This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.

 **/
#define nodeID 2


uint32_t displayTimer = 0;

struct Data {
  uint8_t id;
  unsigned long theTime;
  float theValue;
};

Data data;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

void setup() {

  Serial.begin(9600);
  sensors.begin(); 
  pinMode(TRANSMIT_PIN, OUTPUT);
  pinMode(RECEIVE_PIN, OUTPUT);
  //printf_begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
  data.id = nodeID;
  randomSeed(analogRead(0));
}



void loop() {

  mesh.update();

  // Send to the master node every second
  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();

    data.theTime = displayTimer;
    sensors.requestTemperatures();
    //Serial.print(float(sensors.getTempCByIndex(0)));
    data.theValue = float(sensors.getTempCByIndex(0));
    //data.randNumber = random(300); 
    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&data, 'M', sizeof(data))) {

      // If a write fails, check connectivity to the mesh network
      if ( ! mesh.checkConnection() ) {
        //refresh the network address
        Serial.println("Renewing Address");
        if(!mesh.renewAddress()){
          //If address renewal fails, reconfigure the radio and restart the mesh
          //This allows recovery from most if not all radio errors
          mesh.begin();
        }
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      digitalWrite(TRANSMIT_PIN,HIGH);
      delay(50);
      digitalWrite(TRANSMIT_PIN, LOW);    
      delay(50);
      
      Serial.print("Send OK: "); Serial.println(data.theValue);
    
      Serial.print(data.id);
    }
  }
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
        Serial.print(header.from_node); Serial.print(" "); 
        Serial.print(header.to_node); Serial.print(" ");
        Serial.print(header.id); Serial.println("");
        Serial.print(data.id); Serial.print(" "); 
        Serial.print(data.theTime/1000); Serial.print(" ");
        Serial.println(data.theValue); 
        break;
      default: network.read(header,0,0); 
        Serial.print("default");Serial.println(header.type);
        break;
    }
  }
  

//  while (network.available()) {
//    digitalWrite(RECEIVE_PIN,HIGH);
//    delay(50);
//    digitalWrite(RECEIVE_PIN, LOW);    
//    delay(50);
//    RF24NetworkHeader header;
//    payload_t payload;
//    network.read(header, &payload, sizeof(payload));
//    Serial.print("Received packet #");
//    Serial.print(payload.counter);
//    Serial.print(" at ");
//    Serial.println(payload.ms);
//  }
  
}
