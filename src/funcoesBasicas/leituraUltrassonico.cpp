#include <Wire.h>

// Pinos para o Sensor Ultrassônico (
const int TRIG_PIN = 14; // GPIO para o Trigger
const int ECHO_PIN = 12; // GPIO para o Echo

float lerDistanciaCM() {
    // Limpa o pino Trigger
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Configura o pino Trigger para HIGH por 10 microsegundos
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Lê a duração do pulso no pino Echo com timeout de 30 ms
    long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

    // Se duracao for 0, o timeout ocorreu
    if (duracao == 0) {
        return 0.0;
    }

    // Calcula a distância em cm: (duração do pulso em µs) * (velocidade do som em cm/µs) / 2
    // Velocidade do som: 0.0343 cm/µs
    float distancia = duracao * 0.0343 / 2.0;
    
    // Limita a 400 cm para valores fora do range
    if (distancia > 400) {
        return 400.0; 
    }

    return distancia;
}

void setup() {
    Serial.begin(115200);

    // Inicialização do Ultrassônico
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    Serial.println("Sensor Ultrassonico inicializado!");
}

void loop() {
    float distancia_cm = lerDistanciaCM();
    Serial.printf("Distancia: %.2f cm\n", distancia_cm);
    
    delay(500);
}