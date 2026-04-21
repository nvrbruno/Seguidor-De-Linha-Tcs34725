#include <Wire.h>
#include <Adafruit_TCS34725.h>

// ── Multiplexador ──────────────────────────────────────────────────────────────
#define TCA_ADDR 0x70

// ── Pinos dos Motores ──────────────────────────────────────────────────────────
const int PIN_A1A = 3;
const int PIN_A1B = 5;
const int PIN_B1A = 6;
const int PIN_B1B = 9;

// ── Pinos HC-SR04 ──────────────────────────────────────────────────────────────
const int TRIG_FRONTAL = 12;
const int ECHO_FRONTAL = 11;
const int TRIG_DIREITO = 7;
const int ECHO_DIREITO = 8;

// ── Canais dos Sensores ────────────────────────────────────────────────────────
const int NUM_SENSORES    = 4;
const int CanalEsqExtremo = 1;
const int CanalEsq        = 0;
const int CanalDir        = 5;
const int CanalDirExtremo = 4;

// ── Configurações Gerais ───────────────────────────────────────────────────────
const int   VELOCIDADE_BASE   = 200;
const int   VELOCIDADE_CURVA  = 200;
const int   TEMPO_INVERSAO    = 150;
const float DIST_OBSTACULO_CM = 15.0; // Calibrar
const int   TEMPO_5CM_MS      = 300;  // Calibrar
const int   TEMPO_90_GRAUS_MS = 400;  // Calibrar

// ── Instâncias dos Sensores ────────────────────────────────────────────────────
Adafruit_TCS34725 SensorEsqExtremo(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorEsq       (TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorDir       (TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorDirExtremo(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

Adafruit_TCS34725* Sensores[NUM_SENSORES] = {
    &SensorEsqExtremo, &SensorEsq, &SensorDir, &SensorDirExtremo
};
uint8_t Canais[NUM_SENSORES] = {
    CanalEsqExtremo, CanalEsq, CanalDir, CanalDirExtremo
};
const char* NomesSensores[NUM_SENSORES] = {
    "EsqExtremo", "ESQ", "DIR", "DirExtremo"
};

// ── Setup ──────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(PIN_A1A, OUTPUT);
    pinMode(PIN_A1B, OUTPUT);
    pinMode(PIN_B1A, OUTPUT);
    pinMode(PIN_B1B, OUTPUT);

    pinMode(TRIG_FRONTAL, OUTPUT);
    pinMode(ECHO_FRONTAL, INPUT);
    pinMode(TRIG_DIREITO, OUTPUT);
    pinMode(ECHO_DIREITO, INPUT);

    pararMotores();

    bool todosOK = false;
    while (!todosOK) {
        todosOK = true;
        for (uint8_t i = 0; i < NUM_SENSORES; i++) {
            tcaSelect(Canais[i]);
            if (!Sensores[i]->begin()) {
                Serial.print("Sensor ");
                Serial.print(NomesSensores[i]);
                Serial.println(" falhou.");
                todosOK = false;
            }
        }
        if (!todosOK) delay(1000);
    }

    Serial.println("Sensores OK.");
}

// ── Loop Principal ─────────────────────────────────────────────────────────────
void loop() {
    if (obstaculoFrontal()) {
        desviarObstaculo();
        return;
    }

    moverFrente(VELOCIDADE_BASE);
}

// ── TCA9548A ───────────────────────────────────────────────────────────────────
void tcaSelect(uint8_t canal) {
    Wire.beginTransmission(TCA_ADDR);
    Wire.write(1 << canal);
    Wire.endTransmission();
}

// ── Leitura dos Sensores de Cor ────────────────────────────────────────────────
struct LeituraSensores {
    uint16_t cEsqExtremo;
    uint16_t cEsq;
    uint16_t cDir;
    uint16_t cDirExtremo;
};

// Lê o canal C dos 4 sensores. Uso: LeituraSensores s = lerSensores();
LeituraSensores lerSensores() {
    LeituraSensores leitura;
    uint16_t r, g, b, c;

    tcaSelect(CanalEsqExtremo);
    SensorEsqExtremo.getRawData(&r, &g, &b, &c);
    leitura.cEsqExtremo = c;

    tcaSelect(CanalEsq);
    SensorEsq.getRawData(&r, &g, &b, &c);
    leitura.cEsq = c;

    tcaSelect(CanalDir);
    SensorDir.getRawData(&r, &g, &b, &c);
    leitura.cDir = c;

    tcaSelect(CanalDirExtremo);
    SensorDirExtremo.getRawData(&r, &g, &b, &c);
    leitura.cDirExtremo = c;

    return leitura;
}

// Retorna true se o valor C indicar preto. Uso: if (ePreto(s.cEsq)) { ... }
bool ePreto(uint16_t c) {
    return c < 800; // Calibrar
}

// ── Leitura Ultrassônico ───────────────────────────────────────────────────────
// Lê distância em cm de qualquer HC-SR04. Uso: lerDistancia(TRIG_FRONTAL, ECHO_FRONTAL);
float lerDistancia(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duracao = pulseIn(echoPin, HIGH, 30000);
    if (duracao == 0) return 400.0;

    float distancia = duracao * 0.0343 / 2.0;
    return (distancia > 400) ? 400.0 : distancia;
}

bool obstaculoFrontal() {
    return lerDistancia(TRIG_FRONTAL, ECHO_FRONTAL) < DIST_OBSTACULO_CM;
}

bool obstaculoDireito() {
    return lerDistancia(TRIG_DIREITO, ECHO_DIREITO) < DIST_OBSTACULO_CM;
}

// ── Controle de Motores ────────────────────────────────────────────────────────
void pararMotores() {
    analogWrite(PIN_A1A, 0);
    analogWrite(PIN_A1B, 0);
    analogWrite(PIN_B1A, 0);
    analogWrite(PIN_B1B, 0);
}

void moverFrente(int v) {
    analogWrite(PIN_A1A, v);
    analogWrite(PIN_A1B, 0);
    analogWrite(PIN_B1A, v);
    analogWrite(PIN_B1B, 0);
}

void moverTras(int v) {
    analogWrite(PIN_A1A, 0);
    analogWrite(PIN_A1B, v);
    analogWrite(PIN_B1A, 0);
    analogWrite(PIN_B1B, v);
}

void virarEsquerda(int v) {
    analogWrite(PIN_A1A, 0);
    analogWrite(PIN_A1B, v);
    analogWrite(PIN_B1A, 0);
    analogWrite(PIN_B1B, v);
    delay(TEMPO_INVERSAO);

    analogWrite(PIN_A1A, 0);
    analogWrite(PIN_A1B, v);
    analogWrite(PIN_B1A, v);
    analogWrite(PIN_B1B, 0);
}

void virarDireita(int v) {
    analogWrite(PIN_A1A, 0);
    analogWrite(PIN_A1B, v);
    analogWrite(PIN_B1A, 0);
    analogWrite(PIN_B1B, v);
    delay(TEMPO_INVERSAO);

    analogWrite(PIN_A1A, v);
    analogWrite(PIN_A1B, 0);
    analogWrite(PIN_B1A, 0);
    analogWrite(PIN_B1B, v);
}

// ── Desvio de Obstáculo ────────────────────────────────────────────────────────
// Executa o desvio completo. Retorna ao loop() ao final.
void desviarObstaculo() {
    pararMotores();
    delay(300);

    virarEsquerda(VELOCIDADE_CURVA);
    delay(TEMPO_90_GRAUS_MS);
    pararMotores();

    while (!obstaculoDireito()) {
        moverFrente(VELOCIDADE_BASE);
    }

    moverFrente(VELOCIDADE_BASE);
    delay(TEMPO_5CM_MS);
    pararMotores();

    virarDireita(VELOCIDADE_CURVA);
    delay(TEMPO_90_GRAUS_MS);
    pararMotores();

    while (obstaculoDireito()) {
        moverFrente(VELOCIDADE_BASE);
    }

    moverFrente(VELOCIDADE_BASE);
    delay(TEMPO_5CM_MS);
    pararMotores();

    virarDireita(VELOCIDADE_CURVA);
    delay(TEMPO_90_GRAUS_MS);
    pararMotores();

    while (true) {
        moverFrente(VELOCIDADE_BASE);
        LeituraSensores s = lerSensores();
        if (ePreto(s.cEsqExtremo) || ePreto(s.cEsq)) break;
    }

    pararMotores();
    virarEsquerda(VELOCIDADE_CURVA);
    delay(TEMPO_90_GRAUS_MS);
    pararMotores();
}