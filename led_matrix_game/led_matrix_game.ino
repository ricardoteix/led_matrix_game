/*
  Adicionar elementos que precisam ser destruídos com X tiros, se possível diminuindo o brilho até desaparecer
*/

#include <LedControl.h>

#define X_AXIS A0
#define Y_AXIS A1
#define KEY 8

// Definição dos pinos para conexão com o MAX7219
#define DATA_IN_PIN   2  // Pino de dados (DIN)
#define CLK_PIN       3  // Pino do clock (CLK)
#define LOAD_PIN      4  // Pino de carga (CS)

// Número de matrizes de LED no display
#define NUM_DISPLAYS  4  

LedControl lc = LedControl(DATA_IN_PIN, CLK_PIN, LOAD_PIN, NUM_DISPLAYS);

int displayBits[][8] = {
  {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000
  },
  {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000
  },
  {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000
  },
  {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000
  }
};

int linhaNave = 0;
unsigned long tempoNave = millis();
unsigned long tempoTiro = millis();
unsigned long tempoAtualizarTiro = millis();

// Definição da estrutura para representar um objeto com x, y e display
struct Tiro {
  byte display = 255;
  byte x = 255;
  byte y = 255;
};

// Lista para armazenar os objetos LEDPosition
Tiro tiros[8]; // Tamanho da lista é 10 como exemplo

// Função para adicionar um objeto à lista de tiros
void adicionarTiro(byte display, byte x, byte y) {
  
  // Só adiciona se não tem na linha
  if (tiros[display].display == 255 && tiros[x].x != x) {
    tiros[x].x = x;
    tiros[x].y = y;
    tiros[x].display = display;
  }
}

void atualizarTiros() {
  if (millis() - tempoAtualizarTiro > 30) {
    
    tempoAtualizarTiro = millis();

    for (int i = 0; i < 8; i++) {

      if (tiros[i].display == 255) {
        setBitValue(0, tiros[i].x, 0, false);
        continue;
      }

      if ((tiros[i].display == 3 && tiros[i].y != 5) || tiros[i].display < 3) {
        setBitValue(tiros[i].display, tiros[i].x, tiros[i].y + 1, false);
      }

      if (tiros[i].y == 7) {
        setBitValue(tiros[i].display + 1, tiros[i].x, 0, false);
      }
      
      setBitValue(tiros[i].display, tiros[i].x, tiros[i].y, true);

      if (tiros[i].y == 0 && tiros[i].display == 0) {
        setBitValue(tiros[i].display, tiros[i].x, tiros[i].y, false);
      }

      if (tiros[i].y == 0) {
        tiros[i].y = 7;
        tiros[i].display--;
      } else {
        tiros[i].y--;
      }

      if (tiros[i].display == 255) {
        tiros[i].x = 255;
        tiros[i].y = 255;
      }
      
    }
  }
}

// Modifica o valor de 1 bit na matriz geral
void setBitValue(int display, int row, int column, bool value) {
  int result = displayBits[display][row];
  result = bitWrite(result, column, value);
  displayBits[display][row] = result;
}

void exibirMatrizes() {
  for (byte d = 0; d < NUM_DISPLAYS; d++) {
    for (byte i = 0; i < 8; i++) {
      lc.setRow(d, i, displayBits[d][i]); 
    }
  }
}

// Função para acender um LED em um display específico
void acenderLED(int linha, int coluna, bool ligado) {
  for (int display = 0; display < NUM_DISPLAYS; display++) {
    if (ligado) {
      lc.setLed(display, linha, coluna, true);
    } else {
      lc.setLed(display, linha, coluna, false);
    }
  }
}

void exibirNave() {
  if (linhaNave < -1) {
    linhaNave = -1;
  }
  
  if (linhaNave > 6) {
    linhaNave = 6;
  }

  setBitValue(NUM_DISPLAYS - 1, linhaNave, 7, linhaNave != -1);
  setBitValue(NUM_DISPLAYS - 1, linhaNave + 1, 7, true);
  setBitValue(NUM_DISPLAYS - 1, linhaNave + 2, 7, true);
  setBitValue(NUM_DISPLAYS - 1, linhaNave + 1, 6, true);
}

void limparNave() {
  setBitValue(NUM_DISPLAYS - 1, linhaNave, 7, false);
  setBitValue(NUM_DISPLAYS - 1, linhaNave + 1, 7, false);
  setBitValue(NUM_DISPLAYS - 1, linhaNave + 2, 7, false);
  setBitValue(NUM_DISPLAYS - 1, linhaNave + 1, 6, false);
}

void moverNaveCima() {
  limparNave();
  linhaNave--;
  exibirNave();
}

void moverNaveBaixo() {
  limparNave();
  linhaNave++;
  exibirNave();
}

void esquerda() {
  if (millis() - tempoNave > 200) {
    tempoNave = millis();
    moverNaveCima();
  }
}

void direita() {
  if (millis() - tempoNave > 200) {
    tempoNave = millis();
    moverNaveBaixo();
  }
}

void atirar() {
  if (millis() - tempoTiro > 200) {
    tempoTiro = millis();
    adicionarTiro(NUM_DISPLAYS - 1, linhaNave + 1, 5);
  }
}

void setup() {

  Serial.begin(9600);
  pinMode(KEY, INPUT);

  // Inicialização do objeto LedControl

  for (byte i = 0; i < NUM_DISPLAYS; i++) {
    lc.shutdown(i, false);       // Desativa o modo de economia de energia
    lc.setIntensity(i, 8);       // Define a intensidade do brilho (0-15)
    lc.clearDisplay(i);           // Limpa o display
  }

  // lc.setLed(display, linha, coluna, true);

  exibirNave();
  exibirMatrizes();

}

void loop() {
  int xValue = analogRead(X_AXIS);
  int YValue = analogRead(Y_AXIS);

  if (xValue < 200) {
    esquerda();
  }

  if (xValue > 800) {
    direita();
  }

  if (digitalRead(KEY) == 0) {
    atirar();
  }

  atualizarTiros();
  exibirMatrizes();

}




