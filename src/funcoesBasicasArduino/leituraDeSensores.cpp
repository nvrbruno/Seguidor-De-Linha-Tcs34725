#include <Wire.h>
#include <Adafruit_TCS34725.h>

#define TCA_ADDR 0x70

const int NUM_SENSORES = 4;

// ── Pinos Ultrassônicos ────────────────────────────────────────────────────────
const int TRIG_FRONTAL = 12;
const int ECHO_FRONTAL = 11;
const int TRIG_DIREITO = 8;  // Ajustar pino
const int ECHO_DIREITO = 7;  // Ajustar pino

// ── Canais TCA9548A ────────────────────────────────────────────────────────────
const int CanalEsqExtremo = 1;
const int CanalEsq        = 0;
const int CanalDir        = 5;
const int CanalDirExtremo = 4;

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

// ── Ultrassônico ───────────────────────────────────────────────────────────────
// Lê distância em cm de um par TRIG/ECHO. Uso: lerDistancia(TRIG_FRONTAL, ECHO_FRONTAL);
float lerDistancia(int trig, int echo) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    long duracao = pulseIn(echo, HIGH, 30000);
    if (duracao == 0) return 400.0;

    float distancia = duracao * 0.0343 / 2.0;
    return distancia > 400 ? 400.0 : distancia;
}

// ── Sensores de Cor ────────────────────────────────────────────────────────────
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

// ── Setup ──────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(TRIG_FRONTAL, OUTPUT);
    pinMode(ECHO_FRONTAL, INPUT);
    pinMode(TRIG_DIREITO, OUTPUT);
    pinMode(ECHO_DIREITO, INPUT);

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

// ── Loop ───────────────────────────────────────────────────────────────────────
void loop() {
    float distFrontal = lerDistancia(TRIG_FRONTAL, ECHO_FRONTAL);
    float distDireito = lerDistancia(TRIG_DIREITO, ECHO_DIREITO);

    Serial.print("Frontal: "); Serial.print(distFrontal, 2); Serial.println(" cm");
    Serial.print("Direito: "); Serial.print(distDireito, 2); Serial.println(" cm");

    LeituraSensores s = lerSensores();

    for (int i = 0; i < NUM_SENSORES; i++) {
        uint16_t r, g, b, c;
        tcaSelect(Canais[i]);
        Sensores[i]->getRawData(&r, &g, &b, &c);
        Serial.print(NomesSensores[i]);
        Serial.print(" -> R:"); Serial.print(r);
        Serial.print(" G:");    Serial.print(g);
        Serial.print(" B:");    Serial.print(b);
        Serial.print(" C:");    Serial.println(c);
    }

    Serial.println("-----------------------");
    delay(500);
}