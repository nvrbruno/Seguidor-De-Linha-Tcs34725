// Movimenta 2 MOTORES DC usando a ponte H L298N.
//MOTOR B: POSITIVO OUT 3, NEGATIVO OUT 4
//MOTOR A: POSITIVO OUT 2, NEGATIVO OUT 1


// PINOS ESP32 DEVKIT 
const int ENA = 25;   // Motor A PWM
const int IN1 = 26;
const int IN2 = 27;

const int ENB = 13;   // Motor B PWM
const int IN3 = 14;
const int IN4 = 12;

int velocidade = 70; //Define a velocidade PWM dos motores

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

// Onde o robô movimenta para frente, tras, esquerda e direita, com delay de 3 segundos
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


// DEFINIÇÕES DE FUNÇÕES
void frente() {
  // A = motor esquerdo
  digitalWrite(IN1, LOW);   // esquerda frente
  digitalWrite(IN2, HIGH);

  // B = motor direito 
  digitalWrite(IN3, LOW);   // direita frente 
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}

void tras() {
  // A = motor esquerdo
  digitalWrite(IN1, HIGH);   // esquerda tras
  digitalWrite(IN2, LOW);

  // B = motor direito 
  digitalWrite(IN3, HIGH);   // direita tras 
  digitalWrite(IN4, LOW);

  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}

void esquerda() {
  // A = motor esquerdo
  digitalWrite(IN1, HIGH);   // esquerda tras
  digitalWrite(IN2, LOW);

  // B = motor direito 
  digitalWrite(IN3, LOW);   // direita frente 
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}

void direita() {
  // A = motor esquerdo
  digitalWrite(IN1, LOW);   // esquerda frente
  digitalWrite(IN2, HIGH);

  // B = motor direito 
  digitalWrite(IN3, HIGH);   // direita tras 
  digitalWrite(IN4, LOW);

  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}
