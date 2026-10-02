#include "stm32g474xx.h"
#include "Spi.h"
#include "protocolo/sumo_protocol.h"
#include "protocolo/Crc8Table.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_spi.h"
#include "stm32g4xx_ll_crc.h"
#include "stm32g4xx_ll_dac.h"
#include "stm32g4xx_ll_tim.h"
#include <cstddef>
#include <cstdint>

volatile uint32_t velObjetivos = 0;
volatile uint16_t adc_buffer[5] = {0};
extern uint16_t TRetroceso;

Spi::Spi() :
	bufferA{},
	bufferB{},
	cont(0),
	bufferCpu(bufferB),
	bufferDma(bufferA)
{}

void Spi::begin() {
	recibir();
}

//se calcula el checksum
uint8_t Spi::checksum(uint8_t* paquete, size_t tamaño){
	LL_CRC_ResetCRCCalculationUnit(CRC);
	for (size_t i = 0; i < tamaño - 1; i++){
		LL_CRC_FeedData8(CRC, paquete[i]);
	}
	return LL_CRC_ReadData8(CRC);
}

//recibir reporte
void Spi::armarReporte(){
	static Stm_t reporte{};
	reporte.inicio[0] = HEADER_1;
	reporte.inicio[1] = HEADER_2;
	reporte.inicio[2] = HEADER_3;
	reporte.id = ID_STM;
	reporte.tcrt_1 = (COMP1->CSR & COMP_CSR_VALUE) ? 1 : 0;
	reporte.tcrt_2 = (COMP2->CSR & COMP_CSR_VALUE) ? 1 : 0;
	reporte.tcrt_3 = (COMP3->CSR & COMP_CSR_VALUE) ? 1 : 0;
	reporte.tcrt_4 = (COMP4->CSR & COMP_CSR_VALUE) ? 1 : 0;
	reporte.drv_1 = adc_buffer[0];
	reporte.drv_2 = adc_buffer[1];
	reporte.drv_3 = adc_buffer[2];
	reporte.drv_4 = adc_buffer[3];
	reporte.bateria = adc_buffer[4];
	reporte.cont = cont;
	reporte.final = crc8((uint8_t*)&reporte, sizeof(Stm_t) - 1);
	cont++;
	enviar((uint8_t*)&reporte, sizeof(Stm_t));
}

uint8_t Spi::recibirReporte(uint8_t *reporte){
	if(reporte == nullptr){
		return 0;
	}
	if(reporte[0] != HEADER_1 || reporte[1] != HEADER_2 || reporte[2] != HEADER_3){
		//reporte invalido
		return 0;
	}
	if(reporte[3] == ID_CONF){
		Conf_t* conf = (Conf_t*)reporte;
		if(checksum(reporte, sizeof(Conf_t)) != conf->final){
			return 0;
		}

		//aplicacion del umbral del limite
		LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, conf->u_limite);
		LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_2, conf->u_limite);

		//tiempo de retroceso
		TRetroceso = conf->t_ret;

		static Ok_t ok{};
		ok.inicio[0] = HEADER_1;
		ok.inicio[1] = HEADER_2;
		ok.inicio[2] = HEADER_3;
		ok.id = ID_OK;
		ok.final = checksum((uint8_t*)&ok, sizeof(Ok_t));
		enviar((uint8_t*)&ok, sizeof(Ok_t));
		return ID_CONF;
	}else if(reporte[3] == ID_ESP){
		Esp_t* accion = (Esp_t*)reporte;
		if(checksum(reporte, sizeof(Esp_t)) != accion->final){
			return 0;
		}
		if(accion->banderas & UNLOCK_M){
			//apagamos el pin de interrupcion
			GPIOC->BSRR = GPIO_PIN_9 << 16;
		}
		velObjetivos = ((uint32_t)(uint16_t)accion->obj_2 << 16) | (uint32_t)(uint16_t)accion->obj_1;

		//se envia la telemetria como respuesta
		armarReporte();
		return ID_ESP;
	}
	return 0;
}

//envia mensajea
void Spi::enviar(uint8_t *data, uint16_t size) {
	if(LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_2)){
		return;
	}
	//se deshabilitan las interrupciones
	__disable_irq();

	//desabilitar el SPI
	LL_SPI_Disable(SPI1);

	//limpiar RX
	while(LL_SPI_GetRxFIFOLevel(SPI1) != LL_SPI_RX_FIFO_EMPTY){
		(void)LL_SPI_ReceiveData8(SPI1);
	}

	//se deshabilita el canal de transmision
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)data);
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, size);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
	LL_SPI_EnableDMAReq_TX(SPI1);
	LL_SPI_Enable(SPI1);

	//se habilitan las interrupciones
	__enable_irq();
}

//recibe mensjes
void Spi::recibir() {
	if(LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_1)){
		return;
	}
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)bufferDma);
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, (MAX_PACKET_SIZE*2));
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
	LL_SPI_EnableDMAReq_RX(SPI1);
	LL_SPI_Enable(SPI1);
}

Spi spi;

extern "C" void SPI_DMA_RX_Callback(void) {
	uint8_t* paquete = nullptr;
	if (LL_DMA_IsActiveFlag_HT1(DMA1)) {
		LL_DMA_ClearFlag_HT1(DMA1);
		paquete = (uint8_t*)&spi.bufferDma[0];
		spi.recibirReporte(paquete);
	}else if (LL_DMA_IsActiveFlag_TC1(DMA1)) {
		LL_DMA_ClearFlag_TC1(DMA1);
		paquete = (uint8_t*)&spi.bufferDma[MAX_PACKET_SIZE];
		spi.recibirReporte(paquete);
	}
	if (LL_DMA_IsActiveFlag_TE1(DMA1)) {
			LL_DMA_ClearFlag_TE1(DMA1);
	}
}
