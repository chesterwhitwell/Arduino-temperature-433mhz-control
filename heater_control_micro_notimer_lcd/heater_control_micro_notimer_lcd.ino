//this bit is pretty unnessessarry, it was me figuring out how objects work
//you're a cock
class ApplianceControl    //

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
//-----includes are for calling other librarys to use in the programme



#include <dht.h>  // this one is to make getting the the temprature easier
#include <VirtualWire.h>   // this one is for the rf 433mhz transmitter 

#include <Rotary.h>  // this one is for the rotary encoder


#include <SPI.h>              // these next 3 are needed for the LCD screed
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//--------now we make some instances 


Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);  //this makes the programme aware of an lcd screen connected to pins 7,6,5,4,3

ApplianceControl heater;   //this is the one i wrote, makes an object called heater

dht DHT;  // this makes the programe know there is a temprature/humidity sensor

Rotary r = Rotary(8,9); //this make rotary encoder on pins 8 and 9

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//-------these are constants ans variables that make it easier to read the programme

const int ledPin = 13;  
 
const int transmit_pin = 15;

const int backlightPin = 12;

#define DHT_PIN 10


//------inital targert temprature
float targetTemp = 22;


//------the tick sound 
unsigned long tick = 67000;


//------these make are for the timer
unsigned long interval=3000;

unsigned long heaterOnTime=1200000; //max time the heater is on if the temp is within the temp range

unsigned long previousMillis=0;

unsigned long bkLightpreviousMillis=0;


bool backlightStatus = false;

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//---------set up runs once on startup 

void setup() {
  

   display.begin(); //start the lcd screen
   display.setContrast(55); //obvious i hope
    
   display.display(); // show splashscreen
   delay(2000); //wait 2 seconds 
   display.clearDisplay();
  
  
  
  
  
  Serial.begin(9600); //start a serial connection helps for debugging displays stuff computer when connected
  Serial.println("setup");  // when you see these it's writing this to the computer for debugging

  pinMode (ledPin, OUTPUT);  //
  pinMode (backlightPin, OUTPUT);
  
  Serial.println("ledpin set");
  digitalWrite(ledPin, LOW);
  digitalWrite(backlightPin, LOW);
  Serial.println("ledpin set to off");
  
 

  //-----------------------------------------------------------------------------------------------------
  //transmitter set up

  vw_set_tx_pin(transmit_pin);
 // vw_set_ptt_pin(transmit_en_pin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);       // Bits per sec

  

  //-----------------------------------------------------------------------------------------------------
  //rotary setup may chance for different boards chech the example lib
  
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT4) | (1 << PCINT5);
  sei();

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
  delay (2000);  //waiting for DHT to be ready
}




//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//-------this is the main programme it loops endlessly

void loop() {
  
  
  lcdDisplay(); // this is a function that displays everything we want on the lcd the funtion is written after the main loop
  
 //----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//-------this bit is a timer. it checks the time using millis and if the interval has expired it does the 2 funtions inside

  unsigned long currentMillis = millis();
 
   // How much time has passed, accounting for rollover with subtraction!
   if ((unsigned long)(currentMillis - previousMillis) >= interval) {
      // It's time to do something!
     
      heaterState(); //check if the heater needs turning on off
      heaterSend(); //send instructions to the remote switch
     
      previousMillis = currentMillis;  //reset the timer
      
   }
   
    if (backlightStatus == false && (unsigned long)(currentMillis - bkLightpreviousMillis) >= interval) {
     
      backlightStatus = false;
      digitalWrite(backlightPin, LOW);
     
      bkLightpreviousMillis = currentMillis;  //reset the timer
      
   }
  
   
   
    

}


//-----------------------------------------------------------------------------------------------------
//-----FUNCTIONS--------
//-----------------------------------------------------------------------------------------------------

void lcdDisplay(){
  
  int chk = DHT.read22(DHT_PIN);  //need to do this to get the temp sensor to check the temprature
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("target temp");  //write some text to the lcd
  display.setCursor(0,12);
  display.println(targetTemp); //write the targetTemp variable to the screen
  
  if (heater.status()==true)   //if the heaters on do this stuff, just put an asterix on the screen
  {
  display.setCursor(40,12);
  display.println("*");
  }
  
  display.setCursor(5,24);  // this bit write the actual tep in big text to the lcd scree
  display.setTextSize(2);
  display.println(DHT.temperature); //this bit is the temprature
  display.display();
  display.clearDisplay();
  
  
  
}







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
//--------rotary encoder increases of decreases the target temp depending on the direction the knob is turned


ISR(PCINT0_vect) {
  unsigned char result = r.process();
  if (result == DIR_NONE) {
    // do nothing
  }
  else if (result == DIR_CW) {
    tick= tick + 100;
    if (tick>67300) {tick=67300;}
    
    tone(14, tick, 10);
    targetTemp = targetTemp + 0.05;
    backlightStatus = true;
      digitalWrite(backlightPin, HIGH);
    Serial.println(targetTemp);
  }
  else if (result == DIR_CCW) {
     tick= tick - 100;
     if (tick<66500) {tick=66500;}
    tone(14, tick, 10);
    targetTemp = targetTemp - 0.05;
    backlightStatus = true;
      digitalWrite(backlightPin, HIGH);
    Serial.println(targetTemp);
  }

}





