#include <LiquidCrystal_I2C.h>
int segundos = 0;
int minutos = 0;

LiquidCrystal_I2C tela(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  tela.init();
  tela.backlight();
}

void loop() {
  printarTempoTela();
  delay(1000);
}

void printarTempoTela() {
  segundos++;
  if (segundos > 60) {
    tela.setCursor(0, 0);
    minutos++;
    tela.print("Minutos: " + String(minutos));

    segundos = 0;
    tela.setCursor(11, 1);
    tela.print(" ");
  }
  tela.setCursor(0, 1);
  tela.print("Segundos: " + String(segundos));
}


