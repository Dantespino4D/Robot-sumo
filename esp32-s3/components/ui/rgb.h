#ifndef RGB_H
#define RGB_H

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "pines.h"

#define LEDC_TIMER              LEDC_TIMER_3
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT   // Resolución (0-1023)
#define LEDC_FREQUENCY          (5000)              // Frecuencia 5 kHz

// --- Configuración LED RGB ---
// Canal 6 lo usa Musica.cpp; el azul usa el 7.
#define RGB_CHANNEL_R           LEDC_CHANNEL_4
#define RGB_CHANNEL_G           LEDC_CHANNEL_5
#define RGB_CHANNEL_B           LEDC_CHANNEL_7

static inline void pwm_rgb() {
    // 1. Configurar el Timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK,
		.deconfigure = false
    };
    ledc_timer_config(&ledc_timer);


    // 2. Configurar el Canal R
    ledc_channel_config_t ledc_channel_r = {
        .gpio_num       = RGB_PIN_R,
        .speed_mode     = LEDC_MODE,
        .channel        = RGB_CHANNEL_R,
		.intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,
        .duty           = 1023,
        .hpoint         = 0,
    	.sleep_mode     = LEDC_SLEEP_MODE_KEEP_ALIVE,
    	.flags          = 0,
		.deconfigure    = false
    };
    ledc_channel_config(&ledc_channel_r);


    // 3. Configurar el Canal G
    ledc_channel_config_t ledc_channel_g = {
        .gpio_num       = RGB_PIN_G,
        .speed_mode     = LEDC_MODE,
        .channel        = RGB_CHANNEL_G,
		.intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,
        .duty           = 1023,
        .hpoint         = 0,
    	.sleep_mode     = LEDC_SLEEP_MODE_KEEP_ALIVE,
    	.flags          = 0,
		.deconfigure    = false
    };
    ledc_channel_config(&ledc_channel_g);

    // 4. Configurar el Canal B
    ledc_channel_config_t ledc_channel_b = {
        .gpio_num       = RGB_PIN_B,
        .speed_mode     = LEDC_MODE,
        .channel        = RGB_CHANNEL_B,
		.intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,
        .duty           = 1023,
        .hpoint         = 0,
    	.sleep_mode     = LEDC_SLEEP_MODE_KEEP_ALIVE,
    	.flags          = 0,
		.deconfigure    = false
    };
    ledc_channel_config(&ledc_channel_b);
}

static inline void set_rgb_r(uint32_t duty) {
    ledc_set_duty(LEDC_MODE, RGB_CHANNEL_R, duty);
    ledc_update_duty(LEDC_MODE, RGB_CHANNEL_R);
}

static inline void set_rgb_g(uint32_t duty) {
    ledc_set_duty(LEDC_MODE, RGB_CHANNEL_G, duty);
    ledc_update_duty(LEDC_MODE, RGB_CHANNEL_G);
}

static inline void set_rgb_b(uint32_t duty) {
    ledc_set_duty(LEDC_MODE, RGB_CHANNEL_B, duty);
    ledc_update_duty(LEDC_MODE, RGB_CHANNEL_B);
}

// Función principal para el LED RGB
// Ánodo común: duty 1023 = apagado, duty 0 = encendido.
static inline void rgb(int r, int g, int b = 1023) {
    set_rgb_r(r);
    set_rgb_g(g);
    set_rgb_b(b);
}

#endif
