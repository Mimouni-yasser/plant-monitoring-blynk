//include DHTesp library
#include <DHTesp.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


//define blynk configuration
#define BLYNK_TEMPLATE_ID           "TMPLg2HcwHTC"
#define BLYNK_DEVICE_NAME           "MyPlant"
#define BLYNK_AUTH_TOKEN            "OeIQHhU5LoeKsLhr3n3IE0em4SsVjIND"
#define BLYNK_PRINT Serial

//define sensor local pins
#define DHT11_PIN 33
#define SOIL_PIN 34
#define CO2_PIN 39
#define LIM_PIN 36

//define actuator local pins
#define PUMP_PIN 2
#define FAN_PIN 4
#define LED_PIN 5

//global configuration
#define LIM_threshold 200
#define CO2_threshold 1000
#define H_threshold 250
#define T_threshold 30



//define blynk pins
#define pump_blynk V5
#define fan_blynk V6
#define led_blynk V7
#define A_M_BLYNK V8

//sensors global variables
int co2; 
int soil; 
int h; 
int t;
int luminosity;
bool auto_mode;
DHTesp DHT;

//actuator global variables
bool pump;
int pump_timer = 0;
bool fan;
int fan_timer = 0;
bool led;
int led_timer = 0;


//read blynk values
BLYNK_WRITE(pump_blynk){
  pump = param.asInt();
}
BLYNK_WRITE(fan_blynk){
  fan = param.asInt();
}
BLYNK_WRITE(led_blynk){
  led = param.asInt();
}


void setup() {
  Serial.begin(115200);
  pinMode(SOIL_PIN,INPUT);
  pinMode(CO2_PIN,INPUT);
  pinMode(LIM_PIN,INPUT);    
  pinMode(DHT11_PIN,INPUT);
  pinMode(PUMP_PIN,OUTPUT);
  pinMode(FAN_PIN,OUTPUT);
  pinMode(LED_PIN,OUTPUT);

  //initialize DHT object
  DHT.setup(DHT11_PIN,DHTesp::DHT11);
  //setup blynk connection
  Blynk.begin(BLYNK_AUTH_TOKEN, "S10", "yasser1234");
  //set auto mode to false
  auto_mode = 0;

}


void loop() {
  Blynk.run();

  //non blocking delay
  if (millis() % 1000 == 0) {
    //read temperature
  t = DHT.getTemperature();
  //read humidity
  h = DHT.getHumidity();
  //read co2 sensor
  co2 = analogRead(CO2_PIN);
  //read soil sensor
  soil = analogRead(SOIL_PIN);
  //read luminosity sensor
  luminosity = analogRead(LIM_PIN);
  //send values to Blynk
  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);
  Blynk.virtualWrite(V0, co2);
  Blynk.virtualWrite(V1, soil);
  Blynk.virtualWrite(V4, luminosity);
  }

  if(auto_mode == 1){
    //check if pump should be on
      if(soil<1000){
        digitalWrite(PUMP_PIN, HIGH);
      }
      else{
        //turn off pump
        digitalWrite(PUMP_PIN, LOW);
      }
      //chekc if fan should be on
      if(co2>1000 || h>H_threshold || t>T_threshold){
        //turn on fan
        digitalWrite(FAN_PIN, HIGH);
      }
      else{
        //turn off fan
        digitalWrite(FAN_PIN, LOW);
      }
      //check if led should be on
      if(luminosity<LIM_threshold){
        //turn on led
        digitalWrite(LED_PIN, HIGH);
      }
      else{
        //turn off led
        digitalWrite(LED_PIN, LOW);
      }
  }
  else
  {
    //check if pump is on
    if(pump == 1){
      //turn on pump
      digitalWrite(PUMP_PIN, HIGH);
    }
    else{
      //turn off pump
      digitalWrite(PUMP_PIN, LOW);
    }


    //check if fan is on
    if(fan == 1){
      //turn on fan
      digitalWrite(FAN_PIN, HIGH);
    }
    else{
      //turn off fan
      digitalWrite(FAN_PIN, LOW);
    }
    //check if led is on
    if(led == 1){
      //turn on led
      digitalWrite(LED_PIN, HIGH);
    }
    else{
      //turn off led
      digitalWrite(LED_PIN, LOW);
    }
  }

}
