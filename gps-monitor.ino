#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

static const int RXPin = D4, TXPin = D3;
static const uint32_t GPSBaud = 9600;

const char server[] = "tracker-api.nielsvinke.nl";
const char resource[] = "/TrackerData";
const int  port = 80;

const int capacity = JSON_OBJECT_SIZE(5);
StaticJsonDocument<capacity> trackerData;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

HTTPClient http;

void setup() {
  // Set console baud rate
  Serial.begin(115200);
  ss.begin(GPSBaud);

  ConnectToWifi();
}

void ConnectToWifi() {
  WiFi.begin("iPhone van Niels", "tracker0");   //WiFi connection
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    delay(500);
    Serial.println("Waiting for connection");
  }
}

void loop()
{
  if(WiFi.status()== WL_CONNECTED){
    while (ss.available() > 0){
      if (gps.encode(ss.read())){
        if(gps.location.isValid()){
          String contentType = "application/json";
          String postData = "{\"device_id\":\"JQ47\",\"lat\":"+String(gps.location.lat(), 8)+",\"lng\":"+String(gps.location.lng(), 8)+",\"alt\":"+gps.altitude.meters()+",\"speed\":"+gps.speed.kmph()+"}";
      
          http.begin("http://tracker-api.nielsvinke.nl/TrackerData");      //Specify request destination
          http.addHeader("Content-Type", contentType);  //Specify content-type header
           
          http.POST(postData);   //Send the request
          http.end();  //Close connection
          delay(1 * 30 * 1000);  //30 Seconds delay.
        }
      }
    }
  } else{  
    Serial.println("Error in WiFi connection");   
  }
}
