#include <Wire.h>
#include <Adafruit_TCS34725.h>

#define TCA_ADDR 0x70

// Pinos do ESP32 para I2C
//const int SDA = 21;
//const int SCL = 22;

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

void setup() {
  Serial.begin(115200);
  Wire.begin();

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

  Serial.println("Todos os sensores inicializados!");
}

void loop() {
  uint16_t r, g, b, c;

  // Leitura individualizada 
  
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