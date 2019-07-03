/*
lcd        uno
14 cinza   13
13 rosa    12
12 azul    11
11 verde   10
6 amarelo  9
4 laranja  8
5 amarelo  gnd
3 cinza    potenciometro
2 vermelho 5v
1 marrom   gnd
15 branco  5v
16 preto   gnd
*/

//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>
 
//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(8,9,10,11,12,13);
 
void setup()
{
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);
}
 
void loop()
{
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(2, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("BOMBA LIGADA");
  lcd.setCursor(2, 1);
  lcd.print("UTFPR - 2019");
  delay(5000);
   
  //Rolagem para a esquerda
  for (int posicao = 0; posicao < 2; posicao++)
  
    lcd.scrollDisplayLeft();
    delay(300);
  
   
  //Rolagem para a direita
  for (int posicao = 0; posicao < 4; posicao++)
  
    lcd.scrollDisplayRight();
    delay(300);
  
}
