#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/freeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <led_strip.h>
#include <esp_log.h>

static int btn[3] = {4, 5, 6};
static int res = 7;
static int ledPin = 48;
static led_strip_handle_t led_strip;
static led_strip_config_t strip_config;
static led_strip_rmt_config_t rmt_config;
bool flagRed = false, flagGrn = false, flagBlu = false;

typedef void (*func)(void*);
void IRAM_ATTR handlerRed(void *arg){flagRed = !flagRed;}
void IRAM_ATTR handlerGrn(void *arg){flagGrn = !flagGrn;}
void IRAM_ATTR handlerBlu(void *arg){flagBlu = !flagBlu;}
func handler[3] = {&handlerRed, &handlerGrn, &handlerBlu};

void ledInit(){
    strip_config.strip_gpio_num = ledPin;
    strip_config.max_leds = 1;
    rmt_config.resolution_hz = 10 * 1000 * 1000;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    led_strip_clear(led_strip);
}
void gpioInit(){
    gpio_install_isr_service(0);
    for(int i = 0; i < 3; i++){
        ESP_LOGI("gpioInit", "init..");
        gpio_set_direction(btn[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(btn[i], GPIO_PULLUP_ONLY);
        gpio_set_intr_type(btn[i], GPIO_INTR_NEGEDGE);
        gpio_intr_enable(btn[i]);
        gpio_isr_handler_add(btn[i], handler[i], &i);
    }
}
void app_main(void){
    ESP_LOGI("main", "Main Running");
    ledInit();
    gpioInit();
    while(true){
        led_strip_set_pixel(led_strip, 0, flagRed ? 8 : 0, flagGrn ? 8 : 0, flagBlu ? 8 : 0);
        led_strip_refresh(led_strip);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
