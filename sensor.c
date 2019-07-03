//Programa: Monitoracao de planta usando Arduino
//Autor: FILIPEFLOP

/*
sensor   arduino
a0 cinza a0
gnd azul gnd
5v rosa  5v
*/

#define pino_sinal_analogico A0
#define pino_led_vermelho 5
#define pino_led_amarelo 6
#define pino_led_verde 7
 
int valor_analogico;
 
void setup()
{
  Serial.begin(9600);
  pinMode(pino_sinal_analogico, INPUT);
  //pinMode(9,OUTPUT);bomba

}
 
void loop()
{
  //Le o valor do pino A0 do sensor
  valor_analogico = analogRead(pino_sinal_analogico);
 
  //Mostra o valor da porta analogica no serial monitor
  Serial.print("Porta analogica: ");
  Serial.print(valor_analogico);
 
  //Solo umido, acende o led verde
  if (valor_analogico > 0 && valor_analogico < 400)
  {
    Serial.println(" Status: Solo umido");
    //apagaleds();
    //digitalWrite(9,HIGH);bomba
    //digitalWrite(pino_led_verde, HIGH);
  }
 
  //Solo com umidade moderada, acende led amarelo
  if (valor_analogico > 400 && valor_analogico < 800)
  {
    Serial.println(" Status: Umidade moderada");
    //apagaleds();
    //digitalWrite(PB1, HIGH);
    //digitalWrite(pino_led_amarelo, HIGH);
  }
   //Solo seco, acende led vermelho
  if (valor_analogico > 800 && valor_analogico < 1024)
  {
    Serial.println(" Status: Solo seco");
    //apagaleds();
    //digitalWrite(9,LOW);bomba
    //digitalWrite(pino_led_vermelho, HIGH);
  }
  delay(100);
}
 
void apagaleds()
{
  digitalWrite(pino_led_vermelho, LOW);
  digitalWrite(pino_led_amarelo, LOW);
  digitalWrite(pino_led_verde, LOW);
}

