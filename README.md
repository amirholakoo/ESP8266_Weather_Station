# Weather and Air Quality Monitoring Station
This project is a weather and air quality monitoring station using an ESP8266, BME280, TSL2561, and MQ-135 sensor. The data collected from the sensors is sent to a LAMP server for storage and analysis. The creator of this project is not a programmer but an enthusiast who built this with the help of an OpenAI. 

# Features
Measures temperature, humidity, air pressure, light intensity, and air quality
Connects to Wi-Fi to send data to a LAMP server
Error indication using LED blinking
ESP8266 enters deep sleep mode to save power
Easy configuration of Wi-Fi and server settings

# Hardware Setup

# Components
- ESP8266
- BME280 sensor
- TSL2561 sensor
- MQ-135 sensor
- LED
- Breadboard
- Jumper wires
- Pin Connections

# BME280:

- VCC -> 3.3V
- GND -> GND
- SDA -> D2 (GPIO4)
- SCL -> D1 (GPIO5)

# TSL2561:

- VIN -> 3.3V
- GND -> GND
- SDA -> D2 (GPIO4)
- SCL -> D1 (GPIO5)

# MQ-135:

- VCC -> 3.3V
- GND -> GND
- AOUT -> A0

# LED:
We are using on board LED. Please don't use D4 since it's reserved for our LED. Skip LED pins.
- Anode (+) -> D4 (GPIO2)
- Cathode (-) -> GND

# Software Setup
1. Install the following libraries in the Arduino IDE:

- Adafruit_Sensor
- Adafruit_BME280
- Adafruit_TSL2561
- ESP8266WiFi
- ESP8266HTTPClient

2. Replace the XXXXXXXX placeholders in the ssid and password variables with your Wi-Fi network's SSID and password.

3. Update the serverUrl variable with the URL of your LAMP server and the path to the post-data.php file.

4. Upload the code to the ESP8266.

# Usage
1. Power on the ESP8266.
2. The device will connect to the Wi-Fi network and start collecting sensor data.
3. The sensor data will be sent to the LAMP server for storage and analysis.
4. The LED will indicate the status of the device:
- Solid ON: Sending data to the server
- Solid OFF: In deep sleep mode
- Blinking: Error occurred (e.g., Wi-Fi connection, server communication, or sensor initialization)

# Tweaks and Improvements
- Adjust the deep sleep time to change the frequency of data collection and transmission.
- Customize the error blink interval by modifying the ERROR_BLINK_INTERVAL constant.
- Add additional error handling or reconnection attempts if needed.

# 
This project is designed for beginners and enthusiasts alike, and the instructions provided should make it easy to set up and use. If you have any suggestions or improvements, feel free to contribute to the project. Enjoy your new weather and air quality monitoring station!
