//Autor: http://blog.baudaeletronica.com.br/utilizando-o-rtc-com-arduino/

#include <Wire.h>         
#include <DS3231.h>      
 
DS3231 RTC;              
RTCDateTime data;
    
void setup()
{
  Serial.begin(9600);     
  RTC.begin();            
  RTC.setDateTime(__DATE__, __TIME__);    
                                         
}
void loop()
{
  data = RTC.getDateTime();      
                                     
 
 
  Serial.print(data.year);     
  Serial.print("-");
  Serial.print(data.month);    
  Serial.print("-");
  Serial.print(data.day);      
  Serial.print(" ");
  Serial.print(data.hour);     
  Serial.print(":");
  Serial.print(data.minute);   
  Serial.print(":");
  Serial.print(data.second);   
  Serial.println("");
 
  delay(1000);     
 
}