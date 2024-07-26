#include   <dht.h> 
#include   <LiquidCrystal_I2C.h> 
#include <Wire.h> 
dht DHT;

//Constants
#define   DHT22_PIN 2     
LiquidCrystal_I2C lcd(0x27,16,2);   

//Variables
float hum;  //Stores   humidity value
float temp; //Stores temperature value

void setup()
{
  Serial.begin(9600);
  lcd.init();                    
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setBacklight(HIGH);
}

void   loop()
{
    int chk = DHT.read22(DHT22_PIN);
    hum = DHT.humidity;
    temp= DHT.temperature;
    lcd.setCursor(0,0);
    lcd.print("Humidity:   ");
    lcd.print(hum);
    lcd.print("%");
    lcd.setCursor(0,1);
     lcd.print("Temp: "); 
    lcd.print(temp);
    lcd.println("Celcius");
     delay(2000); 
}