// Controle de 2 motores DC utilizando a ponte H L298N
// MOTOR B: terminal positivo no OUT3 e negativo no OUT4
// MOTOR A: terminal positivo no OUT2 e negativo no OUT1

// Mapeamento dos pinos do ESP32 DevKit
const int ENA = 25;   // Pino PWM do Motor A (controle de velocidade)
const int IN1 = 26;   // Sentido de rotação do Motor A
const int IN2 = 27;   // Sentido de rotação do Motor A

const int ENB = 13;   // Pino PWM do Motor B (controle de velocidade)
const int IN3 = 14;   // Sentido de rotação do Motor B
const int IN4 = 12;   // Sentido de rotação do Motor B

int velocidade = 70;  // Valor de PWM aplicado aos motores (0 a 255)

void setup() {
  // Configura os pinos de controle dos motores como saída
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

// Executa movimentos básicos do robô em sequência,
// mantendo cada movimento por 3 segundos
void loop() {
  frente();
  delay(3000);
  tras();
  delay(3000);
  esquerda();
  delay(3000);
  direita();
  delay(3000);
}

// ===== DEFINIÇÃO DAS FUNÇÕES DE MOVIMENTO =====

void frente() {
  // Motor A (lado esquerdo) girando para frente
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Motor B (lado direito) girando para frente
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Aplica a velocidade definida aos dois motores
  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}

void tras() {
  // Motor A (lado esquerdo) girando para trás
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Motor B (lado direito) girando para trás
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Aplica a velocidade definida aos dois motores
  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}

void esquerda() {
  // Motor A (lado esquerdo) girando para trás
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Motor B (lado direito) girando para frente
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Aplica a velocidade definida aos dois motores
  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}

void direita() {
  // Motor A (lado esquerdo) girando para frente
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Motor B (lado direito) girando para trás
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Aplica a velocidade definida aos dois motores
  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}
