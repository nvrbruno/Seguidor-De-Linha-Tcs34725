#include <Wire.h>
#include <Adafruit_TCS34725.h>

#define TCA_ADDR 0x70

// Constante para o número de sensores
const int NUM_SENSORES = 4;

// Pinos do HC-SR04 adaptados para Arduino
const int TRIG_PIN = 12; // Pino digital 12 para Trigger
const int ECHO_PIN = 11; // Pino digital 11 para Echo

// Canais do TCA9548A - Nomenclatura Padronizada
const int CanalEsqExtremo = 1; 
const int CanalEsq = 0;
const int CanalDir = 5;
const int CanalDirExtremo = 4;

// Sensores individuais - Nomenclatura Padronizada
Adafruit_TCS34725 SensorEsqExtremo(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorEsq(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorDir(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorDirExtremo(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Array para inicialização simplificada
// Ordem: [EsqExtremo, Esq, Dir, DirExtremo]
Adafruit_TCS34725* Sensores[NUM_SENSORES] = {&SensorEsqExtremo, &SensorEsq, &SensorDir, &SensorDirExtremo};
uint8_t Canais[NUM_SENSORES] = {CanalEsqExtremo, CanalEsq, CanalDir, CanalDirExtremo};

// Array de nomes para debug correspondente à ordem acima
const char* NomesSensores[NUM_SENSORES] = {"EsqExtremo", "ESQ", "DIR", "DirExtremo"};

void tcaSelect(uint8_t canal) {
    Wire.beginTransmission(TCA_ADDR);
    Wire.write(1 << canal);
    Wire.endTransmission();
}

// FUNÇÃO DE LEITURA DE DISTÂNCIA
float lerDistanciaCM() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

    if (duracao == 0) {
        return 0.0; 
    }

    float distancia = duracao * 0.0343 / 2.0;
    
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
        for (uint8_t i = 0; i < NUM_SENSORES; i++) {
            tcaSelect(Canais[i]);
            if (!Sensores[i]->begin()) {
                // Log que identifica o sensor falhado
                Serial.print("Sensor ");
                Serial.print(NomesSensores[i]);
                Serial.print(" (Canal ");
                Serial.print(Canais[i]);
                Serial.println(") falhou na inicializacao.");
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
    float distancia_cm;
    // Variáveis para acumular a soma de cada componente dos 4 sensores
    long soma_r = 0, soma_g = 0, soma_b = 0, soma_c = 0;
    
    // Leitura do Ultrassônico
    distancia_cm = lerDistanciaCM();
    Serial.print("Ultrassonico -> Distancia: ");
    Serial.print(distancia_cm, 2);
    Serial.println(" cm");
    
    // --- 1. LEITURA E SOMA DOS DADOS DE TODOS OS SENSORES ---
    for (int i = 0; i < NUM_SENSORES; i++) {
        uint16_t r, g, b, c;
        tcaSelect(Canais[i]);
        Sensores[i]->getRawData(&r, &g, &b, &c);
        
        // Impressão individual (para debug)
        Serial.print(NomesSensores[i]);
        Serial.print(" -> R:");
        Serial.print(r);
        Serial.print(" G:");
        Serial.print(g);
        Serial.print(" B:");
        Serial.print(b);
        Serial.print(" C:");
        Serial.println(c);

        // Soma para o cálculo da média
        soma_r += r;
        soma_g += g;
        soma_b += b;
        soma_c += c;
    }
    
    // --- 2. CÁLCULO DAS MÉDIAS POR COMPONENTE ---
    // Faz a conversão (float) para garantir que o resultado tenha casas decimais
    float media_r = (float)soma_r / NUM_SENSORES;
    float media_g = (float)soma_g / NUM_SENSORES;
    float media_b = (float)soma_b / NUM_SENSORES;
    float media_c = (float)soma_c / NUM_SENSORES;

    // --- 3. IMPRESSÃO DOS RESULTADOS FINAIS ---
    Serial.println("MEDIAS GLOBAIS DOS SENSORES");
    Serial.print("Media R: ");
    Serial.println(media_r, 2);
    Serial.print("Media G: ");
    Serial.println(media_g, 2);
    Serial.print("Media B: ");
    Serial.println(media_b, 2);
    Serial.print("Media C: ");
    Serial.println(media_c, 2);
    Serial.println("-----------------------");

    delay(500);
}