#include <Wire.h>
#include <Adafruit_TCS34725.h>

#define TCA_ADDR 0x70

// Pinos do ESP32 para I2C
//const int SDA = 21;
//const int SCL = 22;

const int TRIG_PIN = 14; // GPIO para o Trigger
const int ECHO_PIN = 12; // GPIO para o Echo

// Canais do TCA9548A - Nomenclatura Padronizada
const int CanalDirExtremo = 4;
const int CanalDir = 5;
const int CanalEsq = 0;
const int CanalEsqExtremo = 1;

// Sensores individuais - Nomenclatura Padronizada
Adafruit_TCS34725 SensorEsqExtremo(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorEsq(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorDir(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorDirExtremo(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Array para inicialização simplificada
Adafruit_TCS34725* Sensores[] = {&SensorEsqExtremo, &SensorEsq, &SensorDir, &SensorDirExtremo};
uint8_t Canais[] = {CanalEsqExtremo, CanalEsq, CanalDir, CanalDirExtremo};

// Array de nomes para debug correspondente à ordem acima
const char* NomesSensores[] = {"EsqExtremo", "ESQ", "DIR", "DirExtremo"};

void tcaSelect(uint8_t canal) {
    Wire.beginTransmission(TCA_ADDR);
    Wire.write(1 << canal);
    Wire.endTransmission();
}

/**
 * @brief Lê a distância em centímetros do sensor ultrassônico HC-SR04.
 * * @return float Distância em centímetros. Retorna 0.0 se o pulso for inválido.
 */
float lerDistanciaCM() {
    // Limpa o pino Trigger
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Configura o pino Trigger para HIGH por 10 microsegundos
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Lê a duração do pulso no pino Echo
    // Timeout de 30000 µs (30 ms) para evitar travamento se não houver objeto
    long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

    // Se duracao for 0, o timeout ocorreu
    if (duracao == 0) {
        return 0.0; 
    }

    // Calcula a distância: (duração do pulso em µs) * (velocidade do som: 0.0343 cm/µs) / 2
    // Velocidade do som a 20°C é aproximadamente 343 m/s, ou 0.0343 cm/µs.
    // Divisão por 2 porque o pulso percorre a distância de ida e volta.
    float distancia = duracao * 0.0343 / 2.0;
    
    // Distância máxima do HC-SR04 é ~400 cm.
    if (distancia > 400) {
        return 400.0; 
    }

    return distancia;
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Inicialização do Ultrassônico
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    bool todosOK = false;

    // Loop de inicialização com verificação individual
    while (!todosOK) {
        todosOK = true;
        for (uint8_t i = 0; i < 4; i++) {
            tcaSelect(Canais[i]);
            if (!Sensores[i]->begin()) {
                // Log que identifica o sensor falhado
                Serial.printf("Sensor %s (Canal %d) falhou na inicialização.\n", NomesSensores[i], Canais[i]);
                todosOK = false;
            }
        }

        // Log de erro geral
        if (!todosOK) {
            Serial.println("Erro: nem todos os sensores inicializaram.");
            delay(1000);
        }
    }

    Serial.println("Todos os sensores I2C inicializados!");
}

void loop() {
    uint16_t r, g, b, c;
    float distancia_cm;

    // Leitura do Ultrassônico
    distancia_cm = lerDistanciaCM();
    Serial.printf("Ultrassonico -> Distancia: %.2f cm\n", distancia_cm);
    
    // Leitura individualizada dos Sensores de Cor
    tcaSelect(CanalEsqExtremo);
    SensorEsqExtremo.getRawData(&r, &g, &b, &c);
    Serial.printf("EsqExtremo -> R:%d G:%d B:%d C:%d\n", r, g, b, c);

    tcaSelect(CanalEsq);
    SensorEsq.getRawData(&r, &g, &b, &c);
    Serial.printf("ESQ -> R:%d G:%d B:%d C:%d\n", r, g, b, c);

    tcaSelect(CanalDir);
    SensorDir.getRawData(&r, &g, &b, &c);
    Serial.printf("DIR -> R:%d G:%d B:%d C:%d\n", r, g, b, c);

    tcaSelect(CanalDirExtremo);
    SensorDirExtremo.getRawData(&r, &g, &b, &c);
    Serial.printf("DirExtremo -> R:%d G:%d B:%d C:%d\n", r, g, b, c);

    Serial.println("-----------------------");
    delay(500);
}