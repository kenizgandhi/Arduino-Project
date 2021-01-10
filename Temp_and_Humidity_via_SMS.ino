// Include DHT library
#include "DHT.h"

// Include Adafruit Sensor Library
#include <Adafruit_Sensor.h>

//Include Software Serial library to communicate with GSM
#include <SoftwareSerial.h>

// Pin DHT is connected to
#define DHTPIN 2
  

#define DHTTYPE DHT11   // DHT 11 sensor used for temperature and humidity measurement


// Initialize DHT sensor for normal 16MHz Arduino
DHT dht(DHTPIN, DHTTYPE);

// Global varibales to store temperature and humidity
float tc; // temperature in celcius
float tf; // temperature in fahrenheit
float h; // humidity

// Configure software serial port
SoftwareSerial SIM900(7, 8);

// Variable to store incoming SMS characters
char incomingChar;

void setup() {
  dht.begin();
  
  Serial.begin(19200); 
  SIM900.begin(19200);

  // Time for your GSM shield log on to network
  delay(20000);
  Serial.print("SIM900 ready...");

  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);
  
  // Set module to send SMS data to serial out upon receipt 
  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);
}

void loop(){
  if (SMSRequest()){
    if(readData()){
      delay(10);
    
      SIM900.println("AT + CMGS = \"+4915218478335\"");
      delay(100);
      
      String dataMessage = ("Temperature: " + String(tc) + "*C " + " Humidity: " + String(h) + "%");
      
      // String dataMessage = ("Temperature: " + String(tf) + "*F " + " Humidity: " + String(h) + "%");      
      
      // Send the SMS text message
      SIM900.print(dataMessage);
      delay(100);
      
      // End AT command with a ^Z, ASCII code 26
      SIM900.println((char)26); 
      delay(100);
      SIM900.println();
      
      // Give module time to send SMS
      delay(5000);  
    }
  }
  delay(10); 
}

boolean readData() {
  //Read humidity
  h = dht.readHumidity();
  // Read temperature as Celsius
  tc = dht.readTemperature();
  // Read temperature as Fahrenheit
  tf = dht.readTemperature(true);

  // Compute temperature values in Celcius
  tc = dht.computeHeatIndex(tc,h,false);

  
  //tf = dht.computeHeatIndex(tf,h,false);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(tc) || isnan(tf)) {
    Serial.println("Failed to read from DHT sensor!");
    return false;
  }
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(tc);
  Serial.print(" *C ");
  //Serial.print(tf);
  //Serial.print(" *F\t");
  return true;
}

boolean SMSRequest() {
  if(SIM900.available() >0) {
    incomingChar=SIM900.read();
    if(incomingChar=='D') {
      delay(10);
      Serial.print(incomingChar);
      incomingChar=SIM900.read();
      if(incomingChar =='A') {
        delay(10);
        Serial.print(incomingChar);
        incomingChar=SIM900.read();
        if(incomingChar=='T') {
          delay(10);
          Serial.print(incomingChar);
          incomingChar=SIM900.read();
          if(incomingChar=='A') {
            delay(10);
            Serial.print(incomingChar);
            incomingChar=SIM900.read();
             Serial.print("...Request Received \n");
              return true;
          }
        }
      }
    }
  }
  return false;
}
