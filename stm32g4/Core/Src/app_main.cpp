#include "app_main.h"
#include "Motores.h"
#include "Spi.h"
#include "adc.h"
#include "comp.h"

extern Spi spi;
Motores mot;

extern DMA_HandleTypeDef hdma_dac1;

extern volatile uint16_t adc_buffer[5];
extern DMA_HandleTypeDef hdma_adc1;

void app_main(void){
	HAL_COMP_Start(&hcomp1);
	HAL_COMP_Start(&hcomp2);
	HAL_COMP_Start(&hcomp7);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 5);
	LL_DAC_SetHighFrequencyMode(DAC1, LL_DAC_HIGH_FREQ_MODE_ABOVE_160MHZ);
	LL_DAC_SetHighFrequencyMode(DAC2, LL_DAC_HIGH_FREQ_MODE_ABOVE_160MHZ);
	__HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT | DMA_IT_TC);
	LL_DAC_Enable(DAC1,LL_DAC_CHANNEL_1);
	LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_2);
	LL_DAC_Enable(DAC2, LL_DAC_CHANNEL_1);
	mot.begin();
	spi.begin();
 while (1){
		__WFI();
	}
}
