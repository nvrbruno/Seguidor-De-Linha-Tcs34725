// Pinos PWM do Arduino (compatível com Uno/Mega)
const int PIN_A1A = 3;   // Motor A - Direção 1 (PWM)
const int PIN_A1B = 5;   // Motor A - Direção 2 (PWM)
const int PIN_B1A = 6;   // Motor B - Direção 1 (PWM)
const int PIN_B1B = 9;   // Motor B - Direção 2 (PWM)

// Configurações de velocidade
const int velocidade = 200;        // Velocidade padrão (0-255)
const int tempoInversao = 150;     // Tempo de inversão em ms

void setup() {
  // Configura os pinos como saída
  pinMode(PIN_A1A, OUTPUT);
  pinMode(PIN_A1B, OUTPUT);
  pinMode(PIN_B1A, OUTPUT);
  pinMode(PIN_B1B, OUTPUT);
  
  // Inicializa os motores parados
  pararMotores();
}

void loop() {
  // Sequência de teste dos movimentos
  moverFrente(velocidade);
  delay(1000);

  pararMotores();
  delay(500);

  virarEsquerda(velocidade);
  delay(1000);

  pararMotores();
  delay(500);

  virarDireita(velocidade);
  delay(1000);

  pararMotores();
  delay(500);

  moverTras(velocidade);
  delay(1000);

  pararMotores();
  delay(2000);
}

// Função: Mover para frente
void moverFrente(int v) {
  analogWrite(PIN_A1A, v);
  analogWrite(PIN_A1B, 0);

  analogWrite(PIN_B1A, v);
  analogWrite(PIN_B1B, 0);
}

// Função: Mover para trás
void moverTras(int v) {
  analogWrite(PIN_A1A, 0);
  analogWrite(PIN_A1B, v);

  analogWrite(PIN_B1A, 0);
  analogWrite(PIN_B1B, v);
}

// Função: Virar à esquerda (com inversão inicial)
void virarEsquerda(int v) {
  // Inversão inicial (ambos motores para trás)
  analogWrite(PIN_A1A, 0);
  analogWrite(PIN_A1B, v);
  analogWrite(PIN_B1A, 0);
  analogWrite(PIN_B1B, v);
  delay(tempoInversao);

  // Curva à esquerda (Motor A para trás, Motor B para frente)
  analogWrite(PIN_A1A, 0);
  analogWrite(PIN_A1B, v);
  analogWrite(PIN_B1A, v);
  analogWrite(PIN_B1B, 0);
}

// Função: Virar à direita (com inversão inicial)
void virarDireita(int v) {
  // Inversão inicial (ambos motores para trás)
  analogWrite(PIN_A1A, 0);
  analogWrite(PIN_A1B, v);
  analogWrite(PIN_B1A, 0);
  analogWrite(PIN_B1B, v);
  delay(tempoInversao);

  // Curva à direita (Motor A para frente, Motor B para trás)
  analogWrite(PIN_A1A, v);
  analogWrite(PIN_A1B, 0);
  analogWrite(PIN_B1A, 0);
  analogWrite(PIN_B1B, v);
}

// Função: Parar os motores
void pararMotores() {
  analogWrite(PIN_A1A, 0);
  analogWrite(PIN_A1B, 0);
  analogWrite(PIN_B1A, 0);
  analogWrite(PIN_B1B, 0);
}