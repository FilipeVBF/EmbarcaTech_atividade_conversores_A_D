#include <stdio.h>
#include "pico/stdlib.h"    
#include "pico/time.h"      
#include "hardware/irq.h"   
#include "hardware/pwm.h"   
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/adc.h"     

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_ADDR 0x3C

#define LED_VERMELHO 13
#define LED_VERDE 11
#define LED_AZUL 12
#define BOTAO_A 5
#define JOYSTICK_BOTAO 22
#define JOYSTICK_X 27
#define JOYSTICK_Y 26

const float PWM_DIV = 100.0;     
const uint16_t PWM_WRAP = 25000;

volatile bool status_led_verde = false;   
volatile bool pwm_ligado = true;         
static volatile uint a = 1;
static volatile uint32_t ultimo_tempo = 0; 

uint16_t RECT_Y_MIN = 4;
uint16_t RECT_Y_MAX = 52;
uint16_t RECT_X_MIN = 4;
uint16_t RECT_X_MAX = 116;
uint8_t borda = 0;  
uint16_t borda_1 = 1;
uint16_t borda_2 = 60;
uint16_t borda_3 = 1;
uint16_t borda_4 = 124;
uint16_t t = 0;
bool aumenta = true; 

void init_io();

//Função para aplicar tratamento das interrupções dos botões
static void gpio_interruptor(uint gpio, uint32_t events)
{
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());

    //Verificação do debounce de 300 ms
    if (tempo_atual - ultimo_tempo > 300000) {
        ultimo_tempo = tempo_atual;

        // Verificação do botão do joystick e alteração do estado do LED verde
        if (gpio == JOYSTICK_BOTAO) {

            // Alterna o estado do LED verde
            status_led_verde = !status_led_verde;
            gpio_put(LED_VERDE, status_led_verde);

            // Altera a borda do retângulo
            if (aumenta) {
                borda++;
                if (borda >= 5)  {
                    aumenta = false;
                }
            }
            else {
                borda--;
                if (borda == 0)  {
                    aumenta = true;
                }
            }
            // Altera tamanho do retângulo
            borda_1 = borda;
            borda_2 = 64 - borda;
            borda_3 = borda;
            borda_4 = 128 - borda;

            // Muda os limites do retângulo
            RECT_Y_MIN = borda_1 + 3;
            RECT_Y_MAX = 52 - borda;
            RECT_X_MIN = borda_3 + 3;
            RECT_X_MAX = 116 - borda;
        }

        if (gpio == BOTAO_A) { 
            pwm_ligado = !pwm_ligado;
        }
    }
}

//Função para configurar o PWM de um pino
void configuracao_pwm(uint pino, uint funcao_gpio, float clkdiv, uint16_t wrap, uint16_t nivel) {
    // Configura o pino como PWM e define a função de GPIO
    gpio_set_function(pino, funcao_gpio);
    // Obtém o número do slice PWM correspondente ao pino
    uint slice_num = pwm_gpio_to_slice_num(pino);
    // Configura o divisor de clock
    pwm_set_clkdiv(slice_num, clkdiv);
    // Configura o wrap
    pwm_set_wrap(slice_num, wrap);
    // Configura o nível do PWM
    pwm_set_gpio_level(pino, nivel);
    // Habilita o PWM
    pwm_set_enabled(slice_num, true);
}

//Função que cria o retangulo
void ssd1306_draw_thick_rectangle(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, uint8_t thickness) {
    for (uint8_t t = 0; t < thickness; ++t) {
        ssd1306_rect(ssd, top - t, left - t, width + 2 * t, height + 2 * t, value, false);
    } 
}

//Função para inicializar leds
void init_gpio_led(int led_pin, bool is_output, bool status) {
    gpio_init(led_pin);                                         // Inicializa o pino do LED
    gpio_set_dir(led_pin, is_output ? GPIO_OUT : GPIO_IN);      // Configura o pino como saída ou entrada
    gpio_put(led_pin, status);                                   // Garante que o LED inicie apagado
}

//Função para inicializar botões
void init_gpio_button(int button_pin, bool is_output) {
    gpio_init(button_pin);                                          // Inicializa o botão
    gpio_set_dir(button_pin, is_output ? GPIO_OUT : GPIO_IN);       // Configura o pino como entrada ou saída
    gpio_pull_up(button_pin);                                       // Habilita o pull-up interno
}

int main() {
    // Inicializa comunicação USB CDC para monitor serial
    stdio_init_all(); 
    
    // Inicializando pino do LED RGB
    init_gpio_led(LED_VERMELHO, true, false);
    init_gpio_led(LED_AZUL, true, false);
    init_gpio_led(LED_VERDE, true, false);

    // Inicializando pino dos botões (Botão A e Botão do Joystick)
    init_gpio_button(BOTAO_A, false);
    init_gpio_button(JOYSTICK_BOTAO, false);

    // Inicializando pino do Joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);  

    // Configuração do PWM para os LEDs RGB
    configuracao_pwm(LED_AZUL, GPIO_FUNC_PWM, PWM_DIV, PWM_WRAP, 0);
    configuracao_pwm(LED_VERMELHO, GPIO_FUNC_PWM, PWM_DIV, PWM_WRAP, 0);

    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_interruptor);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BOTAO, GPIO_IRQ_EDGE_FALL, true, &gpio_interruptor);
  
    // Inicializa a comunicação I2C (400 kHz)
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicializa e configura o display OLED
    ssd1306_t display;
    ssd1306_init(&display, WIDTH, HEIGHT, false, I2C_ADDR, I2C_PORT);
    ssd1306_config(&display);
    ssd1306_send_data(&display);
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);
   
    // Variáveis para controle do joystick e da animação do display
    bool cor = true;
    uint16_t antigo_x = 40;
    uint16_t antigo_y = 40;
    uint16_t novo_x = antigo_x;
    uint16_t novo_y = antigo_y;
    bool controle_borda = status_led_verde;

    // Variáveis para controle do PWM
    int pwm_x = 0;
    int pwm_y = 0;

    while (1) {
        cor = !cor;
        // Leitura dos valores do joystick
        adc_select_input(0); 
        uint16_t adc_y_value = 4095 - adc_read();
        adc_select_input(1);
        uint16_t adc_x_value = adc_read();

        int novo_x = adc_x_value / 32;
        int novo_y = adc_y_value / 64;

        // Limita os valores de x e y
        novo_x = (novo_x < RECT_X_MIN) ? RECT_X_MIN : (novo_x > RECT_X_MAX) ? RECT_X_MAX : novo_x;
        novo_y = (novo_y < RECT_Y_MIN) ? RECT_Y_MIN : (novo_y > RECT_Y_MAX) ? RECT_Y_MAX : novo_y;

        // Desenha o cursor do joystick
        ssd1306_draw_char(&display, ' ', antigo_x, antigo_y);
        ssd1306_draw_char(&display, '~', novo_x, novo_y);

        antigo_x = novo_x;
        antigo_y = novo_y;

        ssd1306_send_data(&display);

        // Desenha o retângulo e a borda
        if (controle_borda != status_led_verde) {
            ssd1306_fill(&display, false);
            ssd1306_draw_thick_rectangle(&display, borda_1, borda_3, borda_4 - borda_3, borda_2 - borda_1, true, borda);
            ssd1306_send_data(&display);
            controle_borda = status_led_verde;
        }

        pwm_x = abs((int16_t)(adc_x_value - 2090));
        pwm_y = abs((int16_t)(adc_y_value - 2090));

        if (pwm_ligado == true && pwm_x > 20 && pwm_y > 20) {
            pwm_set_gpio_level(LED_VERMELHO, ((pwm_x * 25000) / 2090));
            pwm_set_gpio_level(LED_AZUL, ((pwm_y * 25000) / 2090));
        }
        else {
            pwm_set_gpio_level(LED_VERMELHO, 0);
            pwm_set_gpio_level(LED_AZUL, 0);
        }
    }
}
