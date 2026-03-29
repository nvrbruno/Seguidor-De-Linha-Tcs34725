#include <Wire.h>
#include <Adafruit_TCS34725.h>

const uint8_t TCA_ADDR = 0x70;
const int LIMIAR_PRETO = 200;
const int LIMIAR_PRATA = 1200;
const int NUM_SENSORES = 4;

const int CanalEsqExtremo = 3; 
const int CanalEsq = 2;
const int CanalDir = 1;
const int CanalDirExtremo = 0;

Adafruit_TCS34725 SensorEsqExtremo(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorEsq(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorDir(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 SensorDirExtremo(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

Adafruit_TCS34725* Sensores[NUM_SENSORES] = {&SensorEsqExtremo, &SensorEsq, &SensorDir, &SensorDirExtremo};
uint8_t Canais[NUM_SENSORES] = {CanalEsqExtremo, CanalEsq, CanalDir, CanalDirExtremo};
const char* NomesSensores[NUM_SENSORES] = {"EsqExtremo", "ESQ", "DIR", "DirExtremo"};

void tcaSelect(uint8_t canal) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << canal);
  Wire.endTransmission();
}

String classificarCor(uint16_t r, uint16_t g, uint16_t b, uint16_t c) {
  if (c < LIMIAR_PRETO) return "PRETO";
  if (c > LIMIAR_PRATA) return "BRANCO";
  
  if (g > r && g > b) return "VERDE";
  if (r > g && r > b) return "VERMELHO";
  
  return "INDEFINIDO";
}

void processarSensor(uint8_t indice) {
  tcaSelect(Canais[indice]);
  
  uint16_t r, g, b, c;
  Sensores[indice]->getRawData(&r, &g, &b, &c);
  
  String cor = classificarCor(r, g, b, c);
  
  Serial.print(NomesSensores[indice]);
  Serial.print(" | R:");
  Serial.print(r);
  Serial.print(" G:");
  Serial.print(g);
  Serial.print(" B:");
  Serial.print(b);
  Serial.print(" C:");
  Serial.print(c);
  Serial.print(" | Cor: ");
  Serial.println(cor);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  for (uint8_t i = 0; i < NUM_SENSORES; i++) {
    tcaSelect(Canais[i]);
    if (!Sensores[i]->begin()) {
      Serial.print("Falha no sensor ");
      Serial.println(NomesSensores[i]);
    }
  }
}

void loop() {
  for (uint8_t i = 0; i < NUM_SENSORES; i++) {
    processarSensor(i);
  }
  Serial.println();
  delay(1000);
}