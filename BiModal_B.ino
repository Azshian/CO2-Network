#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LED_SIGNAL_PIN 2
#define BUTTON_SIGNAL_PIN 4
#define LED_MODE_PIN 3
#define BUTTON_MODE_PIN 5

RF24 radio(8, 9);
const byte addresses[][6] = {"00435", "00022"};

bool TransmitMode = true;
bool ModePressed  = false;
int Signal_1 = 0;
int Signal_2 = 0;

float total   = 0;
float counter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_SIGNAL_PIN, OUTPUT);
  pinMode(LED_MODE_PIN, OUTPUT);
  pinMode(BUTTON_SIGNAL_PIN, INPUT_PULLUP);
  pinMode(BUTTON_MODE_PIN, INPUT_PULLUP);
  digitalWrite(LED_SIGNAL_PIN, LOW);
  digitalWrite(LED_MODE_PIN, LOW);
  
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
}

void loop() {
  if (TransmitMode == true) {
    //Transmitter Mode
    total = 0;
    counter = 0;
    
    digitalWrite(LED_MODE_PIN, HIGH);
    radio.stopListening();
    
    if (digitalRead(BUTTON_SIGNAL_PIN) == LOW)
      Signal_1 = 1;
    else
      Signal_1 = 0;
    
    digitalWrite(LED_SIGNAL_PIN, HIGH);
    radio.write(&Signal_1, sizeof(Signal_1));
    delay(100);
    digitalWrite(LED_SIGNAL_PIN, LOW);
    delay(100);
    
    Serial.print("Sending:"); Serial.println(Signal_1);
    
    if (digitalRead(BUTTON_MODE_PIN) == LOW) {
      TransmitMode = false;
      delay(500);
    }
  }
    
  else if (TransmitMode == false) {
    //Receiver Mode
    total ++;
    digitalWrite(LED_MODE_PIN, LOW);
    radio.startListening();
    
    if (radio.available()) {
      counter ++;
      radio.read(&Signal_2, sizeof(Signal_2));
      Serial.println(Signal_2);
   
      if (Signal_2 == 1){
        digitalWrite(LED_SIGNAL_PIN, HIGH);
        delay(100);
        digitalWrite(LED_SIGNAL_PIN, LOW);
        delay(100);
      }
      else 
        delay(200);
    }
    
 
    else {
    //Serial.println("Nothing");
    delay(200);
    }
    Serial.print(counter); Serial.print(" ");
    Serial.print(total); Serial.print(" ");
    Serial.println(counter/total*100);
    
    if (digitalRead(BUTTON_MODE_PIN) == LOW) {
      TransmitMode = true;
      delay(500);
    }
  }
  //Serial.println(TransmitMode);
  //delay(100);
}
