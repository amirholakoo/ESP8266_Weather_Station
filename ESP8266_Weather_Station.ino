/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

*/

#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_MPL3115A2.h>

#include "Ai_AP3216_AmbientLightAndProximity.h"



int BoardLed = 16;
int AntenaLed = 2;

bool stat;
bool ambStat;
long alsValue = 0;


// Replace with your network credentials
const char* ssid     = "VPN1";
const char* password = "09126141426";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://192.168.1.88/Cathy/post-Cathy-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorName = "MULTI";
String sensorLocation = "TEST";
String BoardName = "Cathy";


#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;  // I2C
//Adafruit_BME280 bme(BME_CS);  // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);  // software SPI

Ai_AP3216_AmbientLightAndProximity aps = Ai_AP3216_AmbientLightAndProximity();

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

Adafruit_MPL3115A2 baro;

void setup() {
  
  pinMode(AntenaLed, OUTPUT);
  pinMode(BoardLed, OUTPUT);
  delay(100);


  
  digitalWrite(AntenaLed,HIGH);
  delay(100);  
  digitalWrite(BoardLed,LOW);
  delay(100);
  
  Serial.begin(115200);

  //////////////////////////////////////////////////////////////////      Wifi       /////////////////////////////////////
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
    
    flash_B();
    delay(100);
    digitalWrite(AntenaLed,HIGH);
    delay(100);
    
    
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  flash_A();
  delay(100);
  digitalWrite(AntenaLed,HIGH);
  delay(100);

//////////////////////////////////////////////////////////////////      AP 3216       /////////////////////////////////////

  aps.begin();
  
  aps.startAmbientLightAndProximitySensor ();
  ambLight ();
  delay (100);

  flash_B();
  delay(100);
  digitalWrite(AntenaLed,HIGH);
  delay(100);

  
//////////////////////////////////////////////////////////////////      TSL 2561       /////////////////////////////////////

  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");

    flash_B();
    delay(100);
    
    while(1);
  }
  
  Lux ();
  delay (100);
  
  flash_B();
  delay(100);
  digitalWrite(AntenaLed,HIGH);
  delay(100);

//////////////////////////////////////////////////////////////////      MPL 3115A2       /////////////////////////////////////


  if (!baro.begin()) {
    Serial.println("Could not find sensor. Check wiring.");
    flash_B();
    delay(100);
    while(1);
  }

  // use to set sea level pressure for current location
  // this is needed for accurate altitude measurement
  // STD SLP = 1013.26 hPa
  baro.setSeaPressure(1013.26);
  delay (100);
  
  alt();
  delay (100);
  
  flash_B();
  delay(100);
  digitalWrite(AntenaLed,HIGH);
  delay(100);
  

//////////////////////////////////////////////////////////////////      BME 280       /////////////////////////////////////

  // (you can also pass in a Wire library object like &Wire2)
  stat = bme.begin(0x76);
  if (!stat) {
    Serial.println("Could not find a valid BME280 sensor, check wiring or change I2C address!");
    
    flash_B();
    delay(100);
    
    while (1);
  }
  

  flash_B();
  delay(100);
  digitalWrite(BoardLed,LOW);
  delay(100);
  flash_A();
  delay(100);

}


//////////////////////////////////////////////////////////////////      void loop()     \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void loop() {

  // Update Sensors again:
   
  ambLight();
  delay (100);

  Lux();
  sensors_event_t event;
  tsl.getEvent(&event);
  float lux = event.light;
  String luxString = String (lux);
  delay (100);

  alt();
  delay(100);

  
  
  //Check connection status
  
  if(WiFi.status()== WL_CONNECTED && bme.begin(0x76) == 1){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = 
                            "api_key=" + apiKeyValue 
                          + "&board=" + BoardName 
                          + "&sensor=" + sensorName
                          + "&location=" + sensorLocation 
                          + "&value4=" + luxString 
                          + "&value5=" + String(aps.getAmbientLight())
                          + "&value1=" + String(bme.readTemperature())
                          + "&value2=" + String(bme.readHumidity()) 
                          + "&value3=" + String(bme.readPressure()/100.0F) 
                          + "&value6=" + String(baro.getPressure()) 
                          + "&value7=" + String(baro.getAltitude())
                          + "&value8=" + String(baro.getTemperature()) 
                          + "";
                          
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      
      flash_A();
      delay(100);
      digitalWrite(BoardLed,LOW);
      delay(100);
      
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);

      flash_B();
      delay(100);
      digitalWrite(AntenaLed,HIGH);
      delay(100);
      digitalWrite(BoardLed,LOW);
      delay(100);
    }



    
    // Free resources
    http.end();

    flash_A();
    delay(100);
    digitalWrite(BoardLed,LOW);
    delay(100);
  

  }
  else {
    Serial.println("WiFi Disconnected OR Sensor ERORR");

    digitalWrite(AntenaLed,HIGH);
    delay(100);

    flash_B();
    delay(100);
    digitalWrite(BoardLed,LOW);
    delay(100);
    
  }
  //Send an HTTP POST request every 60 seconds
  delay(60000);  
}


//////////////////////////////////////////////////////////////////      FUNCTIONS:       /////////////////////////////////////

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void alt() {
  float pressure = baro.getPressure();
  float altitude = baro.getAltitude();
  float temperature = baro.getTemperature();

  Serial.println("-----------------");
  Serial.print("pressure = "); Serial.print(pressure); Serial.println(" hPa");
  Serial.print("altitude = "); Serial.print(altitude); Serial.println(" m");
  Serial.print("temperature = "); Serial.print(temperature); Serial.println(" C");

  delay(200);
}

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void Lux() 
{  
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    Serial.print("lux: ");
    Serial.println(event.light);
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }

  delay(200);
}

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void ambLight() {
  alsValue = aps.getAmbientLight();
  long psValue = aps.getProximity();

  Serial.print("Ambient Light: ");
  Serial.println(alsValue);
  Serial.print("Proximity: ");
  Serial.println(psValue);

  delay(200);
}

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void flash_A()
{
  digitalWrite(AntenaLed,HIGH);
  delay(100);
  digitalWrite(AntenaLed,LOW);
  delay(100);
  digitalWrite(AntenaLed,HIGH);
  delay(100);
  digitalWrite(AntenaLed,LOW);
  delay(100);
  digitalWrite(AntenaLed,HIGH);
  delay(100);
  digitalWrite(AntenaLed,LOW);
  delay(100);
 }

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

 
void flash_B()
{
  digitalWrite(BoardLed,LOW);
  delay(100);
  digitalWrite(BoardLed,HIGH);
  delay(100);
  digitalWrite(BoardLed,LOW);
  delay(100);
  digitalWrite(BoardLed,HIGH);
  delay(100);
  digitalWrite(BoardLed,LOW);
  delay(100);
  digitalWrite(BoardLed,HIGH);
  delay(100);
 }

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
