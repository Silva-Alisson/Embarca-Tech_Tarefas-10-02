#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define LED_RED 13
#define LED_BLUE 12
#define LED_GREEN 11
#define SW_PIN 22
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22 // GPIO para botão do Joystick
#define Botao_A 5 // GPIO para botão A

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6

void gpio_irq_handler(uint gpio, uint32_t events)
{
  reset_usb_boot(0, 0);
}

uint pwm_init_gpio(uint gpio, uint wrap) {
  gpio_set_function(gpio, GPIO_FUNC_PWM);

  uint slice_num = pwm_gpio_to_slice_num(gpio);
  pwm_set_wrap(slice_num, wrap);
  
  pwm_set_enabled(slice_num, true);  
  return slice_num;  
}

int main()
{
  stdio_init_all();
  // Inicialização do botão BOOTSEL
  gpio_init(botaoB);
  gpio_set_dir(botaoB, GPIO_IN);
  gpio_pull_up(botaoB);
  
  gpio_init(JOYSTICK_PB);
  gpio_set_dir(JOYSTICK_PB, GPIO_IN);
  gpio_pull_up(JOYSTICK_PB);

  gpio_init(SW_PIN);
  gpio_set_dir(SW_PIN, GPIO_IN);
  gpio_pull_up(SW_PIN);

  gpio_init(Botao_A);
  gpio_set_dir(Botao_A, GPIO_IN);
  gpio_pull_up(Botao_A);

  gpio_init(LED_GREEN);
  gpio_set_dir(LED_GREEN, GPIO_OUT);
  gpio_put(LED_GREEN, false); 
  
  
  // Inicialização do I2C
  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  ssd1306_t ssd;
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
  ssd1306_config(&ssd);
  ssd1306_send_data(&ssd);

  // Limpa o display
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Inicializa o ADC
  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN);
  adc_gpio_init(JOYSTICK_Y_PIN);

  uint pwm_wrap = 4096;  
  uint pwm_slice = pwm_init_gpio(LED_RED, pwm_wrap);  
  uint pwm_slice2 = pwm_init_gpio(LED_BLUE, pwm_wrap); 

  uint16_t adc_value_x;
  uint16_t adc_value_y;

  int posX, posY;
  bool cor = true;
  bool estado = false; // Variável de estado para alternar entre os estilos
  bool ultimo_estado_botao = true; // Para detectar transição de pressionamento

  bool pwm_ativo = true; // Estado inicial dos LEDs PWM (ligados)
  bool ultimo_estado_botaoA = true; // Para detectar transição do Botão A


  while (true)
  {
    adc_select_input(0); // Eixo X
    adc_value_x = adc_read();
    adc_select_input(1); // Eixo Y
    adc_value_y = adc_read();

    bool sw_value = gpio_get(SW_PIN) == 0;
    bool botaoA_pressionado = gpio_get(Botao_A) == 0; // Botão A pressionado

    // Detecta se o Botão A foi pressionado (transição de 1 -> 0)
    if (botaoA_pressionado && !ultimo_estado_botaoA) {
      pwm_ativo = !pwm_ativo; // Alterna entre ligado/desligado
    }
    ultimo_estado_botaoA = botaoA_pressionado; // Atualiza o estado anterior

    // Detecta se o botão foi pressionado (transição de 1 -> 0)
    if (sw_value && !ultimo_estado_botao) {
      estado = !estado; // Alterna entre verdadeiro e falso
    }
    ultimo_estado_botao = sw_value; // Atualiza o estado anterior

    if (pwm_ativo) {
      // Controle dos LEDs PWM apenas se estiver ativo
      if (adc_value_x > 2350 || adc_value_x < 1950) {
          pwm_set_gpio_level(LED_RED, adc_value_x);
      } else {
          pwm_set_gpio_level(LED_RED, 0);
      }

      if (adc_value_y > 2350 || adc_value_y < 1950) {
          pwm_set_gpio_level(LED_BLUE, adc_value_y);
      } else {
          pwm_set_gpio_level(LED_BLUE, 0);
      }
  } else {
      // Desliga os LEDs PWM
      pwm_set_gpio_level(LED_RED, 0);
      pwm_set_gpio_level(LED_BLUE, 0);
  }

    // Mapeia os valores do ADC (0-4095) para o display (0-127, 0-63)
    posY = (adc_value_y * 120) / 4095;
    posX = 56 - ((adc_value_x * 56) / 4095);

    // Atualiza o display
    ssd1306_fill(&ssd, !cor); // Limpa a tela
    ssd1306_rect(&ssd, posX, posY, 8, 8, cor, cor); // Desenha o quadrado na nova posição

    // Alterna entre os estilos de desenho
    if (estado) {
      ssd1306_rect(&ssd, 2, 2, 123, 59, cor, !cor);
      gpio_put(LED_GREEN, true);
    } else {
      ssd1306_rect(&ssd, 0, 0, 128, 63, cor, !cor);
      gpio_put(LED_GREEN, false);
    }

    printf("adc_value_x: %u, adc_value_y: %u\n", adc_value_x, adc_value_y);
    printf("posX: %u, posY: %u\n", posX, posY);
    ssd1306_send_data(&ssd); // Atualiza o display

    sleep_ms(100);
  }
}
