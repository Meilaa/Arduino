
#include  <WiFiClientSecure.h>
#include "DHT.h"


#define  DHTTYPE DHT22 

const int DHTPin = D1; 
DHT dht(DHTPin, DHTTYPE); 

#define ON_Board_LED  2  // Defining an On Board LED, used for indicators when the process of connecting  to a wifi router

//SSID and Password  of your WiFi router.
const char* ssid = "SDL AP1"; //Wifi name or  SSID.
const char* password = "Systechd33"; // Wifi password.

//Host  & httpsPort
const char* host = "script.google.com";
const int httpsPort  = 443;

WiFiClientSecure client;  // Create a WiFiClientSecure object.

String GAS_ID = "AKfycbwriCAA9Gjh9V0sspRh8l_PPrfINCba-OwKsjBFoElH1B0ITnnGUqn9-1gTVRBLi3b-";  //spreadsheet script ID

void setup() {
  Serial.begin(115200);
  delay(500);

  dht.begin();
  delay(500);
  
  WiFi.begin(ssid, password);
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT);  
  digitalWrite(ON_Board_LED, HIGH);  
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)  {
    Serial.print(".");
    // Turn on Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED,  LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH); // Turn off the On Board LED when it is connected  to the wifi router.
  Serial.println("");
  Serial.print("Successfully connected  to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  client.setInsecure();
}
void loop() {

  int h = dht.readHumidity();
  // Read temperature 
  float t = dht.readTemperature();
  
  // Check if any reads  failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed  to read from DHT sensor !");
    delay(500);
    return;
  }
  String  Temp = "Temperature : " + String(t) + " C";
  String Humi = "Humidity :  " + String(h) + " %";
  Serial.println(Temp);
  Serial.println(Humi);
  
  sendData(t, h); // Calls the sendData Subroutine
}
// Subroutine for sending data to Google Sheets
void sendData(float  tem, int hum) {
  Serial.println("==========");
  Serial.print("connecting  to ");
  Serial.println(host);
  
  //Connect  to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection  failed");
    return;
  }

  //Processing data and sending data
  String string_temperature =  String(tem);
  String string_humidity =  String(hum, DEC); 
  String  url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature + "&humidity="  + string_humidity;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\
\
" +
         "Host:  " + host + "\
\
" +
         "User-Agent: BuildFailureDetectorESP8266\
\
"  +
         "Connection: close\
\
\
\
");

  Serial.println("request  sent");

  //Checking  whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\
") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\\"state\\":\\"success\\""))  {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply  was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
}  
