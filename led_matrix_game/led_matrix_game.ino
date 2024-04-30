/*
  Adicionar elementos que precisam ser destruídos com X tiros, se possível diminuindo o brilho até desaparecer
*/

#include <LedControl.h>

#define X_AXIS A0
#define Y_AXIS A1
#define KEY_A 2
#define KEY_B 3
#define KEY_C 4
#define KEY_D 5
#define BUZZER 7


// Definição dos pinos para conexão com o MAX7219
#define DATA_IN_PIN   9  // Pino de dados (DIN)
#define CLK_PIN       10  // Pino do clock (CLK)
#define LOAD_PIN      11  // Pino de carga (CS)

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

struct Inimigo {
  byte display = 255;
  byte x = 255;
  byte y = 255;
  byte bits[2] = { B10, B01 };  
};

byte barraProgresso = B00000000;
byte contagemAcertos = 0;

int quantidadeInimigosLevel = 5;

// Lista para armazenar os objetos LEDPosition
Tiro tiros[8]; // Tamanho da lista é 10 como exemplo
Inimigo inimigos[3];
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
  
  if (contagemAcertos == quantidadeInimigosLevel) {
    barraProgresso = B00000001;  
    tempoNascerInimigoEspera -= 200;   
    tempoMoverInimigoEspera -= 20;     
  }
  
  if (contagemAcertos > quantidadeInimigosLevel) {
    barraProgresso |= B00000001 << (int)(contagemAcertos / quantidadeInimigosLevel); 
    tempoNascerInimigoEspera -= 200;  
    tempoMoverInimigoEspera -= 20;      
  }
  
  contagemAcertos++;
    
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
          
          digitalWrite(BUZZER, 1);
          delay(150);
          digitalWrite(BUZZER, 0);
          
          verificarProgresso();

          // setBitValue(0, tiros[i].x, 0, false);
          // continue;
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
        Serial.println("Colisao!");
        // digitalWrite(BUZZER, 1);
        // delay(150);
        // digitalWrite(BUZZER, 0);
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

  for (int c = 0; c < 8; c++) {
    // displayBits[3][c] = bitRead(barraProgresso, c);
    setBitValue(0, c, 0, bitRead(barraProgresso, c));
  }

  for (byte d = 0; d < NUM_DISPLAYS; d++) {
    for (byte i = 0; i < 8; i++) {
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
    lc.shutdown(i, false);       // Desativa o modo de economia de energia
    lc.setIntensity(i, 8);       // Define a intensidade do brilho (0-15)
    lc.clearDisplay(i);           // Limpa o display
  }

  // lc.setLed(display, linha, coluna, true);
  // adicionarInimigo(true);
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

  if (digitalRead(KEY_B) == 0) {
    atirar();
  }

  if (digitalRead(KEY_A) == 0) {
    adicionarInimigo(true);
  }

  if (digitalRead(KEY_C) == 0 || true) { 
    exibirMatrizes();
    adicionarInimigo();
    atualizarInimigos();
    atualizarTiros();
    verificarAcerto();
    exibirNave();
    while(digitalRead(KEY_C) == 0);
  }
}




