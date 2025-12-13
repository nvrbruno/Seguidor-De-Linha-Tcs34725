  # 🤖 Robô Seguidor de Linha

  ## 📌 Visão Geral

  Este projeto apresenta um **robô seguidor de linha**, desenvolvido com base nos desafios da **RoboCup Junior – Rescue Line**.
  O sistema utiliza o **ESP32** como microcontrolador principal, responsável pelo processamento dos sensores e pelo controle dos motores, garantindo navegação autônoma, precisa e estável.

  ---

  ## ⚙️ Componentes Utilizados

  | Componente          | Descrição                                                                                                       |
  | ------------------- | --------------------------------------------------------------------------------------------------------------- |
  | **ESP32**           | Microcontrolador responsável pelo controle geral do robô.                                                       |
  | **4× TCS34725**     | Sensores de cor utilizados para leitura IR e RGB, permitindo a identificação da linha e de marcações coloridas. |
  | **TCA9548A**        | Multiplexador I²C para possibilitar o uso de múltiplos sensores TCS34725 no mesmo barramento.                   |
  | **HC-SR04**         | Sensor ultrassônico utilizado para detecção e desvio de obstáculos.                                             |
  | **Pilhas de Lítio** | Fonte de alimentação dos motores e da eletrônica.                                                               |
  | **Mini Protoboard** | Distribuição dos polos positivo e negativo entre os componentes.                                                |

  ---

  ## 🧠 Estratégia de Controle

  O robô utiliza uma **lógica de controle híbrida**, combinando controle contínuo com decisões condicionais.

  ### 🔁 Controle PID

  * Aplicado aos **sensores centrais (ESQ e DIR)**.
  * Mantém o robô alinhado à linha, reduzindo oscilações.
  * Garante movimento suave em retas e curvas.
  * Responsável pela identificação das cores **verde** e **vermelha**, acionando regras específicas:

    * Curvas de **90°**
    * Curvas de **180°**
    * Parada do robô
    * Entrada no **modo resgate**

  ### 🔀 Lógica Condicional (If / Else)

  * Utilizada pelos **sensores extremos**.
  * Executa correções rápidas e agressivas em situações como:

    * Curvas fechadas
    * Intersecções
    * Perda momentânea da linha

  ---

  ## ⚠️ Desvio de Obstáculos

  * O **sensor ultrassônico HC-SR04** monitora continuamente a distância à frente do robô.
  * Ao detectar um obstáculo, o sistema ativa automaticamente uma **rotina de desvio**.
  * Após contornar o obstáculo, o robô retorna ao seguimento da linha.

  ---

  ## 🏁 Objetivo do Projeto

  Desenvolver um robô capaz de:

  * Seguir linhas com precisão
  * Identificar cores e marcações especiais
  * Executar curvas complexas
  * Desviar de obstáculos
  * Atuar de forma autônoma em ambientes inspirados na **RoboCup Junior Rescue Line**

## 📚 Bibliotecas Utilizadas

| Biblioteca | Finalidade |
| :--- | :--- |
| **Wire.h** | Comunicação I²C entre o ESP32, o TCA9548A e os sensores TCS34725. |
| **Adafruit_TCS34725.h** | Interface de alto nível para leitura RGB e Clear dos sensores TCS34725. |

---

## 🔧 Detalhes de Uso

### Wire.h

* Inicializa e gerencia o barramento I²C.
* Necessária para seleção de canais no TCA9548A (Multiplexador I²C).
* Permite o uso de múltiplos sensores com o mesmo endereço I²C via multiplexação.

### Adafruit_TCS34725.h

* Abstrai configuração de ganho, tempo de integração e leitura de cores (RGB e Clear).
* Utilizada para detecção de linha (Clear/IR) e identificação de cores (RGB).
* Funciona em conjunto com o multiplexador para leitura sequencial dos 4 sensores.

---

## 📍 Endereço I²C

```cpp
#define TCA_ADDR 0x70
```
* Endereço padrão do TCA9548A.
* Usado para alternar entre os canais conectados aos sensores TCS34725.