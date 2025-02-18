// Codigo construido com base no codigo original https://github.com/wiltonlacerda/EmbarcaTechU4C8/tree/main/ADC_DisplayEmC_Ex5
// Hugo Santos Dias - 2025 - Tarefa 1 - conversor ADC e display OLED SSD1306 128x64 I2C

#include <stdio.h> //Bibliotec para printf e sprintf 
#include <stdlib.h> //Biblioteca para funções de conversão de tipos de dados
#include "pico/stdlib.h" //Biblioteca para funções de inicialização do Pico e funções de delay

#include "hardware/adc.h" //Biblioteca para funções de inicialização do ADC e funções de leitura do ADC
#include "hardware/i2c.h" //Biblioteca para funções de inicialização do I2C e funções de comunicação I2C uso do display OLED SSD1306 128x64 I2C
#include "hardware/irq.h" //Biblioteca para funções de interrupção de hardware
#include "hardware/pwm.h" //Biblioteca para funções de inicialização do PWM controle de brilho do LED RGB
#include "hardware/timer.h" //Biblioteca para funções de inicialização do Timer e funções de interrupção de tempo
#include "pico/bootrom.h" //Biblioteca para funções de inicialização do Pico e funções de delay para modo BOOTSEL Botão B
//Bibliotecas especificas para o display OLED SSD1306 128x64 I2C e fontes
#include "lib/ssd1306.h" //Biblioteca para funções de inicialização do display OLED SSD1306 128x64 I2C
#include "lib/font.h" //Biblioteca para uso de fontes maiuscula/minuscula/numero no display OLED SSD1306 128x64 I2C

// Definições para o display OLED SSD1306 128x64 I2C 
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

// Definições para a leitura do joystick e botões - ADC
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22 // GPIO para botão do Joystick

// Definições para LEDs e botões
#define LED_PIN_R 13    // LED Vermelho 
#define LED_PIN_B 12    // LED Azul
#define LED_PIN_G 11    // LED Verde
#define BUTTON_PIN_A 5 // Botão A
#define BUTTON_PIN_B 6 // Botão B

// Variáveis globais
static ssd1306_t ssd;  // Definição global do display OLED SSD1306 128x64 I2C
static volatile bool estado_led_vermelho = false; // Estado do LED Vermelho (inicialmente desligado)
static volatile bool estado_led_verde = false; // Estado do LED Verde
static volatile bool estado_pwm_leds = true; // Estado do controle PWM dos LEDs
static volatile bool estado_borda = false; // Estado da borda do display


// Trecho para modo BOOTSEL com botão B e controle do LED Vermelho
void gpio_irq_handler(uint gpio, uint32_t events) {
   
  
  if (gpio == BUTTON_PIN_B) {
    reset_usb_boot(0, 0);
}
  
  if (gpio == BUTTON_PIN_A) {
        estado_led_vermelho = !estado_led_vermelho; // Alterna o estado do LED Vermelho
        gpio_put(LED_PIN_R, estado_led_vermelho); // Atualiza o LED
        printf("Botão A pressionado: LED Vermelho %s\n", estado_led_vermelho ? "Ligado" : "Desligado");
    }
}

void init_gpio(void) {
    // Inicializa os botões
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BUTTON_PIN_B);
    gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_B);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);

    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);

    // Inicializa os LEDs
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, false);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, false);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, false);
}

void init_display(void) {
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display

  // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd); // Envia os dados para o display
}

int main() {
    init_gpio();
    init_display();
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    
    uint16_t adc_value_x;
    uint16_t adc_value_y;
    char str_x[5];  // Buffer para armazenar a string
    char str_y[5];  // Buffer para armazenar a string  
    
    bool cor = true;
    while (true) {
        adc_select_input(0); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
        adc_value_x = adc_read();
        adc_select_input(1); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
        adc_value_y = adc_read();
        
        sprintf(str_x, "%d", adc_value_x);
        sprintf(str_y, "%d", adc_value_y);
        
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_send_data(&ssd);
        
        sleep_ms(100);
    }
}
