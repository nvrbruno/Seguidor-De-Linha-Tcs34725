#include <Wire.h>
#include <Adafruit_TCS34725.h>

// ── Multiplexador ──────────────────────────────────────────────────────────────
#define TCA_ADDR 0x70

// ── Pinos dos Motores ──────────────────────────────────────────────────────────
const int PIN_A1A = 3;
const int PIN_A1B = 5;
const int PIN_B1A = 6;
const int PIN_B1B = 9;

// ── Canais dos Sensores ────────────────────────────────────────────────────────
const int NUM_SENSORES    = 4;
const int CanalEsqExtremo = 1;
const int CanalEsq        = 0;
const int CanalDir        = 5;
const int CanalDirExtremo = 4;

// ── Configurações Gerais ───────────────────────────────────────────────────────
const int VELOCIDADE_BASE  = 180;
const int VELOCIDADE_CURVA = 160;
const int LIMIAR_PRETO     = 800; // Calibrar: abaixo = preto

// ── Configurações PID ──────────────────────────────────────────────────────────
const float Kp = 1.5;
const float Ki = 0.0;
const float Kd = 0.5;

float erroAnterior = 0;
float integral     = 0;

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

// ── TCA9548A ───────────────────────────────────────────────────────────────────
void tcaSelect(uint8_t canal) {
    Wire.beginTransmission(TCA_ADDR);
    Wire.write(1 << canal);
    Wire.endTransmission();
}

// ── Leitura dos Sensores ───────────────────────────────────────────────────────
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
    return c < LIMIAR_PRETO;
}

// ── Controle de Motores ────────────────────────────────────────────────────────
// Faixa: -255 a 255. Negativo = ré. Uso: setMotores(180, 100);
void setMotores(int velocidadeA, int velocidadeB) {
    if (velocidadeA >= 0) {
        analogWrite(PIN_A1A, velocidadeA);
        analogWrite(PIN_A1B, 0);
    } else {
        analogWrite(PIN_A1A, 0);
        analogWrite(PIN_A1B, -velocidadeA);
    }

    if (velocidadeB >= 0) {
        analogWrite(PIN_B1A, velocidadeB);
        analogWrite(PIN_B1B, 0);
    } else {
        analogWrite(PIN_B1A, 0);
        analogWrite(PIN_B1B, -velocidadeB);
    }
}

void pararMotores() {
    setMotores(0, 0);
}

// ── Curvas Extremas ────────────────────────────────────────────────────────────
// Gira à esquerda até SensorEsq ver preto. Trocar para CanalDir se necessário.
void curvaExtremaEsquerda() {
    uint16_t r, g, b, c;
    do {
        setMotores(-VELOCIDADE_CURVA, VELOCIDADE_CURVA);
        tcaSelect(CanalEsq);
        SensorEsq.getRawData(&r, &g, &b, &c);
    } while (!ePreto(c));
}

// Gira à direita até SensorDir ver preto. Trocar para CanalEsq se necessário.
void curvaExtremaDireita() {
    uint16_t r, g, b, c;
    do {
        setMotores(VELOCIDADE_CURVA, -VELOCIDADE_CURVA);
        tcaSelect(CanalDir);
        SensorDir.getRawData(&r, &g, &b, &c);
    } while (!ePreto(c));
}

// ── PID ────────────────────────────────────────────────────────────────────────
// Ajusta os motores com base no erro entre os sensores centrais. Uso: aplicarPID(s.cEsq, s.cDir);
void aplicarPID(uint16_t cEsq, uint16_t cDir) {
    float erro     = (float)cEsq - (float)cDir;
    integral      += erro;
    float derivada = erro - erroAnterior;
    erroAnterior   = erro;

    float saida = (Kp * erro) + (Ki * integral) + (Kd * derivada);
    saida = constrain(saida, -255, 255);

    int velA = constrain(VELOCIDADE_BASE + (int)saida, 0, 255);
    int velB = constrain(VELOCIDADE_BASE - (int)saida, 0, 255);

    setMotores(velA, velB);
}

// ── Setup ──────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(PIN_A1A, OUTPUT);
    pinMode(PIN_A1B, OUTPUT);
    pinMode(PIN_B1A, OUTPUT);
    pinMode(PIN_B1B, OUTPUT);

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
    LeituraSensores s = lerSensores();

    bool esqExtremoPreto = ePreto(s.cEsqExtremo);
    bool esqPreto        = ePreto(s.cEsq);
    bool dirPreto        = ePreto(s.cDir);
    bool dirExtremoPreto = ePreto(s.cDirExtremo);

    if (esqExtremoPreto && esqPreto && dirPreto && dirExtremoPreto) {
        setMotores(VELOCIDADE_BASE, VELOCIDADE_BASE);
        return;
    }

    if (esqExtremoPreto) {
        curvaExtremaEsquerda();
        return;
    }

    if (dirExtremoPreto) {
        curvaExtremaDireita();
        return;
    }

    aplicarPID(s.cEsq, s.cDir);
}