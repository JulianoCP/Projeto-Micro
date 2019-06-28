#include <LiquidCrystal.h>
#include <Keypad.h>
#include <DS1307.h>

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

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
DS1307 rtc(A4, A3);
byte pinoslinhas[linhas] = {6}; //pinos utilizados nas linhas
byte pinoscolunas[colunas] = {5,4,3}; //pinos utilizados nas colunas
Keypad teclado = Keypad( makeKeymap(matrizteclado), pinoslinhas, pinoscolunas, linhas, colunas );

void setup()
{
    //rtc.setDOW(FRIDAY);      //Define o dia da semana
    //rtc.setTime(16, 16, 0);     //Define o horario
    //rtc.setDate(12, 6, 2019);   //Define o dia, mes e ano
    rtc.halt(false);
    Serial.begin(9600);
    pinMode(pino_sinal_analogico, INPUT);
    pinMode(motor, OUTPUT); //bomba
    pinMode(buzzer, OUTPUT);
    pinMode(ledGreen,OUTPUT);
    pinMode(ledRed,OUTPUT);
    lcd.begin(16, 2);
    Serial.begin(9600);
    Serial.println("Pressione uma tecla");
    rtc.setSQWRate(SQW_RATE_1);
    rtc.enableSQW(true);
}

void loop()
{
    //Le o valor do pino A0 do sensor
    lcd.clear();
    valor_analogico = analogRead(pino_sinal_analogico);
    char apertatecla = teclado.getKey();
    lcd.setCursor(2, 0);
    lcd.print("Umidade:");
    float x = valor_analogico/1000.0;
    lcd.print(x);

    if (apertatecla) {
        if(apertatecla == '1'){
            vetHora[0] += 10;
            lcd.clear();
            lcd.print(vetHora[0]);
            lcd.print(":");
            lcd.print(vetHora[1]);
            delay(1000);
        }
        else if(apertatecla == '2'){
            vetHora[1] += 10;
            lcd.clear();
            lcd.print(vetHora[0]);
            lcd.print(":");
            lcd.print(vetHora[1]);
            delay(1000);
        }
        else if(apertatecla == '3'){
            lcd.clear();
            lcd.print("Hora : ");
            //char *x = rtc.getTimeStr();
            //lcd.print(x[0]);
            lcd.print(rtc.getTimeStr());

            //Aguarda 1 segundo e repete o processo
            delay (3000);
           
        }
        else{
            lcd.print("sqwiejhqwe");
        }
    }


    //Solo umido, acende o led verde
    if (valor_analogico > 0 && valor_analogico < 400)
    {
        digitalWrite(motor, LOW); //bomba

        if (ligado == 0)
        {
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