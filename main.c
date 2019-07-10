#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>

#define pino_sinal_analogico A0
#define motor 2
#define ledGreen A1
#define ledRed A2
#define cpl_bit(y,bit)   (y^=(1<<bit)) //troca o estado l�gico do bit x da vari�vel Y

int i = 0;
int ligado = 0;
int valor_analogico;
const int buzzer = 7;
int vetHora[] = {0, 0};
const byte linhas = 4;  //4 linhas
const byte colunas = 4; //4 colunas
//i = EEPROM.read(0)

char matrizteclado[linhas][colunas] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

DS3231 RTC;
RTCDateTime data;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
byte pinoslinhas[linhas] = {6}; //pinos utilizados nas linhas
byte pinoscolunas[colunas] = {5, 4, 3}; //pinos utilizados nas colunas
Keypad teclado = Keypad(makeKeymap(matrizteclado), pinoslinhas, pinoscolunas, linhas, colunas);


void Menu(char opc,bool valor){
while(valor == true){

    if(opc == '1'){
        // definir horario de irrigaçao
        if(Serial.available()>0){
            String t = Serial.readStringUntil('\n');
            Serial.print("valor");
            Serial.print(t);
            Serial.print("\n");
            valor = false;
        }

        /*if(Serial.available()){
            while(Serial.available() > 0){
                Serial.print(char(Serial.read()));
            }
        } */
    }
    else if (opc == '2'){
        // funcao para ver log
        EEPROMleitura();
        valor = false;
    }
}

}


void percentUmd(int valor_analogico){
    float x = 100*(((-1)*(valor_analogico-1000))/1000.0);
    lcd.setCursor(2, 0);
    //lcd.print("                ");
    lcd.print("Umidade:");
    lcd.print(x);
    lcd.print("%     ");
}

void horaDigitada(int* vetHora){
    lcd.print(vetHora[0]);
    lcd.print(":");
    lcd.print(vetHora[1]);
}

void buzzerLiga(){
    tone(buzzer, 2500);
    delay(500);
    noTone(buzzer);
}

void buzzerDesliga(){
    tone(buzzer, 2500);
    delay(500);
    noTone(buzzer);
    delay(200);
    tone(buzzer, 2500);
    delay(500);
    noTone(buzzer);
}

void EEPROMescreve(char *p,int i , int j){
    while(p[j] != '\0'){
        EEPROM.write(i+1, p[j]);
        j++;
        i++;
    }
    i++;
    EEPROM.write(i, '\n');
}

void EEPROMleitura(){
    int i=200, j = 1;
    while(j <= i){
        Serial.print(char(EEPROM.read(j)));
        j++;
    }

    EEPROM.write(0, i);
    Serial.print('\n');
}

void trocaSituacao(){
    digitalWrite(ledRed,!digitalRead(ledRed));
    digitalWrite(ledGreen,!digitalRead(ledGreen)); 
    //digitalWrite(motor,!digitalRead(motor));
    ligado = !ligado;
}

void setup()
{
    Serial.begin(9600);
    Serial.print("[1] = PARA CONFIGURAR A HORA DE IRRIGACAO \n");
    Serial.print("[2] = PARA VER O LOG\n");

    pinMode(pino_sinal_analogico, INPUT);
    pinMode(motor, OUTPUT); // bomba
    pinMode(buzzer, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);

    digitalWrite(motor, HIGH);
    digitalWrite(ledRed, HIGH);

    lcd.begin(16, 2);
    RTC.begin();
    RTC.setDateTime(__DATE__, __TIME__);
}

void loop()
{
    data = RTC.getDateTime();
    valor_analogico = analogRead(pino_sinal_analogico);
    char apertatecla = teclado.getKey();

    percentUmd(valor_analogico);

    int h, m;
    h = data.hour;
    m = data.minute;
    char opc = '0';

    if(Serial.available()){
        // while(Serial.available() > 0){
            Serial.readBytesUntil('\n', & opc, 1);
            //}

        if (opc == '1' || opc == '2'){
            Menu(opc,true);
        }
    }


    if (apertatecla) {
        lcd.setCursor(2, 1);

        if(apertatecla == '1'){

            vetHora[0] += 1;
            if(vetHora[0] == 24){
                vetHora[0] = 0;
            }

            horaDigitada(vetHora);
            delay(100);
            lcd.print("               ");
        }

        else if(apertatecla == '2'){

            vetHora[1] += 5;
            if (vetHora[1] == 60){
                vetHora[1] = 0;
            }

            horaDigitada(vetHora);
            delay(100);
            lcd.print("               ");
        }

        else if(apertatecla == '3'){

            lcd.print("Hora definida");
            delay(1000);
            lcd.setCursor(2,1);
            lcd.print("               ");
            delay(500);
            lcd.setCursor(2,1);
            lcd.print("Hora : ");
            horaDigitada(vetHora);
            delay(2000);
            lcd.setCursor(2,1);
            lcd.print("               ");
            delay(1000);
        }

    }

    if (h == vetHora[0] && m == vetHora[1]){
        lcd.setCursor(2, 1);
        lcd.print("ta na hora");
        delay(1000);
        lcd.setCursor(2, 1);        
        lcd.print("               ");
    }

    // Solo umido, acende o led verde
    if (valor_analogico > 0 && valor_analogico < 400)
    {
        digitalWrite(motor,LOW);
        if (ligado == 0)
        {
            trocaSituacao();
            buzzerLiga();

            int j = 0;
            char p[50];
            sprintf(p, "Bomba Desligada %d:%d Horas", h, m);
            EEPROMescreve(p,i,j);

        }
    }

    // Solo seco, acende led vermelho
    if (valor_analogico > 800 && valor_analogico < 1024)
    {
        digitalWrite(motor,HIGH);
        if (ligado == 1)
        {
            trocaSituacao();
            buzzerDesliga();

            int j = 0;
            char p[50];
            sprintf(p, "Bomba Ligada %d:%d Horas", h, m);
            EEPROMescreve(p,i,j);

        }
    }
    delay(100);
}
