/*************************************************************
 * This is a sample code for our plant monitoring system
 * it is based on ESP32 and it uses Blynk and DHT11, mq135, soil moisture and luminosity sensors
 * it also uses a pump, fan and led as actuators
 * the future of the project is to have a fully modular functionality
 * so that the user can add or remove sensors and actuators as they desire
 * code by: Yasser Mimouni, Adam Bakadour, Samir Elkanouni
 *************************************************************/



//define blynk configuration
#define BLYNK_TEMPLATE_ID "TMPLg2HcwHTC"
#define BLYNK_DEVICE_NAME "MyPlant"
#define BLYNK_AUTH_TOKEN "OeIQHhU5LoeKsLhr3n3IE0em4SsVjIND"
#define BLYNK_PRINT Serial
//include DHTesp library
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <DHTesp.h>


char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "S10";
char pass[] = "yasser1234";



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
#define CO2_threshold 400
#define H_threshold 95
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

//this function is similar to read_hum but it is used for temperature
int read_temp(){
  int err = 0;
  float readings = 0;
  for(int i=0;i<10;i++)
  {
    float temp = DHT.getTemperature();
    if ( abs(temp - INT_MAX) < 100)
    {
      i--;
      err++;
      if(err == 10) return -1;
      continue;
    }
    else
    readings = readings + temp;
  }
  readings = readings/10;
  return readings;
}
//this function reads humidity from DHT11 sensor and verifies if it is valid
int read_hum(){
  int err = 0;
  float readings = 0;
  for(int i=0;i<10;i++)
  {
    float hum = DHT.getHumidity();
    if ( abs(hum - INT_MAX) < 100)
    {
      i--;
      err++;
      if(err == 10) return -1;
      continue;
    }
    else
    readings = readings + hum;
  }
  readings = readings/10;
  return readings;
}
//this funtion is similar to read_hum but it is used for co2 sensor
int read_co2(){
  int err = 0;
  float readings = 0;
  for(int i=0;i<10;i++)
  {
    float co2 = analogRead(CO2_PIN);
    if ( abs(co2) < 20)
    {
      i--;
      err++;
      if(err == 10) return -1;
      continue;
    }
    else
    readings = readings + co2;
  }
  readings = readings/10;
  return readings;
}

//this function is similar to read_hum but it is used for soil sensor
// int read_soil(){
//   int err = 0;
//   float readings = 0;
//   for(int i=0;i<10;i++)
//   {
//     float soil = analogRead(SOIL_PIN);
//     if ( abs(soil) < 20)
//     {
//       i--;
//       err++;
//       if(err == 10) return -1;
//       continue;
//     }
//     else
//     readings = readings + soil;
//   }
//   readings = readings/10;
//   return readings;
// }



void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(SOIL_PIN,INPUT);
  pinMode(CO2_PIN,INPUT);
  pinMode(LIM_PIN,INPUT);    
  pinMode(DHT11_PIN,INPUT);
  pinMode(PUMP_PIN,OUTPUT);
  pinMode(FAN_PIN,OUTPUT);
  pinMode(LED_PIN,OUTPUT);

  //initialize DHT object
  DHT.setup(DHT11_PIN,DHTesp::DHT11);
  //set auto mode to false
  auto_mode = 0;

}

BLYNK_WRITE(pump_blynk){
  pump = param.asInt();
}
BLYNK_WRITE(fan_blynk){
  fan = param.asInt();
}
BLYNK_WRITE(led_blynk){
  led = param.asInt();
}


void loop() {
  Blynk.run();
  //print debug values
  


  //non blocking delay
  if (millis() % 1000 == 0) {
  Serial.print("CO2: ");
  Serial.print(co2);
  Serial.print(" Soil: ");
  Serial.print(soil);
  Serial.print(" Humidity: ");
  Serial.print(h);
  Serial.print(" Temperature: ");
  Serial.print(t);
  Serial.print(" Luminosity: ");
  Serial.println(luminosity);
    //read temperature
  t = read_temp();
  //read humidity
  h = read_hum();
  //read co2 sensor
  co2 = read_co2();
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

  if(auto_mode == 1){ //auto mode
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
  else if(auto_mode == 0) //manual mode
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
