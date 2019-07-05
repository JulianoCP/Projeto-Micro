#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>

#define pino_sinal_analogico A0
#define motor 2
#define ledGreen A1
#define ledRed A2


int valor_analogico;
const int buzzer = 7;
int ligado = 0;

int vetHora[] = {0,0};
const byte linhas = 4; //4 linhas
const byte colunas = 4; //4 colunas

char matrizteclado[linhas][colunas] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
int i = 0;
//i = EEPROM.read(0);
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
DS3231 RTC;              
RTCDateTime data;
byte pinoslinhas[linhas] = {6}; //pinos utilizados nas linhas
byte pinoscolunas[colunas] = {5,4,3}; //pinos utilizados nas colunas
Keypad teclado = Keypad( makeKeymap(matrizteclado), pinoslinhas, pinoscolunas, linhas, colunas );



void Menu(char x){
    
    if(x == '1'){
      String t = Serial.readStringUntil('\n');
      Serial.print(t); 
      
      
      /*if(Serial.available()){
        while(Serial.available()>0){
          Serial.print(char(Serial.read()));
        }
      } */
      
    }
    else if (x=='2'){
      
    }
    
}


void setup()
{
    Serial.begin(9600);
    Serial.print("[1] = PARA CONFIGURAR A HORA DE IRRIGACAO \n");
    Serial.print("[2] = PARA VER O LOG\n");


    
    pinMode(pino_sinal_analogico, INPUT);
    pinMode(motor, OUTPUT); //bomba
    pinMode(buzzer, OUTPUT);
    pinMode(ledGreen,OUTPUT);
    pinMode(ledRed,OUTPUT);
    lcd.begin(16, 2);
    RTC.begin();
    RTC.setDateTime(__DATE__, __TIME__);
    
}

void loop()
{
    
    //Le o valor do pino A0 do sensor
    //lcd.clear();
    data = RTC.getDateTime();
    valor_analogico = analogRead(pino_sinal_analogico);
    char apertatecla = teclado.getKey();
    lcd.setCursor(2, 0);
    
    lcd.print("Umidade:");
    float x = valor_analogico/1000.0;
    lcd.print(x);
    lcd.print("%");
    int h, m;
    h = data.hour;
    m = data.minute;

    char opc = char(Serial.read());
    if (opc == '1' || opc == '2'){
      
      Menu(opc);
    }
    
    if (apertatecla) {
      
        if(apertatecla == '1'){
          
          lcd.setCursor(2, 1);
            vetHora[0] += 1;
            if(vetHora[0]==24){
              vetHora[0]=0;
            }
            lcd.print(vetHora[0]);
            lcd.print(":");
            lcd.print(vetHora[1]);
            delay(200);
            lcd.print("               ");
        }
        else if(apertatecla == '2'){
          
         lcd.setCursor(2, 1);
            vetHora[1] += 5;
            if (vetHora[1] == 60){
              vetHora[1]=0;
              //vetHora[0]+=1;
            }
            
            lcd.print(vetHora[0]);
            lcd.print(":");
            lcd.print(vetHora[1]);
            delay(200);
            lcd.print("               ");
        }
        else if(apertatecla == '3'){
            
            //lcd.clear();
            lcd.setCursor(2, 1);
            lcd.print("Hora : ");
            lcd.print(vetHora[0]);
            lcd.print(":");
            lcd.print(vetHora[1]);

            //lcd.print(data.hour);     
            //Serial.print("-");
            //Serial.print(data.month);    
            //Serial.print("-");
            //Serial.print(data.day);      
            //Serial.print(" ");
            //Serial.print(data.hour);     
            //Serial.print(":");
            //Serial.print(data.minute);   
            //Serial.print(":");
            //Serial.print(data.second);   
            //Serial.println("");

            //Aguarda 1 segundo e repete o processo
            delay (1000);        
        }
       
    }
    if (h == vetHora[0] && m == vetHora[1]){
         lcd.setCursor(2, 1);
         lcd.print("ta na hora");   
         delay (1000);
         lcd.clear();                             
    }

    //Solo umido, acende o led verde
    if (valor_analogico > 0 && valor_analogico < 400)
    {
        digitalWrite(motor, LOW); //bomba

        if (ligado == 0)
        {
            
            int j = 0;
            int x = data.hour;
            int y = data.minute;
            char p[50];
            sprintf(p,"Bomba Desligadu %d:%d Horas",x ,y);
            while(p[j] != '\0'){
                EEPROM.write(i+1, p[j]);
                j++;
                i++;
            }
            i++;
            EEPROM.write(i,'\n');
            j = 1;
            while(j <= i){
              Serial.print(char(EEPROM.read(j)));
              j++;
            }
            EEPROM.write(0,i);
            Serial.print('\n');
            digitalWrite(ledRed, LOW);
            digitalWrite(ledGreen, HIGH);
            ligado = 1;
            tone(buzzer, 2500);
            delay(500);
            noTone(buzzer);
            delay(1000);
        }
    }

    //Solo seco, acende led vermelho
    if (valor_analogico > 800 && valor_analogico < 1024)
    {
        digitalWrite(motor, HIGH); //bomba
        
        if (ligado == 1)
        {
            int j = 0;
            int x = data.hour;
            int y = data.minute;
            char p[50];
            sprintf(p,"Bomba Ligado %d:%d Horas",x ,y);
            while(p[j] != '\0'){
                EEPROM.write(i+1, p[j]);
                j++;
                i++;
            }
            i++;
            EEPROM.write(i,'\n');
            j = 1;
            while(j <= i){
              Serial.print(char(EEPROM.read(j)));
              j++;
            }
            EEPROM.write(0,i);
            Serial.print('\n');
            digitalWrite(ledRed, HIGH);
            digitalWrite(ledGreen, LOW);
            ligado = 0;
            tone(buzzer, 2500);
            delay(500);
            noTone(buzzer);
            delay(200);
            tone(buzzer, 2500);
            delay(500);
            noTone(buzzer);
        }
    }
    delay(100);
}
