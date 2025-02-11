# BitDogLab-ADC-Experimentos

Projeto para explorar a conversão **Analógico-Digital (ADC)** no RP2040 usando a placa **BitDogLab**.

**Neste Link podemos ver o projeto em funcionamento**


O projeto implementa:

* Leitura de valores analógicos usando o **ADC** do RP2040 - BitDogLab.
* Controle de LEDs RGB via **PWM** baseado nos valores do joystick.
* Representação visual da posição do joystick em um **display SSD1306** via **I2C**.
* Uso de **interrupções (IRQ)** para lidar com eventos de botões físicos.
* Implementação de **debounce** para evitar acionamentos repetidos de botões.

## Funcionalidades Implementadas

* **Leitura de valores analógicos:** Captura valores dos eixos **X** e **Y** do joystick via **ADC**.
* **Controle de LEDs RGB:** Varia a intensidade do LED **Azul** com base no eixo Y e do LED **Vermelho** com base no eixo X.
* **Movimentação no display:** Exibe um quadrado de **8x8 pixels** no **SSD1306**, representando a posição do joystick.
* **Botões físicos:**
  * **Botão do joystick:** Alterna o estado do **LED Verde** e modifica a borda do display.
  * **Botão A:** Liga/desliga os LEDs controlados por **PWM**.

## Bibliotecas Utilizadas

### **1. Pico SDK**
Fornece suporte para programação no RP2040, incluindo **GPIO, ADC, I2C e PWM**.

### **2. hardware/adc.h**
Utilizada para captura de valores analógicos dos pinos do **RP2040**.

### **3. hardware/pwm.h**
Permite o controle de **intensidade dos LEDs RGB** usando PWM.

### **4. hardware/i2c.h**
Usada para comunicação com o **display SSD1306**.

### **5. hardware/irq.h**
Gerencia **interrupções** para detecção de acionamentos de botões físicos.

### **6. inc/ssd1306.h**
Biblioteca para manipulação gráfica do **display OLED SSD1306**.

## Estrutura do Projeto

```plaintext
BitDogLab-ADC-Experimentos  # Nome do programa principal
├── inc/                     # Diretório de cabeçalhos
│   ├── ssd1306.h            # Biblioteca do display OLED
├── BitDogLab_ADC_Experimentos.c  # Código-fonte principal
├── pico_sdk_import.cmake    # Configuração do SDK
├── CMakeLists.txt           # Configuração do projeto
├── wokwi.toml               # Configuração para simulação no Wokwi
├── diagram.json             # Conexões do projeto - WOKWI
├── README.md                # Documentação do projeto
```

## Configuração do Ambiente de Desenvolvimento

O projeto utiliza o **Pico SDK**, **CMake**, **Ninja** e o compilador **arm-none-eabi-gcc**. O ambiente pode ser configurado conforme as instruções abaixo.

## Comandos para Clonar e Compilar o Projeto

1. Clone o repositório:
   ```bash
   git clone https://github.com/hsantosdias/BitDogLab-ADC-Experimentos.git
   ```
2. Compile e envie para o RP2040 utilizando o VS Code + Pico SDK.

## Fluxograma do Projeto

**Fluxograma Simplificado:**


**Descrição:**
1. **Inicialização do Sistema:** Configuração de **ADC, GPIOs, PWM e Display OLED**.
2. **Leitura do Joystick:** Captura dos valores **X e Y** via **ADC**.
3. **Controle dos LEDs:** Ajusta o brilho dos LEDs **Azul e Vermelho** conforme os valores do joystick.
4. **Atualização do Display:** Exibe um quadrado móvel representando a posição do joystick.
5. **Interação com Botões:** O botão do joystick e o botão **A** alteram o estado dos LEDs e do display.
6. **Loop Contínuo:** Mantém a leitura e atualização do sistema em tempo real.

## Considerações Finais

Este projeto demonstra a integração de **ADC, PWM e I2C** no RP2040 para leitura de sensores analógicos e controle de periféricos. Ele serve como uma base para aplicações mais avançadas em **sistemas embarcados** e **automação**.


