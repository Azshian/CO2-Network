
#include <SPI.h>          //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h>     //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h>         //nRF2401 libarary found at https://github.com/tmrh20/RF24/

#define LED_PIN      7
#define SIGNAL_PIN   6
#define RESET_BUTTON 5

const uint8_t pinCE   = 8;                  //This pin is used to set the nRF24 to standby (0) or active mode (1)
const uint8_t pinCSN  = 10;                 //This pin is used for SPI comm chip select
RF24 wirelessSPI(pinCE, pinCSN);            // Declare object from nRF24 library (Create your wireless SPI) 
const uint64_t rAddress = 0xB00B1E50C3LL;   //Create pipe address for the network the "LL" is for LongLong type
const uint8_t rFChan = 89;                  //Set channel frequency default (chan 84 is 2.484GHz to 2.489GHz)

float counter_6 = 0;
float total_6 = 0;
float counter_7 = 0;
float total_7 = 0;
float counter_9 = 0;
float total_9 = 0;

//Create a structure to hold fake sensor data and channel data
struct PayLoad {
  uint8_t chan;
  uint8_t sensor;
  unsigned long Time;
};

PayLoad payload; //create struct object

void setup() {
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  wirelessSPI.begin();                      //Start the nRF24 module
  wirelessSPI.setChannel(rFChan);           //set communication frequency channel
  wirelessSPI.openReadingPipe(1,rAddress);  //This is receiver or master so we need to be ready to read data from transmitters
  wirelessSPI.startListening();             // Start listening for messages
  Serial.begin(9600);                       //serial port to display received data
  Serial.println("Network master is online...");
}

void loop() {
  if (digitalRead(RESET_BUTTON) == LOW) {
    counter_6 = 0;
    total_6   = 0;
    counter_7 = 0;
    total_7   = 0;
    counter_9 = 0;
    total_9   = 0;
  }
  
  
  
  if(wirelessSPI.available()){ //Check if recieved data
     wirelessSPI.read(&payload, sizeof(payload)); //read packet of data and store it in struct object
     Serial.print("Received data packet from node: ");
     Serial.println(payload.chan); //print node number or channel
     Serial.print("Node sensor value is: ");
     Serial.println(payload.sensor); //print node's sensor value
     Serial.print("Node time is: ");
     Serial.println(payload.Time/1000);
     Serial.println(); 
     
     digitalWrite(SIGNAL_PIN,HIGH);
     delay(50);
     digitalWrite(SIGNAL_PIN, LOW);    
     delay(50);
     if (payload.chan == 6)
       counter_6 ++;
     if (payload.chan == 7)
       counter_7 ++;
     if (payload.chan == 9)
       counter_9 ++;
   
  }

  else {
  Serial.println("None");
  delay(100);
  }


  total_6++;
  Serial.print(counter_6); Serial.print(" ");
  Serial.print(total_6); Serial.print(" ");
  Serial.println(counter_6/total_6*100);

  total_7++;
  Serial.print(counter_7); Serial.print(" ");
  Serial.print(total_7); Serial.print(" ");
  Serial.println(counter_7/total_7*100);

  total_9++;
  Serial.print(counter_9); Serial.print(" ");
  Serial.print(total_9); Serial.print(" ");
  Serial.println(counter_9/total_9*100);
  delay(100);
}
