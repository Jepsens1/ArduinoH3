#include <SPI.h>
#include <WiFi101.h>

int motorPin1 = 8; // Blue – 28BYJ48 pin 1
int motorPin2 = 9; // Pink – 28BYJ48 pin 2
int motorPin3 = 10; // Yellow – 28BYJ48 pin 3
int motorPin4 = 11; // Orange – 28BYJ48 pin 4

int motorSpeed = 1200; //variable to set stepper speed
int count = 0; // count of steps made
int countsperrev = 512; // number of steps per full revolution
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};


char ssid[] = "TestRouter";      //  network SSID (name) 

int status = WL_IDLE_STATUS;
IPAddress server(192,168,1,112); //IP to server
WiFiClient client; //Cleint
bool IsHatchOn = false; //variable for status of hatch
bool ClientHatchsOn = false; //used for client so either open or close hatch
int sendMessageTimer = 0; //timer for send message
int sendMessageMaxTimer = 10 * 7000; //max timer

void setup() {
//declare the motor pins as outputs
pinMode(motorPin1, OUTPUT);
pinMode(motorPin2, OUTPUT);
pinMode(motorPin3, OUTPUT);
pinMode(motorPin4, OUTPUT);
Serial.begin(9600);
 delay(2000);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid,"Kode1234!"); //Connect to wifi
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
  } 
  else {
    Serial.println("Connected to wifi");
    Serial.println("\nStarting connection...");
    // Connects to tcp server, sends "2" to server, to let know arduino joined
    if (client.connect(server, 9999)) {
      Serial.println("connected");
      client.println("2");
      client.flush();
      
    }
  }
}

void loop(){
  //Reads data from server
  char c = client.read();
  if(c == 'H')
  {
    Serial.println(c);
    ClientHatchsOn = !ClientHatchsOn;
    //Simulate to open hatch
     if(ClientHatchsOn)
    {
      for(int i = 0; i < 500; i++)
      {
      clockwise();        
      }
    
    }
    //Simulate to close hatch
   else if(!ClientHatchsOn) {
     for(int i = 0; i < 500; i++)
      {
      anticlockwise();
      }
   }
  }
  if(ClientHatchsOn)
  {
    //Used as timer, instead using delay(), so we dont spam server
    if(sendMessageTimer >= sendMessageMaxTimer)
    {
      //Reset timer 
       sendMessageTimer = 0;
       //Sends data to server
       client.print("Hatch true");
    client.flush();
    }
  }
  else {
        //Used as timer, instead using delay(), so we dont spam server
    if(sendMessageTimer >= sendMessageMaxTimer)
    {
      //Reset timer
      sendMessageTimer = 0;
       //Sends data to server
      client.print("Hatch false");
    client.flush();
    }
  }
  sendMessageTimer++;
}

void anticlockwise()
{
for(int i = 0; i < 8; i++)
{
setOutput(i);
delayMicroseconds(motorSpeed);
}
}

void clockwise()
{
for(int i = 7; i >= 0; i--)
{
setOutput(i);
delayMicroseconds(motorSpeed);
}
}
//Writes to pins
void setOutput(int out)
{
digitalWrite(motorPin1, bitRead(lookup[out], 0));
digitalWrite(motorPin2, bitRead(lookup[out], 1));
digitalWrite(motorPin3, bitRead(lookup[out], 2));
digitalWrite(motorPin4, bitRead(lookup[out], 3));
}