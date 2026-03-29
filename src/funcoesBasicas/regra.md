# 📌 Lógica de Controle – Seguidor de Linha com PID e Modos Especiais
---

## 1. Modo Principal — Seguidor de Linha (PID)

- Referência: **sensores centrais**
- Comportamento padrão: **seguir linha usando PID**

---

## 2. Detecção de Verde (Regra Base)

Ao detectar **verde em qualquer sensor**:

1. Parar motores  
2. Aguardar estabilização  
3. Confirmar leitura de verde  
4. Avançar um deslocamento fixo pequeno  
5. Ler sensores novamente  

---

## 3. Decisões Após o Avanço

### 3.1 Verde no meio direito + preto no extremo direito
- Virar à **direita** enquanto a condição persistir  
- Caso contrário: mover à frente  

### 3.2 Verde no meio esquerdo + preto no extremo esquerdo
- Virar à **esquerda** enquanto a condição persistir  
- Caso contrário: mover à frente  

### 3.3 Verde no meio esquerdo **e** direito + preto nos dois extremos
- Executar **giro de 180°**

### 3.4 Sem verde após o avanço
- Retomar **seguimento de linha (PID)**

---

## 4. Condições Críticas (Qualquer Momento)

### 4.1 Vermelho nos sensores extremos
- Parar **imediatamente** todos os motores  

### 4.2 Prata nos sensores extremos
- Ativar **modo rescue**

### 4.3 Obstáculo detectado
- Ativar **modo desvio**

---

## 5. Comportamento Sem Verde

### 5.1 Preto à esquerda + branco à direita
- Girar à **esquerda**  enquanto persistir  (`WHILE`)

### 5.2 Preto à direita + branco à esquerda
- Girar à **direita** enquanto persistir  (`WHILE`)

### 5.3 Preto em ambos os lados
- Mover à **frente**

---

## 6. Modo Especial — Desvio de Obstáculo

Ao detectar obstáculo frontal, o robô entra no modo **DESVIO**:

1. Parar  
2. Virar à esquerda  
3. Avançar até o sensor ultrassônico **direito** detectar o obstáculo  
4. Avançar mais **5 cm**  
5. Virar à direita  
6. Avançar até o sensor ultrassônico **direito** não detectar mais o obstáculo  
7. Avançar mais **5 cm**  
8. Virar à direita  
9. Avançar até o **sensor de linha esquerdo** detectar linha  
   - Ignorar detecção apenas no meio ou direita  
10. Virar à esquerda  
11. Retornar ao **modo seguidor de linha (PID)**  

📌 Observação:  
- O modo **DESVIO** é um **estado exclusivo**  
- Ao final, o robô **retorna automaticamente** ao modo PID

---

## 7. Estados do Sistema

- `PID`
- `VERDE`
- `DESVIO`
- `RESCUE`
- `PARAR`

---