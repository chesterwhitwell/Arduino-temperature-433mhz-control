//you're not really a cock
#include <VirtualWire.h>

const int ledPin = 1;
const int receive_pin = 2;



unsigned long interval=60000;
unsigned long previousMillis=0;


void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  
 
 
  
  vw_set_rx_pin(receive_pin);
  vw_setup(2000);
  vw_rx_start();
}



void loop() {
  
  // put your main code here, to run repeatedly:
  
  unsigned long currentMillis = millis();
 
   // How much time has passed, accounting for rollover with subtraction!
   if ((unsigned long)(currentMillis - previousMillis) >= interval) {
      // It's time to do something!
      digitalWrite(ledPin, LOW); // Toggle the LED on Pin 13
 
      // Use the snapshot to set track time until next event
      previousMillis = currentMillis;
   }
  
  
  
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
  
  
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    int i;
    int checkState = 0;
   
    
    for (i = 0; i < buflen; i++)
    {
      int checkState = buf[i];
      

      if (checkState == 1)
      {
        previousMillis=0;
  
        digitalWrite(ledPin, HIGH);
        previousMillis = currentMillis;
        

      }
      else if (checkState == 0)
      {

        digitalWrite(ledPin, LOW);
      }

    }

  }

}
