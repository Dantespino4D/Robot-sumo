/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "protocolo/sumo_protocol.h"
#include "stm32g474xx.h"
#include "stm32g4xx_ll_tim.h"
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ALPHA 1
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
//las velocidades objetivos
extern volatile uint32_t velObjetivos;
extern volatile uint16_t adc_buffer[5];
static uint32_t batFiltrada = 0;
//tiempo
uint64_t ms = 0;

//variables de la evacion de mimite
uint64_t tiempo = 0;
uint16_t TRetroceso = 1000;
uint8_t evasion = 0;

//variables de la evacion de stall
volatile uint32_t tiempo_st = 0;
uint16_t TStall = 250;
volatile uint8_t stall = 0;

//variables de la direccion del stall
volatile uint8_t stall_dir1 = 0;
volatile uint8_t stall_dir2 = 0;

//variables de los comparadores
uint8_t tcrt1 = 0;
uint8_t tcrt2 = 0;
uint8_t tcrt3 = 0;
uint8_t tcrt4 = 0;

//loop up table
const uint16_t LUT_RAMPA[50] = {
      0,    2,    5,    9,   14,   20,   28,   38,   50,   64,
     81,  101,  124,  151,  181,  215,  253,  294,  338,  385,
    434,  485,  511,  538,  589,  638,  685,  729,  770,  808,
    842,  872,  899,  922,  942,  959,  973,  985,  995, 1003,
   1009, 1014, 1018, 1021, 1023, 1023, 1023, 1023, 1023, 1023
};

// Variables de la rampa de acerelacion
static int16_t v1_obj = 0;
static int16_t v2_obj = 0;
static int16_t vel1_ini = 0;
static int16_t vel2_ini = 0;
static uint8_t indice_1 = 50;
static uint8_t indice_2 = 50;
static int16_t velActual_1 = 0;
static int16_t velActual_2 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
extern void SPI_DMA_RX_Callback(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
	SPI_DMA_RX_Callback();
  /* USER CODE END DMA1_Channel1_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */
	if (LL_DMA_IsActiveFlag_TC2(DMA1)) {
		LL_DMA_ClearFlag_TC2(DMA1);
	}
  /* USER CODE END DMA1_Channel2_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */
	if (LL_DMA_IsActiveFlag_TE2(DMA1)) {
		LL_DMA_ClearFlag_TE2(DMA1);
	}
  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */
	if(LL_DMA_IsActiveFlag_TC3(DMA1)) {
		LL_DMA_ClearFlag_TC3(DMA1);
	}
	if(LL_DMA_IsActiveFlag_HT3(DMA1)) {
		LL_DMA_ClearFlag_HT3(DMA1);
	}
	if(LL_DMA_IsActiveFlag_TE3(DMA1)) {
		LL_DMA_ClearFlag_TE3(DMA1);
	}
	return;
  /* USER CODE END DMA1_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */

  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
  * @brief This function handles TIM1 break interrupt and TIM15 global interrupt.
  */
void TIM1_BRK_TIM15_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_BRK_TIM15_IRQn 0 */
	//se limpia la bandera de interrupcion
	TIM1->SR = ~TIM_SR_BIF;

	//se envia una señal al esp32
	GPIOC->BSRR = GPIO_PIN_9;

	//verifica si estamos en evacion
	if(evasion == 1){
		return;
	}

	//actuvar evasion
	evasion = 1;

	//se guarga el estado de los comparadores
	tcrt1 = (COMP1->CSR & COMP_CSR_VALUE) ? 1 : 0;
	tcrt2 = (COMP2->CSR & COMP_CSR_VALUE) ? 1 : 0;
	tcrt3 = (COMP3->CSR & COMP_CSR_VALUE) ? 1 : 0;
	tcrt4 = (COMP4->CSR & COMP_CSR_VALUE) ? 1 : 0;

	//detonante de la secuencia de frenado
	tiempo = ms;

	//se para la rampa de velocidad
	velObjetivos = 0;
	velActual_1 = 0;
	velActual_2 = 0;
	TIM1->CCR1 = 1023;
	TIM1->CCR2 = 1023;
	TIM1->CCR3 = 1023;
	TIM1->CCR4 = 1023;

	//desactivamos temporalmente esta interrupcion
	LL_TIM_DisableIT_BRK(TIM1);

  /* USER CODE END TIM1_BRK_TIM15_IRQn 0 */
  /* USER CODE BEGIN TIM1_BRK_TIM15_IRQn 1 */

  /* USER CODE END TIM1_BRK_TIM15_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC3 channel underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */
	if(LL_TIM_IsActiveFlag_UPDATE(TIM6)) {
		LL_TIM_ClearFlag_UPDATE(TIM6);

		//aumento del contador de tiempo
		ms++;

		//se evalua si efectuar la evacion
		if(evasion == 1) {
			//apagamos el freno para poder salir de la linea
			LL_TIM_DisableBreakInputSource(TIM1, LL_TIM_BREAK_INPUT_BKIN, LL_TIM_BKIN_SOURCE_BKCOMP1);
			LL_TIM_DisableBreakInputSource(TIM1, LL_TIM_BREAK_INPUT_BKIN, LL_TIM_BKIN_SOURCE_BKCOMP2);
			LL_TIM_DisableBreakInputSource(TIM1, LL_TIM_BREAK_INPUT_BKIN, LL_TIM_BKIN_SOURCE_BKCOMP3);
			LL_TIM_DisableBreakInputSource(TIM1, LL_TIM_BREAK_INPUT_BKIN, LL_TIM_BKIN_SOURCE_BKCOMP4);

			//limpiar la bandera de freno
			LL_TIM_ClearFlag_BRK(TIM1);


			//secuencia de retroceso direccion a
			if(((ms - tiempo) <= TRetroceso) && (tcrt1 == 1 || tcrt2 == 1)) {
				//accion de retroceso direccion a
				TIM1->CCR1 = 1023;
				TIM1->CCR2 = 0;
				TIM1->CCR3 = 1023;
				TIM1->CCR4 = 0;
			}else if(((ms - tiempo) <= TRetroceso) && (tcrt3 == 1 || tcrt4 == 1)) {
				//accion de retroceso direcionb
				TIM1->CCR1 = 0;
				TIM1->CCR2 = 1023;
				TIM1->CCR3 = 0;
				TIM1->CCR4 = 1023;
			}else{
				//termino la evacion
				evasion = 0;
				tcrt1 = 0;
				tcrt2 = 0;
				tcrt3 = 0;
				tcrt4 = 0;

				//reactivamos el freno para que no se salga de la linea
				LL_TIM_EnableBreakInputSource(TIM1, LL_TIM_BREAK_INPUT_BKIN, LL_TIM_BKIN_SOURCE_BKCOMP1);
				LL_TIM_EnableBreakInputSource(TIM1, LL_TIM_BREAK_INPUT_BKIN, LL_TIM_BKIN_SOURCE_BKCOMP2);
				LL_TIM_EnableBreakInputSource(TIM1, LL_TIM_BREAK_INPUT_BKIN, LL_TIM_BKIN_SOURCE_BKCOMP3);
				LL_TIM_EnableBreakInputSource(TIM1, LL_TIM_BREAK_INPUT_BKIN, LL_TIM_BKIN_SOURCE_BKCOMP4);

				//limpiamos bandera
				LL_TIM_ClearFlag_BRK(TIM1);

				//reactivamos la interrupcion
				LL_TIM_EnableIT_BRK(TIM1);

				//frenamos el robot esperando ordenes del esp32
				TIM1->CCR1 = 1023;
				TIM1->CCR2 = 1023;
				TIM1->CCR3 = 1023;
				TIM1->CCR4 = 1023;
			}

			//hailitamos las salidas
			LL_TIM_EnableAllOutputs(TIM1);

			//retornamos
			return;
		}

		if(stall == 1) {
			//desactibar el AWD
			ADC1->IER &= ~ADC_IER_AWD2IE;

			//ejecutamos la maniobra
			if((ms - tiempo_st) <= TStall) {
				//accion de retroceso direccion opuesta
				if (stall_dir1 > 0) {
					TIM1->CCR1 = stall_dir1;
					TIM1->CCR2 = 0;
				} else if (stall_dir1 < 0) {
					TIM1->CCR1 = 0;
					TIM1->CCR2 = -stall_dir1;
				} else {
					TIM1->CCR1 = 1023;
					TIM1->CCR2 = 1023;
				}

				if (stall_dir2 > 0) {
					TIM1->CCR3 = stall_dir2;
					TIM1->CCR4 = 0;
				} else if (stall_dir2 < 0) {
					TIM1->CCR3 = 0;
					TIM1->CCR4 = -stall_dir2;
				} else {
					TIM1->CCR3 = 1023;
					TIM1->CCR4 = 1023;
				}
			}else{
				//termino la maniobra
				stall = 0;
				velActual_1 = 0;
				velActual_2 = 0;
				v1_obj = 0;
				v2_obj = 0;

				//frenamos el robot esperando ordenes del esp32
				TIM1->CCR1 = 1023;
				TIM1->CCR2 = 1023;
				TIM1->CCR3 = 1023;
				TIM1->CCR4 = 1023;

				//reactivamos el AWD y limpiamos la bandera
				ADC1->ISR = ADC_ISR_AWD2;
				ADC1->IER |= ADC_IER_AWD2IE;
			}
			LL_TIM_EnableAllOutputs(TIM1);
			return;
		}

		GPIOC->BSRR = (GPIO_PIN_9 << 16U);

		//verificar si esta sobre el borde
		if(!(TIM1->BDTR & TIM_BDTR_MOE)) {
			//establecer las velocidades en 0
			velActual_1 = 0;
			velActual_2 = 0;

			//estavlecer los objetivos en 0
			v1_obj = 0;
			v2_obj = 0;

			//forzar frenado
			TIM1->CCR1 = 1023;
			TIM1->CCR2 = 1023;
			TIM1->CCR3 = 1023;
			TIM1->CCR4 = 1023;

			//salir
			return;
		}

		//Extraer objetivos dados por el ESP32
		uint32_t objs = velObjetivos;
		int16_t target_1 = (int16_t)(objs & 0xFFFF);
		int16_t target_2 = (int16_t)(objs >> 16);

		//Cambio de objetivo Motores Izquierdos
		if (target_1 != v1_obj) {
			v1_obj = target_1;
			vel1_ini = velActual_1;
			indice_1 = 0;
		}
		//Cambio de objetivo Motores Derechos
		if (target_2 != v2_obj) {
			v2_obj = target_2;
			vel2_ini = velActual_2;
			indice_2 = 0;
		}

		// 3. Modulación del Motores Izquierdos
		if (indice_1 < 50) {
			int32_t dif1 = (int32_t)v1_obj - (int32_t)vel1_ini;
			velActual_1 = vel1_ini + (int16_t)((LUT_RAMPA[indice_1] * dif1) >> 10);
			indice_1++;
		} else {
			velActual_1 = v1_obj;
		}

		// 3. Modulación del Motores Derechos
		if (indice_2 < 50) {
			int32_t dif2 = (int32_t)v2_obj - (int32_t)vel2_ini;
			velActual_2 = vel2_ini + (int16_t)((LUT_RAMPA[indice_2] * dif2) >> 10);
			indice_2++;
		} else {
			velActual_2 = v2_obj;
		}

		//filtrado del votaje de la bateria

		//se verifica si es la primera vez que se lee el voltaje de la bateria
		if(batFiltrada == 0){
			batFiltrada = adc_buffer[4] << 4;
		}

		//filtro
		batFiltrada = batFiltrada - (batFiltrada >> 4) + (adc_buffer[4]);

		uint32_t batReal = batFiltrada >> 4;

		//valor adc de la bateria(falta tomar la lectura real)
		const uint16_t BAT_NOMINAL = 2500;

		//establecer limite para evitar division por 0
		uint32_t batSegura = (batReal < 100) ? 100 : batReal;

		//calculo del pwm compensado
		int32_t pwm1 = ((int32_t)((velActual_1 * (int32_t)BAT_NOMINAL) / (int32_t)batSegura));
		int32_t pwm2 = ((int32_t)((velActual_2 * (int32_t)BAT_NOMINAL) / (int32_t)batSegura));

		//evaluacion de limites
		if(pwm1 > 1023) {
			pwm1 = 1023;
		}
		if(pwm1 < -1023) {
			pwm1 = -1023;
		}
		if(pwm2 > 1023) {
			pwm2 = 1023;
		}
		if(pwm2 < -1023) {
			pwm2 = -1023;
		}

		// aplicacion de velocidades de los motores izquierdos
		if(pwm1 > 0) {
			TIM1->CCR1 = pwm1;
			TIM1->CCR2 = 0;
		} else if(pwm1 < 0) {
			TIM1->CCR1 = 0;
			TIM1->CCR2 = -pwm1;
		} else {
			TIM1->CCR1 = 1023;
			TIM1->CCR2 = 1023;
		}

		// aplicacion de velocidades de los motores derechos
		if(pwm2 > 0) {
			TIM1->CCR3 = pwm2;
			TIM1->CCR4 = 0;
		} else if(pwm2 < 0) {
			TIM1->CCR3 = 0;
			TIM1->CCR4 = -pwm2;
		} else {
			TIM1->CCR3 = 1023;
			TIM1->CCR4 = 1023;
		}
	}
  /* USER CODE END TIM6_DAC_IRQn 0 */
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/* USER CODE BEGIN 1 */
/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
	//accion de evasion de glitches
	if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4) != RESET)
	{
		//lismpiamos la bandera
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);

		//verificamos si termino la transferencia
		uint32_t cndtr = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_1);

		if(cndtr == 0 || cndtr == MAX_PACKET_SIZE || cndtr == MAX_PACKET_SIZE*2){
			return;
		}

		//desactivamos los canales DMA
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);

		//limpiamos la bandera de error del spi
		LL_DMA_ClearFlag_HT1(DMA1);
		LL_DMA_ClearFlag_TC1(DMA1);

		//forazamos que inicie donde debe el canal 1
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, (MAX_PACKET_SIZE*2));

		//reactivamos el canal
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
	}
}

void ADC1_2_IRQHandler(void)
{
	//validamos si hubo stall
	if(ADC1->ISR & ADC_ISR_AWD2){
		//accion de proteccion por sobrevoltaje
		TIM1->EGR |= TIM_EGR_BG;

		//verificar estado actual
		if (stall == 0) {
			//activamos la evasion del stall
			stall = 1;
			tiempo_st = ms;
			//notificamos al esp32
			GPIOC->BSRR = GPIO_PIN_9;
			//determinar la direccion del stall
			stall_dir1 = (velActual_1 > 0) ? -1023 : (velActual_1 < 0) ? 1023 : 0;
			stall_dir2 = (velActual_2 > 0) ? -1023 : (velActual_2 < 0) ? 1023 : 0;
		}
		//limpiar la bandera
		ADC1->ISR = ADC_ISR_AWD2;
	}
}
/* USER CODE END 1 */
