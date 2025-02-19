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
static volatile bool estado_led_verde = false; // Estado do LED Verde
static volatile bool estado_pwm_leds = true; // Estado do controle PWM dos LEDs
static volatile bool estado_borda = false; // Estado da borda do display


// Trecho para modo BOOTSEL com botão B e controle do LED Vermelho
void gpio_irq_handler(uint gpio, uint32_t events) {
    static uint32_t last_time = 0; // Variável para debounce
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Tempo atual em microsegundos desde o boot

    if (current_time - last_time > 200000) { // Debounce de 200ms
        last_time = current_time;

        if (gpio == JOYSTICK_PB) { // Botão do Joystick para controle do LED Vermelho e Azul 
            estado_led_verde = !estado_led_verde; // Inverte o estado do LED Verde
            gpio_put(LED_PIN_G, estado_led_verde); // Atualiza o estado do LED Verde
            estado_borda = !estado_borda; // Inverte o estado da borda do display OLED SSD1306 128x64 I2C
        }
        else if (gpio == BUTTON_PIN_B) { // Botão B para modo BOOTSEL
            reset_usb_boot(0, 0); // Reseta o Pico para o modo BOOTSEL
        }
        else if (gpio == BUTTON_PIN_A) { // Botão A para controle do PWM dos LEDs
            estado_pwm_leds = !estado_pwm_leds; // Inverte o estado do controle PWM dos LEDs 
            if (!estado_pwm_leds) { // Se o controle PWM dos LEDs estiver desativado 
                pwm_set_gpio_level(LED_PIN_R, 0); // Desliga o LED Vermelho
                pwm_set_gpio_level(LED_PIN_B, 0); // Desliga o LED Azul
            }
        }
    }
}

// Initialização das portas GPIO para botões e LEDs
void init_gpio(void) {
    //Trata a interrupção dos botões B e A
    // Botão B para modo BOOTSEL 
    gpio_init(BUTTON_PIN_B);
    gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_B);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Botão A para controle do PWM dos LEDs
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    // Trata a interrupção do botão do Joystick
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Inicializa o LED Verde digital
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, false);
}

//Função para inicialização do display OLED SSD1306 128x64 I2C
void init_display(void) {
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT); // Inicializa o display OLED SSD1306 128x64 I2C com endereço 0x3C
    ssd1306_config(&ssd); // Configura o display OLED SSD1306 128x64 I2C

  // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd); // Envia os dados para o display
}

//Função para inicialização do pwm para controle do LED RGB vermleho e azul
void init_pwm(void) {
    gpio_set_function(LED_PIN_R, GPIO_FUNC_PWM);
    gpio_set_function(LED_PIN_B, GPIO_FUNC_PWM);
    uint slice_r = pwm_gpio_to_slice_num(LED_PIN_R);
    uint slice_b = pwm_gpio_to_slice_num(LED_PIN_B);
    pwm_set_wrap(slice_r, 4095);
    pwm_set_wrap(slice_b, 4095);
    pwm_set_enabled(slice_r, true);
    pwm_set_enabled(slice_b, true);
}

int main() {
    init_gpio(); // Inicializa as portas GPIO para botões e LEDs
    init_pwm(); // Inicializa o PWM para controle do LED RGB Vermelho e Azul
    init_display(); // Inicializa o display OLED SSD1306 128x64 I2C
    adc_init(); // Inicializa o ADC para leitura do Joystick
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    
    uint16_t adc_value_x, adc_value_y; // Variáveis para armazenar o valor do ADC
    int quadrado_x = WIDTH / 2 - 4;
    int quadrado_y = HEIGHT / 2 - 4;
    
    
    while (true) {
        // Leitura dos valores do ADC para os eixos X e Y
        adc_select_input(0); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
        adc_value_x = adc_read();
        adc_select_input(1); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
        adc_value_y = adc_read();
        
    // Atualiza o brilho dos LEDs Vermelho e Azul com base nos valores do ADC para os eixos X e Y 
    if (adc_value_x >= 1900 && adc_value_x <= 2050 && adc_value_y >= 1900 && adc_value_y <= 2050) {
        pwm_set_gpio_level(LED_PIN_R, 0);
        pwm_set_gpio_level(LED_PIN_B, 0);
    } else if (estado_pwm_leds) {
        pwm_set_gpio_level(LED_PIN_R, abs(adc_value_x - 2048));
        pwm_set_gpio_level(LED_PIN_B, abs(adc_value_y - 2048));
    }

        // Atualiza a posição do quadrado no display com base nos valores do ADC para os eixos X e Y
        // codigo original removido pois estava invertendo os eixos X e Y do display
        // quadrado_x = ((adc_value_x) * (WIDTH - 8)) / 4095;
        // quadrado_y = ((4095 - adc_value_y) * (HEIGHT - 8)) / 4095;

        // Atualiza a posição do quadrado no display com base nos valores do ADC para os eixos X e Y
        // Comenta a linha acima e descomenta as linhas abaixo para inverter os eixos X e Y do display
        quadrado_x = ((adc_value_y) * (WIDTH - 8)) / 4095;  // Usa adc_value_y para o eixo X    
        quadrado_y = ((4095 - adc_value_x) * (HEIGHT - 8)) / 4095;  // Usa adc_value_x para o eixo Y


        // Atualiza o display com o quadrado e a borda
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, quadrado_y, quadrado_x, 8, 8, true, true);
        ssd1306_rect(&ssd, 0, 0, WIDTH, HEIGHT, true, false);
        // Atualiza a borda do display
        if (estado_borda) {
            ssd1306_rect(&ssd, 2, 2, WIDTH, HEIGHT, true, false);
        }

        ssd1306_send_data(&ssd); // Envia os dados para o display OLED SSD1306 128x64 I2C
        
        sleep_ms(100);
    }
}
