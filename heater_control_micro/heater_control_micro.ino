class ApplianceControl

{
  public:

    ApplianceControl();

    void on();
    void off();
    bool status();

  private:

    bool _status;

};



ApplianceControl::ApplianceControl()
{
  _status = 0;
}

void ApplianceControl::on()
{
  _status = 1;
}

void ApplianceControl::off()
{
  _status = 0;
}

bool ApplianceControl::status()
{
  return _status;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


#include <dht.h>
#include <VirtualWire.h>
#include "Timer.h"
#include <Rotary.h>

ApplianceControl heater;

dht DHT;
Timer t;
Rotary r = Rotary(8,9);

const int ledPin = 13;
const int transmit_en_pin = 3;
const int transmit_pin = 15;
#define DHT_PIN 10


float targetTemp = 22;

void setup() {
  

  

  
  
  
  
  
  
  Serial.begin(9600);
  Serial.println("setup");

  pinMode (ledPin, OUTPUT);
  Serial.println("ledpin set");
  digitalWrite(ledPin, LOW);
  Serial.println("ledpin set to off");
  int checkHeater = t.every(2000, heaterState);
  Serial.println("timer set");
 

  //-----------------------------------------------------------------------------------------------------
  //transmitter set up

  vw_set_tx_pin(transmit_pin);
  vw_set_ptt_pin(transmit_en_pin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);       // Bits per sec

  

  //-----------------------------------------------------------------------------------------------------
  //rotary setup may chance for different boards chech the example lib
  
  
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT4) | (1 << PCINT5);
  sei();

  delay (2000);  //wait for DHT to be ready
}




void loop() {
  
 t.update();

}


//-----------------------------------------------------------------------------------------------------
//-----FUNCTIONS--------
//-----------------------------------------------------------------------------------------------------



void heaterState() {


    Serial.print("heater");
  int chk = DHT.read22(DHT_PIN);

  Serial.println(heater.status());

  Serial.print("Checking temp : ");
  Serial.println(DHT.temperature);
  float check = DHT.temperature;


  if (check > targetTemp+0.2)
  {
    Serial.print("temp is more than ");
    Serial.println(targetTemp);
    heater.off();
    Serial.println("turning heater off");
    digitalWrite(ledPin, LOW);
  }


  if (check < targetTemp-0.2)
  {
    Serial.print("temp is lower than ");
    Serial.println(targetTemp);
    heater.on();
    Serial.println("turning heater on");
    digitalWrite(ledPin, HIGH);

  }


  heaterSend();

}



//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

void heaterSend() {
  Serial.println("sending");
  char hState[1] = {heater.status()};
  Serial.println(hState[1]);
  vw_send((uint8_t *)hState, 1);
  vw_wait_tx(); // Wait until the whole message is gone

}


//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------


ISR(PCINT0_vect) {
  unsigned char result = r.process();
  if (result == DIR_NONE) {
    // do nothing
  }
  else if (result == DIR_CW) {
    targetTemp = targetTemp + 0.05;
    Serial.println(targetTemp);
  }
  else if (result == DIR_CCW) {
    targetTemp = targetTemp - 0.05;
    Serial.println(targetTemp);
  }

}




