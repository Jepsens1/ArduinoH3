#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "TestRouter";      // network SSID (name) 

int status = WL_IDLE_STATUS; // used for wifi status
IPAddress server(192,168,1,112); //Ip to tcp server
bool IsLightsOn = false; //used for status of lights
bool ClientLightsOn = false; //used for client for either turn on or off lights
int timer = 0; //used for how long lights has being on or off
int maxTimer = 10 * 1000; //max for how long lights to be on or off
int sendMessageTimer = 0; //timer for send message
int sendMessageMaxTimer = 10 * 100; //max timer for send message

WiFiClient client; //Client object

const int LED_PIN = 3;  // Arduino pin connected to LED's pin

void setup() {
  //Configure pin to be output mode
  pinMode(LED_PIN, OUTPUT);
  delay(2000);
  // initialize serial communication at 2000000 bits per second:
  Serial.begin(2000000);
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
    //Connect to tcp server
    if (client.connect(server, 9999)) {
      Serial.println("connected");
      //Sends 2 to tcp server, to let server know its arduino that joined
      client.println("2");
      client.flush();
      
    }
  }
  
}

void loop() {

  if(client.connected())
  {
   // reads the input on analog pin A1 (value between 0 and 1023)
  int analogValue = analogRead(A1);
  //reads data from tcp server
  char c = client.read();
  if(c == 'L')
  {
    //sets lights timer to 0
    timer = 0;
    Serial.println(c);
    ClientLightsOn = !ClientLightsOn;
  }
  //Checks to see if its dark in room and timer is 0
  if(analogValue < 150 && timer == 0)
  {
    IsLightsOn = true;
  }
  //Checks to see if there is enough light in room
  else if(analogValue >= 150 && timer == 0){
    IsLightsOn = false;
  }
  if(ClientLightsOn)
  {
    if(timer == 0)
    {
      IsLightsOn = !IsLightsOn;
    }
    timer++;
    if(timer >= maxTimer)  
    {
      timer = 0;
      ClientLightsOn = !ClientLightsOn;
    }  
  }
  if(IsLightsOn)
  {
    //Turn on light
    digitalWrite(LED_PIN, HIGH);
    if(sendMessageTimer >= sendMessageMaxTimer)
    {
      //reset timer for send message
       sendMessageTimer = 0;
       //sends message to server
      client.print("Lights true");
      client.flush();
    }
  }
  else{
    //turn off light
    digitalWrite(LED_PIN, LOW);
    if(sendMessageTimer >= sendMessageMaxTimer)
    {
      //reset timer for send message
      sendMessageTimer = 0;
      //Sends message to server
    client.print("Lights false");
    client.flush();
    }
  }
  sendMessageTimer++;
  }
}