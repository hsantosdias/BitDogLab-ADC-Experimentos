//Codigo construido com base no codigo original https://github.com/wiltonlacerda/EmbarcaTechU4C8/tree/main/ADC_DisplayEmC_Ex5

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "pico/bootrom.h"


#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22 // GPIO para botão do Joystick


// Definições para LEDs e botões
#define LED_PIN_R 13    // LED Vermelho 
#define LED_PIN_B 12    // LED Azul
#define LED_PIN_G 11    // LED Verde
#define BUTTON_PIN_A 5 // Botão A
#define BUTTON_PIN_B 6 // Botão B

//Trecho para modo BOOTSEL com botão B
void gpio_irq_handler(uint gpio, uint32_t events)
{
  reset_usb_boot(0, 0);
}


// Variáveis globais
//static volatile bool estado_led_verde = false; // Estado do LED Verde (inicialmente desligado) 
//static volatile bool estado_led_azul = false; // Estado do LED Azul (inicialmente desligado)
static ssd1306_t ssd;  // Definição global do display OLED SSD1306 128x64 I2C

// Prototipação de funções (assinaturas) - declaração de funções
//static void gpio_irq_handler(uint gpio, uint32_t events); // Função de interrupção com debounce e detecção de botão

void init_gpio(void); // Inicializa GPIOs (LEDs e Botões) 
void init_display(void); // Inicializa Display OLED SSD1306 128x64 I2C 
void atualizar_display(const char *linha1, const char *linha2); // Atualiza o display com duas mensagens (duas linhas)


/*
// Função de interrupção com debounce e detecção de botão
void gpio_irq_handler(uint gpio, uint32_t events) {
    static uint32_t last_time = 0; // Último tempo de pressionamento do botão (inicialmente 0)
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Tempo atual em microssegundos desde o boot

    if (current_time - last_time > 200000) { // Debounce de 200ms (200000us)
        last_time = current_time; // Atualiza o tempo de pressionamento do botão

        // Verifica qual botão foi pressionado e atualiza o estado do LED correspondente e exibe no display OLED SSD1306
        if (gpio == BUTTON_PIN_A) {
            estado_led_verde = !estado_led_verde; // Inverte o estado do LED Verde  (liga/desliga)
            gpio_put(LED_PIN_G, estado_led_verde); // Atualiza o estado do LED Verde 
            printf("Botão A pressionado: LED Verde %s\n", estado_led_verde ? "Ligado" : "Desligado"); // Exibe mensagem no terminal UART 
            atualizar_display(estado_led_verde ? "LED Verde ON" : "LED Verde off", ""); // Atualiza o display OLED SSD1306 (LED Verde ON/OFF)
        } 
        else if (gpio == BUTTON_PIN_B) {
            estado_led_azul = !estado_led_azul; // Inverte o estado do LED Azul (liga/desliga)
            gpio_put(LED_PIN_B, estado_led_azul); // Atualiza o estado do LED Azul
            printf("Botão B pressionado: LED Azul %s\n", estado_led_azul ? "Ligado" : "Desligado"); // Exibe mensagem no terminal UART
            atualizar_display(estado_led_azul ? "LED Azul ON" : "LED Azul off", ""); // Atualiza o display OLED SSD1306 (LED Azul ON/OFF)
        }
    }
}

*/



int main()
{
  // Para ser utilizado o modo BOOTSEL com botão B
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

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_t ssd; // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN);
  adc_gpio_init(JOYSTICK_Y_PIN);  
  


  uint16_t adc_value_x;
  uint16_t adc_value_y;  
  char str_x[5];  // Buffer para armazenar a string
  char str_y[5];  // Buffer para armazenar a string  
  
  bool cor = true;
  while (true)
  {
    adc_select_input(0); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
    adc_value_x = adc_read();
    adc_select_input(1); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
    adc_value_y = adc_read();    
    sprintf(str_x, "%d", adc_value_x);  // Converte o inteiro em string
    sprintf(str_y, "%d", adc_value_y);  // Converte o inteiro em string
    
    //cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_line(&ssd, 3, 25, 123, 25, cor); // Desenha uma linha
    ssd1306_line(&ssd, 3, 37, 123, 37, cor); // Desenha uma linha   
    ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 6); // Desenha uma string
    ssd1306_draw_string(&ssd, "Hugo S Dias", 20, 16); // Desenha uma string
    ssd1306_draw_string(&ssd, "ADC   JOYSTICK", 10, 28); // Desenha uma string 
    ssd1306_draw_string(&ssd, "X    Y    PB", 20, 41); // Desenha uma string    
    ssd1306_line(&ssd, 44, 37, 44, 60, cor); // Desenha uma linha vertical         
    ssd1306_draw_string(&ssd, str_x, 8, 52); // Desenha uma string     
    ssd1306_line(&ssd, 84, 37, 84, 60, cor); // Desenha uma linha vertical      
    ssd1306_draw_string(&ssd, str_y, 49, 52); // Desenha uma string   
    ssd1306_rect(&ssd, 52, 90, 8, 8, cor, !gpio_get(JOYSTICK_PB)); // Desenha um retângulo  
    ssd1306_rect(&ssd, 52, 102, 8, 8, cor, !gpio_get(BUTTON_PIN_A)); // Desenha um retângulo    
    ssd1306_rect(&ssd, 52, 114, 8, 8, cor, !cor); // Desenha um retângulo       
    ssd1306_send_data(&ssd); // Atualiza o display


    sleep_ms(100);
  }
}