#include <SPI.h>
#include <WiFi101.h>
#include "DHT.h"

#define DHTPIN 1     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define fan 4
#define TEMPSIZE 10

int maxHum = 60;
int maxTemp = 40;

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "TestRouter";  //  network SSID (name) 
float temperatures[TEMPSIZE];   //used for average temp

int status = WL_IDLE_STATUS;
IPAddress server(192,168,1,112);

WiFiClient client;

void setup() {
  delay(2000);
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  //Connects to wifi
  status = WiFi.begin(ssid,"Kode1234!");
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
  } 
  else {
    Serial.println("Connected to wifi");
    Serial.println("\nStarting connection...");
    // Connects to tcp, sends 2 to server, to let know its arduino joined
    if (client.connect(server, 9999)) {
      Serial.println("connected");
      client.println("2");
      //Begins reading temp
      dht.begin();
      client.flush();
    }
  }
  
  
}
void AddTemp(float temp)
{
  ShiftFloats();
  for(int i = 0; i < TEMPSIZE; i++)
  {
    if(temperatures[i] == 0)
    {
      //Adds latest temp to array
      temperatures[i] = temp;
      
    }
  }
}

void ShiftFloats()
{
    
  float next = temperatures[0];
  for(int i = 0; i < TEMPSIZE; i++)
  {
    if(i <= TEMPSIZE)
    {
      //Gets the next value
      float temp = temperatures[i + 1];
      //Sets value from index(i) to index i + 1
      temperatures[i + 1] = next;
      //Updates next variable
      next = temp;
    }
  }
  temperatures[0] = 0;
}
float FindAverage()
{
  float average = 0;
  for(int i = 0; i < TEMPSIZE; i++)
  {
    //Adds all temp value to float variable
    average += temperatures[i];
  }
  //Returns average
  return average / TEMPSIZE;
}
void loop() {
  //Delay 2 seconds to make sure the sensors have time
  delay(2000);
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  //Add temp to array
  AddTemp(t);
  float temp = FindAverage();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if(t > maxTemp) {
    //Turn on fan
      digitalWrite(fan, HIGH);
  } else {
    //turn of fan
     digitalWrite(fan, LOW); 
  }
  String buf;
  buf += F("Temp ");
  buf += String(temp, 1);
  Serial.println(buf);
  //Sends temp data to tcp server
  client.print(buf); 
  }
