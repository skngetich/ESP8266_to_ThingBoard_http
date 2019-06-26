#include <ESP8266WiFi.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp;

char ssid[]= "YOUR WIFI NAME"; // case sensitive
char password[] = "YOUR WIFI PASSWORD";

char serverAddress[] = "YOU SERVER IP";  // server address(hosting the thingsboard)
int port = 9090;//thingsboard port
String accessToken = "ACCESS TOKEN PROVIDED BY THINGSBOARD"


int status = WL_IDLE_STATUS;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting");
  delay(3000);

  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  dht.begin();

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  delay(3000);
  Serial.print("Creating json buffer");
  
 
}

void loop(){  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
 

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

    int bmpTemp = bmp.readTemperature();
    int pressure = bmp.readPressure();
    int altitude = bmp.readAltitude();
    
    Serial.print("Temperature = ");
    Serial.print(bmpTemp);
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(altitude);
    Serial.println(" meters");
    
    delay(500);



  
  //http post request
  String postdata= String("{\"temp\":")+ t +
                          ",\"humidity\":"+ h +
                          ",\"headtIndex\":"+ hic +
                          ",\"pressure\":"+ pressure +
                          ",\"bmpTemp\":" + bmpTemp +"}";
                          
  String postRequestUrl = String("/api/v1/")+ accessToken + "/telemetry";
  
  client.post(postRequestUrl,"application/json",postdata);
  
 // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  
  
  delay(5000);
  }

