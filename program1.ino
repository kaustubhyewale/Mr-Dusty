
/*
 *              Project Name:- Mr Dusty
 *              Group Name:-  Rutherford @Core2Web Technology
 * 
 */
//Add header files
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <time.h>


const char ssid[] = "Rutherford";  // your network SSID (name)
const char pass[] = "core2Web";   // your network password
int statusCode = 0;
WiFiClient  client;


//---------Channel Details---------//
unsigned long myChannelNumber = 844803;            // Channel ID
const char * myReadAPIKey = "MQ5JN9YWFXAV61D9"; // Read API Key
const char * myWriteAPIKey = "7NBQ69K8AQU59B4I"; // Write API Key
const char* server = "api.thingspeak.com";

//varibles for the fields
const int FieldNumber1 = 1;
const int FieldNumber2 = 2;
const int FieldNumber3 = 3;
const int FieldNumber4 = 4;
//-------------------------------//


//
int timezone = 5.50 * 3600;
int dst = 0;

long hour = -1;
long minute = -1;


uint8_t wire1 = D8;
uint8_t wire2 = D7;
uint8_t wire3 = D6;
uint8_t wire4 = D5;

uint8_t wire5 = D3;
uint8_t wire6 = D2;
uint8_t wire7 = D1;
uint8_t wire8 = D0;

const uint16_t _delay = 2; /* delay in between two steps. minimum delay more the rotational speed */

void sequence(bool a, bool b, bool c, bool d){  /* four step sequence to stepper motor */
  digitalWrite(wire1, a);
  digitalWrite(wire2, b);
  digitalWrite(wire3, c);
  digitalWrite(wire4, d);
  delay(_delay);
}


//Function for wifi connection
void networkConnection()
{
  
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
    Serial.print("Connect to Wi-Fi");
    Serial.println(" Succesfully.");
    delay(1000);
  }  
}

//function to get data from the cloud
long getDataFromCloud(int FieldNum)
{
  long val = ThingSpeak.readLongField(myChannelNumber, FieldNum, myReadAPIKey);
  statusCode = ThingSpeak.getLastReadStatus();

  if (statusCode == 200)
  {
    Serial.println(val);
  }
  else
  {
    Serial.println("Unable to read or No internet!");
  }
  delay(1000);

  return val;
}

//Function to send the data to cloud
void sendDataToCloud()
{

   // set the fields with the values
  ThingSpeak.setField(1, -1);
  ThingSpeak.setField(2, -1);
  ThingSpeak.setField(3, 0);
  ThingSpeak.setField(4, 1);


  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(20000);
  
}



//initilize the pins used
void setup()
{
  
  Serial.begin(115200);  // Initialize serial
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  Serial.print("Display Test:OK");
  delay(1000);

  pinMode(wire1, OUTPUT); /* set four wires as output for Stepper Motor (Wheels) */
  pinMode(wire2, OUTPUT);
  pinMode(wire3, OUTPUT);
  pinMode(wire4, OUTPUT);

  pinMode(wire8,OUTPUT); /* set four wires as output for DC (300rpm) Motor (cleaner roller) */
  pinMode(wire7,OUTPUT);
  pinMode(wire6,OUTPUT);
  pinMode(wire5,OUTPUT);
  
 digitalWrite(wire8,LOW); /* DC Motor is OFF now*/
 digitalWrite(wire7,LOW);
 digitalWrite(wire6,LOW);
 digitalWrite(wire5,LOW);
 

  configTime(timezone,dst,"pool.ntp.org","time.nist.gov");
  Serial.println("waitingfor Internet time");

  while(!time(nullptr)){
    Serial.print("*");
    delay(1000);  
  }
}



void loop() {

    networkConnection();

 
    //  Read Data From Cloud
    long userHour = getDataFromCloud(FieldNumber1);
    long userMinute = getDataFromCloud(FieldNumber2);
    long userManual = getDataFromCloud(FieldNumber3);
    long userStatus = getDataFromCloud(FieldNumber4);
    
    if (userHour != -1 && userMinute != -1)
    {
      hour = userHour;
      minute = userMinute;  
    }

    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);

    Serial.println(p_tm->tm_hour);
    Serial.println(p_tm->tm_min);
 
    if (((hour == (p_tm->tm_hour) && minute == (p_tm->tm_min)) || userManual==1) && userStatus == 0)
    {
  
    digitalWrite(wire8,HIGH);
    digitalWrite(wire7,LOW);
    digitalWrite(wire6,HIGH);
    digitalWrite(wire5,LOW);

  
    /* Rotation in one direction */
    for(int i = 0; i<600; i++)
    {
      sequence(HIGH, LOW, LOW, LOW);
      sequence(HIGH, HIGH, LOW, LOW);
      sequence(LOW, HIGH, LOW, LOW);
      sequence(LOW, HIGH, HIGH, LOW);
      sequence(LOW, LOW, HIGH, LOW);
      sequence(LOW, LOW, HIGH, HIGH);
      sequence(LOW, LOW, LOW, HIGH);
      sequence(HIGH, LOW, LOW, HIGH);
    }
    sequence(HIGH, LOW, LOW, LOW);
  
    /* Rotation in opposite direction */
    for(int j = 0; j<600; j++)
    {
      sequence(LOW, LOW, LOW, HIGH);
      sequence(LOW, LOW, HIGH, HIGH);
      sequence(LOW, LOW, HIGH, LOW);
      sequence(LOW, HIGH, HIGH, LOW);
      sequence(LOW, HIGH, LOW, LOW);
      sequence(HIGH, HIGH, LOW, LOW);
      sequence(HIGH, LOW, LOW, LOW);
      sequence(HIGH, LOW, LOW, HIGH);
    }
    
    sequence(LOW, LOW, LOW, HIGH);
  
    digitalWrite(wire8,LOW);
    digitalWrite(wire7,LOW);
    digitalWrite(wire6,LOW);
    digitalWrite(wire5,LOW);

    sendDataToCloud();

  }
  
}
