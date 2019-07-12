#include <Wire.h>
#include <DS3231.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define motor 2
#define ledRed A2
#define ledGreen A1
#define pino_sinal_analogico A0

int i = 0;
int ligado = 1;
int valor_analogico;
const int buzzer = 7;
int vetHora[] = {0, 0};
int horaDefinida[] = {0,0};
const byte linhas = 4;  
const byte colunas = 4; 

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

void serialFlush(){
    //limpar a entrada da portal serial
    while(Serial.available() > 0){
        char t = Serial.read();
    }
}

void percentUmd(int valor_analogico){
    //mostrar a porcentagem de umidade atual
    float x = 100*(((-1)*(valor_analogico-1000))/1000.0);
    lcd.setCursor(2, 0);
    lcd.print("Umidade:");
    lcd.print(x);
    lcd.print("%     ");
}

void horaDigitada(int* vetHora){
    //mostrar o horario que foi ou esta sendo definido
    lcd.print(vetHora[0]);
    lcd.print(":");
    lcd.print(vetHora[1]);
}

void buzzerLiga(){
    //som do buzzer ao ligar a bomba
    tone(buzzer, 2500);
    delay(500);
    noTone(buzzer);
}

void buzzerDesliga(){
    //som do buzzer ao desligar a bomba
    tone(buzzer, 2500);
    delay(500);
    noTone(buzzer);
    delay(200);
    tone(buzzer, 2500);
    delay(500);
    noTone(buzzer);
}

void EEPROMlimpa(){
    //limpar a memoria eeprom
    for(int i = 0 ; i < EEPROM.length() ; i++){
        EEPROM.write(i,0);
    }
}

int EEPROMescreve(char *p,int i , int j){
    //escrever uma frase na memoria eeprom
    while(p[j] != '\0'){
        if(i==(EEPROM.length()-1)){
            i=0;
        }
        EEPROM.write(i+1, p[j]);
        j++;
        i++;
    }
    EEPROM.write(0, i);
    return i;
}

void EEPROMleitura(){
    //ler todos dados gravados na eeprom
    int i = EEPROM.read(0), j = 1;
    while(j <= i){
        Serial.print(char(EEPROM.read(j)));
        j++;
    }
    Serial.print('\n');
}

void trocaSituacao(){
    //trocar os estados logicos dos leds e da bomba de agua
    digitalWrite(ledGreen,!digitalRead(ledGreen)); 
    digitalWrite(ledRed,!digitalRead(ledRed));
    digitalWrite(motor,!digitalRead(motor));
    ligado = !ligado;
}

void Menu(char opc,bool valor){
    //funcionalidade de um menu pelo monitor serial
    int horas, minutos;
    serialFlush();
    if(opc == '1'){
      Serial.print("DEFINA A HORA: ");
    }
    
    while(valor == true){
    
        if(opc == '1'){
            // definir horario de irrigaçao
            if(Serial.available()){
                
                String t = Serial.readStringUntil('\n');
                if(t.length()<6){
                    //conversao para decimal
                    horas = 10*(int(t[0])-48)+int(t[1])-48;
                    minutos = 10*(int(t[3])-48)+int(t[4])-48;
                }
                if(horas > 23 || minutos > 59){
                    //validando a hora inserida
                    horas = 0;
                    minutos = 0;
                    Serial.print("hora invalida \n");
                    Serial.print("DEFINA A HORA: ");
                }
                else{
                    //salvando e mostrando o horario definido
                    horaDefinida[0]= horas;
                    horaDefinida[1]= minutos;
                    Serial.print(horaDefinida[0]);
                    Serial.print("-");
                    Serial.print(horaDefinida[1]);
                    Serial.print("\n");
                    valor = false;
                }
            }
        }
        else if (opc == '2'){
            //funcao para ver o log da eeprom
            EEPROMleitura();
            valor = false;
        }
    }
}

void setup()
{
    EEPROMlimpa();

    //ativacao da porta serial e print da mensagem de menu
    Serial.begin(9600);
    Serial.print("[1] = PARA CONFIGURAR A HORA DE IRRIGACAO, PADRAO HH:MM \n");
    Serial.print("[2] = PARA VER O LOG\n");

    //definicao das portas
    pinMode(pino_sinal_analogico, INPUT);
    pinMode(motor, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);

    //definindo bomba de agua e led vermelho para comecarem ligados
    digitalWrite(motor, HIGH);
    digitalWrite(ledRed, HIGH);

    //Define o número de colunas e linhas do LCD
    lcd.begin(16, 2);

    //definicao da hora e data utilizada pelo RTC
    RTC.begin();
    RTC.setDateTime(__DATE__, __TIME__);
}

void loop()
{
    data = RTC.getDateTime();   //recebe data e hora atual
    valor_analogico = analogRead(pino_sinal_analogico); //leitura atual da umidade
    char apertatecla = teclado.getKey();    //recebe tecla apertada no teclado 4x4

    percentUmd(valor_analogico); 

    int h, m;
    h = data.hour;
    m = data.minute;
    char opc = '0';

    if(Serial.available()){
        //salvar em uma variavel a opcao digita na porta serial
        Serial.readBytesUntil('\n',&opc, 1);
        
        if (opc == '1' || opc == '2'){
            //chamar o menu para a opcao desejada 
            Menu(opc,true);
            Serial.print("[1] = PARA CONFIGURAR A HORA DE IRRIGACAO, PADRAO HH:MM \n");
            Serial.print("[2] = PARA VER O LOG\n");
            opc = '0';
        }
    }

    if (apertatecla) {
        //verifica se alguma tecla foi apertada no teclado 4x4
        lcd.setCursor(2, 1);

        if(apertatecla == '1'){
            //se foi apertada a tecla 1 é definida o valor da hora
            vetHora[0] += 1;
            if(vetHora[0] == 24){
                //limitador de valor de horas para o formato 24 horas
                vetHora[0] = 0;
            }

            horaDigitada(vetHora);
            delay(100);
            lcd.print("               ");
        }

        else if(apertatecla == '2'){
            //se foi apertada a tecla 2 são definidos os miniutos
            vetHora[1] += 5;
            if (vetHora[1] == 60){
                //limitador de valor dos minutos
                vetHora[1] = 0;
            }

            horaDigitada(vetHora);
            delay(100);
            lcd.print("               ");
        }

        else if(apertatecla == '3'){
            //se foi apertada a tecla 3, confirmar e salvar horario
            lcd.print("Hora definida");
            delay(1000);
            lcd.setCursor(2,1);
            lcd.print("               ");
            delay(500);
            lcd.setCursor(2,1);
            lcd.print("Hora  ");

            if(!(vetHora[0]==0 && vetHora[1]==0)){
                //verificando se é hora valida, e se tem algum horario para salvar
                horaDefinida[0] = vetHora[0];
                horaDefinida[1] = vetHora[1]; 
                vetHora[0] = 0;
                vetHora[1] = 0;
            }

            horaDigitada(horaDefinida);
            delay(2000);
            lcd.setCursor(2,1);
            lcd.print("               ");
            delay(1000);
        }
    }

    if (h == horaDefinida[0] && m == horaDefinida[1]){
        //verificar se o horario do RTC é o mesmo que o horario definido
        if(ligado==0){
        //ligar a bomba somente se antes ela estava deligada
            trocaSituacao();
            buzzerLiga();
        }
    }

    if (valor_analogico > 0 && valor_analogico < 400){
        //detecta solo umido
        if (ligado == 1){
            //se a bomba estava ligada
            trocaSituacao();
            buzzerDesliga();

            char p[40];
            sprintf(p, "Bomba Desigada %d:%d Horas \n", h, m);
            i = EEPROMescreve(p,i,0);
        }
    }

    else if (valor_analogico > 800 && valor_analogico < 1024){
        //detecta o solo seco
        if (ligado == 0){
            //se a bomba estava desligada
            trocaSituacao();
            buzzerLiga();

            char p[40];
            sprintf(p, "Bomba Ligada %d:%d Horas \n", h, m);
            i = EEPROMescreve(p,i,0);
        }
    }
    delay(100);
}