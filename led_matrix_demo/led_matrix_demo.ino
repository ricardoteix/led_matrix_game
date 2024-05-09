#include <LedControl.h>

// Definição dos pinos para conexão com o MAX7219
#define DATA_IN_PIN   A0   // Pino de dados (DIN)
#define LOAD_PIN      A1  // Pino de carga (CS)
#define CLK_PIN       A2  // Pino do clock (CLK)
#define NUM_DISPLAYS  1   // Número de matrizes de LED no display

LedControl lc = LedControl(DATA_IN_PIN, CLK_PIN, LOAD_PIN, NUM_DISPLAYS);

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 2);
  lc.clearDisplay(0);


  lc.setLed(0, 0, 0, 1);
  lc.setLed(0, 1, 1, 1);
  lc.setLed(0, 2, 2, 1);
  lc.setLed(0, 3, 3, 0);
  lc.setLed(0, 4, 4, 0);
  lc.setLed(0, 5, 5, 1);
  lc.setLed(0, 6, 6, 1);
  lc.setLed(0, 7, 7, 1);


  for(int i = 0; i < 8; i++) {
    lc.setRow(0, i, B00000001 << i);
    delay(1000);
  }

}

void loop() {
  
}
