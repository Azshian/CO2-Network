#include <SPI.h>        //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h>   //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h>       //nRF2401 libarary found at https://github.com/tmrh20/RF24/

#define SIGNAL_PIN 3

const uint8_t pinCE  = 8;                   //This pin is used to set the nRF24 to standby (0) or active mode (1)
const uint8_t pinCSN = 10;                   //This pin is used to tell the nRF24 whether the SPI communication is a command
RF24 wirelessSPI(pinCE, pinCSN);            // Declare object from nRF24 library (Create your wireless SPI)
const uint64_t wAddress = 0xB00B1E50C3LL;   //Create pipe address to send data, the "LL" is for LongLong type

const uint8_t rFChan = 89;                  //Set channel default (chan 84 is 2.484GHz to 2.489GHz)
const uint8_t rDelay = 7;                   //this is based on 250us increments, 0 is 250us so 7 is 2 ms
const uint8_t rNum   = 5;                   //number of retries that will be attempted
const uint8_t chan1  = 6;                   //D2 pin for node channel check
const uint8_t chan2  = 7;                   //D3 pin for node channel check
const uint8_t chan3  = 9;                  //D4 pin for node channel check

//stuct of payload to send fake sensor data and node channel
struct PayLoad {
  uint8_t chan;
  uint8_t sensor;
  unsigned long Time;
};

PayLoad payload; //create struct object

void setup() {
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
  digitalWrite(SIGNAL_PIN, HIGH);
  pinMode(chan1, INPUT_PULLUP); //set channel select digital pins to input pullup
  pinMode(chan2, INPUT_PULLUP);
  pinMode(chan3, INPUT_PULLUP);
  
  wirelessSPI.begin();  //Start the nRF24 module
  wirelessSPI.setChannel(rFChan);
  wirelessSPI.setRetries(rDelay, rNum);   //if a transmit fails to reach receiver (no ack packet) then this sets retry attempts and delay between retries
  wirelessSPI.openWritingPipe(wAddress);  //open writing or transmit pipe
  wirelessSPI.stopListening();            //go into transmit mode
  randomSeed(analogRead(0));              //set random seed for fake sensor data
  setChannel();                           //checks current channel setting for transceiver
}

void loop() {
  digitalWrite(SIGNAL_PIN, HIGH);
  delay(100);
  digitalWrite(SIGNAL_PIN, LOW);
  delay(100);

  payload.Time   = millis();
  payload.sensor = random(0, 255); //get made up sensor value
  Serial.println(payload.Time/1000);
  //Serial.println(payload.sensor);
  
  if (!wirelessSPI.write(&payload, sizeof(payload))) { //send data and remember it will retry if it fails
    delay(random(5, 20)); //as another back up, delay for a random amount of time and try again
    if (!wirelessSPI.write(&payload, sizeof(payload))) {
      //set error flag if it fails again
    }
  }
}

//check for low digital pin to set node address
void setChannel() {
  if (!digitalRead(chan1)) payload.chan = 6;
  else if (!digitalRead(chan2)) payload.chan = 7;
  else if (!digitalRead(chan3)) payload.chan = 9;
  else payload.chan = 0;
}
