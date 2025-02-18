# Projeto: Controle de Joystick com Raspberry Pi Pico e Display OLED

## Demonstração 📹
*https://drive.google.com/file/d/11mwvvzdRmntwD1z07JLkwbUb5prZuEEo/view?usp=sharing*

## Descrição
Este projeto implementa um sistema interativo utilizando um **joystick analógico**, um **display OLED SSD1306**, **LEDs RGB controlados por PWM** e **botões físicos**, utilizando um Raspberry Pi Pico e a placa BitDogLab. O sistema permite visualizar e controlar a posição do joystick, além de alternar estados com botões para diferentes funcionalidades.

## Funcionalidades
- **Leitura dos eixos X e Y do joystick** via ADC.
- **Controle de LEDs RGB com PWM**:
  - O LED Azul ajusta o brilho com base no eixo Y.
  - O LED Vermelho ajusta o brilho com base no eixo X.
- **Exibição no display OLED**:
  - Quadrado móvel de 8x8 pixels que se desloca proporcionalmente aos valores do joystick.
  - Alternância de estilos de borda ao pressionar o botão do joystick.
- **Interação com botões**:
  - O botão do joystick alterna a borda do display e ativa/desativa o LED Verde.
  - O Botão A ativa ou desativa os LEDs RGB controlados por PWM.

## Requisitos Técnicos
- **Uso de interrupções (IRQ)** para detecção dos botões.
- **Implementação de debouncing** via software.
- **Comunicação I2C** para o display SSD1306.
- **Código bem estruturado e comentado**.

## Hardware Utilizado
- **Raspberry Pi Pico**
- **Joystick analógico** (GPIOs 26 e 27)
- **Display OLED SSD1306** (I2C: GPIOs 14 e 15)
- **LED RGB** (GPIOs 11, 12 e 13)
- **Botão do Joystick** (GPIO 22)
- **Botão A** (GPIO 5)

## Como Executar
1. Conecte os componentes conforme os pinos definidos no código.
2. Compile e carregue o código no Raspberry Pi Pico.
3. Observe a resposta dos LEDs e do display conforme os movimentos do joystick e botões.

## Licença
Este projeto é de código aberto e pode ser modificado e distribuído conforme necessário.
