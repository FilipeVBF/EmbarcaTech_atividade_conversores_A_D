# EmbarcaTech_atividade_conversores_A_D

## Descrição

Este projeto visa a utilização do conversor analógico-digital (ADC) do RP2040 para controlar a intensidade de LEDs RGB e representar a posição de um joystick em um display OLED SSD1306. Além disso, o sistema implementa funcionalidade de interrupção para botões e exibição dinâmica no display.

## Componentes Utilizados

- Placa de desenvolvimento BitDogLab
- LED RGB (conectado às GPIOs 11, 12 e 13)
- Joystick (conectado às GPIOs 26 e 27)
- Botão do joystick (conectado à GPIO 22)
- Botão A (conectado à GPIO 5)
- Display OLED SSD1306

## Funcionalidades

- **Controle de LEDs RGB:**

  - O LED Azul ajusta seu brilho conforme o eixo Y do joystick.
  - O LED Vermelho ajusta seu brilho conforme o eixo X do joystick.
  - Controle realizado via PWM para transição suave da intensidade luminosa.

- **Exibição Gráfica no Display:**

  - Quadrado de 8x8 pixels se move proporcionalmente ao joystick.
  - Botão do joystick altera a borda do display.

- **Funções dos Botões:**

  - O botão do joystick alterna o estado do LED Verde e modifica a borda do display.
  - O botão A ativa/desativa os LEDs controlados por PWM.

## Requisitos

1. **Interrupções:** Uso de IRQs para funcionalidades dos botões.
2. **Debouncing:** Implementação via software para evitar acionamentos indesejados.
3. **Organização do Código:** Código estruturado e comentado para facilitar manutenção.

## Como Executar o Projeto

1. Clone o repositório:
   git clone https://github.com/FilipeVBF/EmbarcaTech_atividade_conversores_A_D.git
2. Importe a pasta do projeto para o ambiente de desenvolvimento do **Pico SDK**.
3. Conecte o hardware conforme a configuração sugerida.
4. Compile e carregue o código no **Raspberry Pi Pico W**.

## Instruções de Uso

1. **Movimentação do Joystick:**
   - Movimente o joystick para a esquerda/direita para alterar o brilho do LED Vermelho.
   - Movimente o joystick para cima/baixo para alterar o brilho do LED Azul.
   - A posição do joystick controla a posição do quadrado no display OLED.

2. **Uso dos Botões:**
   - Pressione o botão do joystick para alternar o LED Verde e modificar a borda do display.
   - Pressione o botão A para ativar/desativar os LEDs controlados por PWM.

3. **Exibição no Display:**
   - O quadrado de 8x8 pixels representa a posição do joystick.
   - A borda do display muda quando o botão do joystick é pressionado.

## Vídeo da Solução
Segue abaixo o link do vídeo da demonstração do projeto:
[Ver vídeo](https://drive.google.com/file/d/1kbH4HkeSD0vZiU3Lu2oL3OL7X_p8mPMJ/view?usp=sharing).
