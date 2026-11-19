#ifndef PINES_H
#define PINES_H

#include "driver/gpio.h"

    // inicio
    #define INI GPIO_NUM_48

	//interrupcion stm32
	#define STM32_INT GPIO_NUM_14

	//musica
    #define MUS GPIO_NUM_4

	//spi
	#define SPI_MOSI GPIO_NUM_11
	#define SPI_MISO GPIO_NUM_12
	#define SPI_CLK GPIO_NUM_13
	#define SPI_CS GPIO_NUM_10

    // I2C
    #define SDA GPIO_NUM_5
    #define SCL GPIO_NUM_6

    // ToF XSHUT
    #define XSHUT_1 GPIO_NUM_41
    #define XSHUT_2 GPIO_NUM_42
    #define XSHUT_3 GPIO_NUM_44
    #define XSHUT_4 GPIO_NUM_16
    #define XSHUT_5 GPIO_NUM_15
    #define XSHUT_6 GPIO_NUM_7

    // ToF Interrupciones
    #define INT_1 GPIO_NUM_38
    #define INT_2 GPIO_NUM_39
    #define INT_3 GPIO_NUM_40
    #define INT_4 GPIO_NUM_8
    #define INT_5 GPIO_NUM_18
    #define INT_6 GPIO_NUM_17

    // LED RGB
    #define RGB_PIN_R GPIO_NUM_43
    #define RGB_PIN_G GPIO_NUM_2
	#define RGB_PIN_B GPIO_NUM_1

    // Botones Auxiliares
    #define VIC GPIO_NUM_47
    #define DER GPIO_NUM_21
#endif
