#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2561_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi and server settings
const char* ssid = "XXXXXXXX";
const char* password = "XXXXXXXX";
const char* serverUrl = "http://192.168.1.88/Cathy/post-data.php";

const int LED = D4; // GPIO2
const int ERROR_BLINK_INTERVAL = 200; // Blink interval for error (milliseconds)

const int MQ135_PIN = A0;

// BME280 and TSL2561 instances
Adafruit_BME280 bme;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

int readMQ135() {
  int sensorValue = analogRead(MQ135_PIN);
  return sensorValue;
}

// Function to send data to the LAMP server
void sendData(float temperature, float humidity, float pressure, uint16_t lux, int mq135Value) {
  digitalWrite(LED, LOW); // Turn LED ON before sending data

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverUrl);
    
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "temperature=" + String(temperature) + "&humidity=" + String(humidity) + "&pressure=" + String(pressure) + "&lux=" + String(lux) + "&mq135=" + String(mq135Value);
    int httpCode = http.POST(postData);

    if (httpCode > 0) {
      Serial.printf("Sent data to server. HTTP response: %d\n", httpCode);
    } else {
      Serial.printf("Failed to send data. Error: %s\n", http.errorToString(httpCode).c_str());
      blinkError(); // Blink LED for server communication error
    }

    http.end();
  } else {
    blinkError(); // Blink LED for WiFi connection error
  }

  delay(5000);
  digitalWrite(LED, HIGH); // Turn LED OFF after sending data
}

// Add a new function to blink the LED for error
void blinkError() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED, LOW); // LED ON
    delay(ERROR_BLINK_INTERVAL);
    digitalWrite(LED, HIGH); // LED OFF
    delay(ERROR_BLINK_INTERVAL);
  }
}

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      digitalWrite(LED, LOW); // LED ON
      delay(100);
      Serial.print(".");
      digitalWrite(LED, HIGH); // LED OFF
      delay(100);
    }
    Serial.println("\nConnected to WiFi");
  }
}


// Update the setup() function
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH); // Turn LED OFF initially

  // Initialize WiFi
  connectToWiFi();

  // Initialize BME280
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    blinkError(); // Blink LED for sensor error
    while (1);
  }

  // Initialize TSL2561
  if (!tsl.begin()) {
    Serial.println("Could not find a valid TSL2561 sensor, check wiring!");
    blinkError(); // Blink LED for sensor error
    while (1);
  }
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
}

void loop() {
  // Reconnect to WiFi if necessary
  connectToWiFi();

  // Read BME280 sensor data
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;

  // Read TSL2561 sensor data
  sensors_event_t event;
  tsl.getEvent(&event);
  uint16_t lux = event.light;

  // Read MQ-135 sensor data
  int mq135Value = readMQ135();

  // Print sensor data
  Serial.printf("Temperature: %.2f C, Humidity: %.2f %%, Pressure: %.2f hPa, Lux: %d, MQ-135: %d\n", temperature, humidity, pressure, lux, mq135Value);

  // Send sensor data to the LAMP server
  sendData(temperature, humidity, pressure, lux, mq135Value);

  // Enter deep sleep mode for 60 seconds (1 minute)
  ESP.deepSleep(60 * 1000000);


  delay(100);
}
