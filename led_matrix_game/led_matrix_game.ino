/**
 * LED Matrix Game
 * Developed by: Ricardo Teixeira
 * Repository: https://github.com/ricardoteix/led_matrix_game
 * 
 * All rights reserved. 
 * 
*/

// https://wayoda.github.io/LedControl/
#include <LedControl.h>

// Shield Joystick
#define X_AXIS A0
#define Y_AXIS A1
#define KEY_A 2
#define KEY_B 3
#define KEY_C 4
#define KEY_D 5

#define BUZZER 7

// Definição dos pinos para conexão com o MAX7219
#define DATA_IN_PIN   9   // Pino de dados (DIN)
#define CLK_PIN       10  // Pino do clock (CLK)
#define LOAD_PIN      11  // Pino de carga (CS)
#define NUM_DISPLAYS  4   // Número de matrizes de LED no display

LedControl lc = LedControl(DATA_IN_PIN, CLK_PIN, LOAD_PIN, NUM_DISPLAYS);

// Inicia com o texto PRESS C
int displayBits[][8] = {
  {
    B00000000,
    B00111100,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00111100,
    B00000000
  },
  {
    B00000000,
    B11011100,
    B00010000,
    B00010000,
    B11011100,
    B01000100,
    B11011100,
    B00000000
  },
  {
    B00000000,
    B10011101,
    B01010001,
    B01010001,
    B10011101,
    B01010000,
    B01011101,
    B00000000
  },
  {
    B00000000,
    B01110011,
    B01001010,
    B01001010,
    B01110011,
    B01000010,
    B01000010,
    B00000000
  }
};

bool pause = false;
bool start = false;
bool fim = false;
byte animaFimFrame = 0;
byte animaFimDisplay = 3;
unsigned long tempoAnimaFim = millis();
int linhaNave = 0;
unsigned long tempoNave = millis();
unsigned long tempoTiro = millis();
unsigned long tempoAtualizarTiro = millis();
unsigned long tempoNascerInimigo = millis();
unsigned long tempoMoverInimigo = millis();

int tempoMoverInimigoEspera = 200;
int tempoNascerInimigoEspera = 2000;

// Definição da estrutura para representar um objeto com x, y e display
struct Tiro {
  byte display = 255;
  byte x = 255;
  byte y = 255;
};

// Definição do inimigo para representar um objeto com x, y e display
struct Inimigo {
  byte display = 255;
  byte x = 255;
  byte y = 255;
  byte bits[2] = { B10, B01 };  
};

byte barraProgresso = B00000000;
byte contagemAcertos = 0;

Tiro tiros[8]; // Máximo de 8 tipos simultâneos, 1 por coluna
Inimigo inimigos[3]; // Máximo de 3 inimigos simultâneos em tela
byte ultimoInimigo = 0;

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

void verificarProgresso() {
  
  contagemAcertos++;
  
  barraProgresso |= 1 << (contagemAcertos) - 1;

  if (barraProgresso == B11111111) {
    fim = true;
  }
  
}

void verificarAcerto() {
  
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 3; j++) {

        byte displayTiro = tiros[i].display;
        byte displayInimigo = inimigos[j].display;

        if (displayTiro == 255 || displayInimigo == 255) {
          continue;
        }

        if (
          ((tiros[i].x == inimigos[j].x) || (tiros[i].x == inimigos[j].x - 1)) &&
          ((tiros[i].y == inimigos[j].y - 1) || (tiros[i].y == inimigos[j].y - 2)) &&
          (displayTiro == displayInimigo)) {
          
          // Limpa
          setBitValue(tiros[i].display, tiros[i].x, tiros[i].y - 1, false);
          setBitValue(tiros[i].display, tiros[i].x, tiros[i].y - 2, false);
          setBitValue(tiros[i].display, tiros[i].x, tiros[i].y, false);
          setBitValue(tiros[i].display, tiros[i].x, tiros[i].y + 1, false);

          if (tiros[i].x < inimigos[j].x) {
            setBitValue(tiros[i].display, tiros[i].x + 1, tiros[i].y - 1, false);
            setBitValue(tiros[i].display, tiros[i].x + 1, tiros[i].y - 2, false);
            setBitValue(tiros[i].display, tiros[i].x + 1, tiros[i].y, false);
            setBitValue(tiros[i].display, tiros[i].x + 1, tiros[i].y + 1, false);
          } 

          if (tiros[i].x == inimigos[j].x) {
            setBitValue(tiros[i].display, tiros[i].x - 1, tiros[i].y - 1, false);
            setBitValue(tiros[i].display, tiros[i].x - 1, tiros[i].y - 2, false);
            setBitValue(tiros[i].display, tiros[i].x - 1, tiros[i].y, false);
            setBitValue(tiros[i].display, tiros[i].x - 1, tiros[i].y + 1, false);
          } 
          // Fim Limpa  

          tiros[i].display = 255;
          tiros[i].x = 255;
          tiros[i].y = 255;
          inimigos[j].display = 255;
          inimigos[j].x = 255;
          inimigos[j].y = 255;

          setBitValue(tiros[i].display, tiros[i].x, tiros[i].y, false);
          
          tone(BUZZER, 1200, 100);

          // TODO Finalizar a barra de progresso
          verificarProgresso();
          
        }
    }
  }
}

void adicionarInimigo(bool semEspera = false, bool atualizarPosicao = true) {

  byte posicao = (byte)random(1, 7);

  if (inimigos[ultimoInimigo].display != 255) {
    return;
  }

  if (millis() - tempoNascerInimigo > tempoNascerInimigoEspera || semEspera) {
    tempoNascerInimigo = millis();
    inimigos[ultimoInimigo].display = 0;
    inimigos[ultimoInimigo].x = posicao;
    inimigos[ultimoInimigo].y = ultimoInimigo;
    
    if (atualizarPosicao) {
      ultimoInimigo = ++ultimoInimigo % 3;
    } 
  }
}

void atualizarInimigos() {
  if (millis() - tempoMoverInimigo > tempoMoverInimigoEspera) {
    tempoMoverInimigo = millis();

    for (int i = 0; i < 3; i++) {
      
      if (inimigos[i].display == 255) {
        continue;
      }

      if (inimigos[i].y == 0 && inimigos[i].display > 0) {
        setBitValue(inimigos[i].display - 1, inimigos[i].x, 6, false);
        setBitValue(inimigos[i].display - 1, inimigos[i].x - 1, 6, false);
      } else if (inimigos[i].y == 1 && inimigos[i].display > 0) {
        setBitValue(inimigos[i].display - 1, inimigos[i].x, 7, false);
        setBitValue(inimigos[i].display - 1, inimigos[i].x - 1, 7, false);
      } else {
        setBitValue(inimigos[i].display, inimigos[i].x, inimigos[i].y - 2, false);
        setBitValue(inimigos[i].display, inimigos[i].x - 1, inimigos[i].y - 2, false);
      }
      
      int mode = inimigos[i].y % 2 == 0;
      setBitValue(inimigos[i].display, inimigos[i].x, inimigos[i].y, bitRead(inimigos[i].bits[mode], 0));
      setBitValue(inimigos[i].display, inimigos[i].x - 1, inimigos[i].y, bitRead(inimigos[i].bits[mode], 1));
      setBitValue(inimigos[i].display, inimigos[i].x, inimigos[i].y - 1, bitRead(inimigos[i].bits[!mode], 0));
      setBitValue(inimigos[i].display, inimigos[i].x - 1, inimigos[i].y - 1, bitRead(inimigos[i].bits[!mode], 1));

      if (inimigos[i].y == 7) {
        inimigos[i].y = 0;
        inimigos[i].display++;
      } else {
        inimigos[i].y++;
      }

      if (inimigos[i].display == 3 && inimigos[i].y > 6) {

        setBitValue(inimigos[i].display, inimigos[i].x, inimigos[i].y, 0);
        setBitValue(inimigos[i].display, inimigos[i].x - 1, inimigos[i].y, 0);
        setBitValue(inimigos[i].display, inimigos[i].x, inimigos[i].y - 1, 0);
        setBitValue(inimigos[i].display, inimigos[i].x - 1, inimigos[i].y - 1, 0);

        setBitValue(inimigos[i].display, inimigos[i].x, inimigos[i].y, 0);
        setBitValue(inimigos[i].display, inimigos[i].x - 1, inimigos[i].y, 0);
        setBitValue(inimigos[i].display, inimigos[i].x, inimigos[i].y - 2, 0);
        setBitValue(inimigos[i].display, inimigos[i].x - 1, inimigos[i].y - 2, 0);

        inimigos[i].display = 255;
        inimigos[i].x = 255;
        inimigos[i].y = 255;

        tone(BUZZER, 500, 200);
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

void exibirMatrizes(bool limpar = false) {

  // Exibe a linha da barra de progresso
  for (int c = 0; c < 8; c++) {
    setBitValue(0, c, 0, bitRead(barraProgresso, c));
  }

  // Desenha toda a matriz
  for (byte d = 0; d < NUM_DISPLAYS; d++) {
    for (byte i = 0; i < 8; i++) {
      if (limpar) {
        displayBits[d][i] = B00000000;
      }
      lc.setRow(d, i, displayBits[d][i]); 
    }
  }
}

void exibirNave(bool limpar = false) {
  if (linhaNave < -1) {
    linhaNave = -1;
  }
  
  if (linhaNave > 6) {
    linhaNave = 6;
  }

  // Não desenha na linha do display anterior
  setBitValue(NUM_DISPLAYS - 1, linhaNave, 7, linhaNave != -1 && !limpar);

  setBitValue(NUM_DISPLAYS - 1, linhaNave + 1, 7, !limpar);
  setBitValue(NUM_DISPLAYS - 1, linhaNave + 2, 7, !limpar);
  setBitValue(NUM_DISPLAYS - 1, linhaNave + 1, 6, !limpar);
}

void moverNaveCima() {
  exibirNave(true);
  linhaNave--;
  exibirNave();
}

void moverNaveBaixo() {
  exibirNave(true);
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

void fimJogo() {
  
  displayBits[0][0] = B00000000;
  barraProgresso = 0;
  int tempoMover = 200;
  
  if (millis() - tempoAnimaFim > tempoMover && animaFimFrame == 0) {
    exibirMatrizes(true);
    displayBits[animaFimDisplay][2] = B10000000;
    displayBits[animaFimDisplay][3] = B10000000;
    displayBits[animaFimDisplay][4] = B10000000;
    displayBits[animaFimDisplay][3] = B11000000; 
    exibirMatrizes();
    // delay(tempoMover);
    tempoAnimaFim = millis();
    animaFimFrame++;
  }
  
  if (millis() - tempoAnimaFim > tempoMover && animaFimFrame == 1) {
    displayBits[animaFimDisplay][2] = B10000000;
    displayBits[animaFimDisplay][3] = B00000000;
    displayBits[animaFimDisplay][4] = B00000000;
    displayBits[animaFimDisplay][3] = B00000000; 

    displayBits[animaFimDisplay][2] = B01110000;
    displayBits[animaFimDisplay][3] = B00110000;
    displayBits[animaFimDisplay][4] = B00010000;
    exibirMatrizes();
    // delay(tempoMover);
    tempoAnimaFim = millis();
    animaFimFrame++;
  }
  
  if (millis() - tempoAnimaFim > tempoMover && animaFimFrame == 2) {
    tempoMover = millis();
    displayBits[animaFimDisplay][2] = B00000000;
    displayBits[animaFimDisplay][3] = B00000000;
    displayBits[animaFimDisplay][4] = B00000000;

    displayBits[animaFimDisplay][0] = B00100000;
    displayBits[animaFimDisplay][1] = B01110000;
    exibirMatrizes();
    // delay(tempoMover);
    tempoAnimaFim = millis();
    animaFimFrame++;
  }

  if (millis() - tempoAnimaFim > tempoMover && animaFimFrame == 3) {
    displayBits[animaFimDisplay][1] = B00000000;
    displayBits[animaFimDisplay][0] = B01110000;
    exibirMatrizes();
    //delay(tempoMover);
    tempoAnimaFim = millis();
    animaFimFrame++;
  }

  if (millis() - tempoAnimaFim > tempoMover && animaFimFrame == 4) {
    displayBits[animaFimDisplay][0] = B00000000;
    exibirMatrizes();
    // delay(tempoMover * 3);
    tempoAnimaFim = millis();
    animaFimFrame++;
    animaFimDisplay--;
  
    if (animaFimDisplay == 255) {
      animaFimDisplay = 3;
    };
  }
  
  if (millis() - tempoAnimaFim > tempoMover && animaFimFrame == 5) {
    displayBits[animaFimDisplay][3] = B10000000;
    exibirMatrizes();
    // delay(tempoMover);
    tempoAnimaFim = millis();
    animaFimFrame = 0;
  }
}

void setup() {

  Serial.begin(9600);

  pinMode(KEY_A, INPUT);
  pinMode(KEY_B, INPUT);
  pinMode(KEY_C, INPUT);
  pinMode(KEY_D, INPUT);
  pinMode(BUZZER, OUTPUT);

  randomSeed(analogRead(5));

  // Inicialização do objeto LedControl
  for (byte i = 0; i < NUM_DISPLAYS; i++) {
    lc.shutdown(i, false);  // Desativa o modo de economia de energia
    lc.setIntensity(i, 2);  // Define a intensidade do brilho (0-15)
    lc.clearDisplay(i);     // Limpa o display
  }

}

void loop() {

  if (start) {
    int xValue = analogRead(X_AXIS);
    int YValue = analogRead(Y_AXIS);

    if (xValue < 200) {
      esquerda();
    }

    if (xValue > 800) {
      direita();
    }

    if (digitalRead(KEY_B) == 0) {
      atirar();
    }

    if (digitalRead(KEY_D) == 0) {
      pause = !pause;
      while(digitalRead(KEY_D) == 0);
    }
    
    if(!pause && !fim) {
      adicionarInimigo();
      atualizarInimigos();
      atualizarTiros();
      verificarAcerto();
      exibirNave();
    }

    if (fim) {
      fimJogo();
    }

  } else {
    if (digitalRead(KEY_C) == 0) { 
      start = true;
      exibirMatrizes(true);
      while(digitalRead(KEY_C) == 0);
    }
  }
  
  exibirMatrizes();
}




