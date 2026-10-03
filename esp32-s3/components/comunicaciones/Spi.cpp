#include "Spi.h"
#include "Nvs.h"
#include "Velocidades.h"
#include "pines.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstdint>
#include <string.h>
#include "protocolo/sumo_protocol.h"
#include "protocolo/Crc8Table.h"

#define TAG "spi"

Spi::Spi():
	telemetria{},
	orden{},
	ok{},
	conf{},
	stm32_handle(nullptr),
	transaccionEnCurso(false),
	cont(0),
	fallos(0)
{
	tx = (uint8_t*)heap_caps_malloc(MAX_PACKET_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
	rx = (uint8_t*)heap_caps_malloc(MAX_PACKET_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
	memset(&transmision, 0, sizeof(spi_transaction_t));
	memset(&spistm32, 0, sizeof(spi_device_interface_config_t));
	if(tx == nullptr || rx == nullptr){
		ESP_LOGE(TAG, "error al asignar memoria para buffers SPI");
		abort();
	}
}

void Spi::begin() {
    spi_bus_config_t spibus = {};
    spibus.mosi_io_num = SPI_MOSI;
    spibus.miso_io_num = SPI_MISO;
    spibus.sclk_io_num = SPI_CLK;
    spibus.quadwp_io_num = -1;
    spibus.quadhd_io_num = -1;
    spibus.max_transfer_sz = MAX_PACKET_SIZE;

    spistm32 = {};
    spistm32.clock_speed_hz = 10000000;
    spistm32.mode = 0;
    spistm32.spics_io_num = SPI_CS;
    spistm32.queue_size = 7;

    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &spibus, SPI_DMA_CH_AUTO);
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "error al configurar el bus SPI");
    }

    ret = spi_bus_add_device(SPI2_HOST, &spistm32, &stm32_handle);
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "error al configurar el dispositivo SPI");
    }
}

void Spi::armarOrden(int16_t obj_1, int16_t obj_2){
	Esp_t* orden = (Esp_t*)tx;
	orden->inicio[0] = HEADER_1;
	orden->inicio[1] = HEADER_2;
	orden->inicio[2] = HEADER_3;
	orden->id = ID_ESP;
	orden->obj_1 = obj_1;
	orden->obj_2 = obj_2;
	orden->banderas = 0;//quizas se elminara si se encuentra inutil
	orden->cont = cont;
	orden->final = crc8((uint8_t*)orden, sizeof(Esp_t) - 1);
	enviarRecibir((uint8_t*)orden, rx, sizeof(Esp_t));
	cont++;
}

void Spi::enviarRecibir(uint8_t* mensaje, uint8_t* respuesta, size_t size){
	if(transaccionEnCurso){
		return;
	}
	transmision.length = size * 8;
	transmision.tx_buffer = mensaje;
	transmision.rx_buffer = respuesta;
	esp_err_t err = spi_device_queue_trans(stm32_handle, &transmision, 0);
	if(err == ESP_OK){
		transaccionEnCurso = true;
	}else{
		fallos++;
	}
}

void Spi::enviarConfiguracion(){
	Nvs sensores("sensores");
	Nvs tiempos("tiempos");
	Conf_t* conf = (Conf_t*)tx;
	conf->inicio[0] = HEADER_1;
	conf->inicio[1] = HEADER_2;
	conf->inicio[2] = HEADER_3;
	conf->id = ID_CONF;
	conf->u_limite = sensores.leer("umbral_color", 1000);//valor por defecto a definir en el futuro
	conf->t_ret = tiempos.leer("retroceso", 1000);//valor por defecto a definir en el futuro
	conf->final = crc8((uint8_t*)conf, sizeof(Conf_t) - 1);
	enviarRecibir((uint8_t*)conf, rx, sizeof(Conf_t));
}

void Spi::procesarRespuesta(){
	if(rx == nullptr){
		fallos++;
		return;
	}
	if(rx[0] != HEADER_1 || rx[1] != HEADER_2 || rx[2] != HEADER_3){
		fallos++;
		return;
	}
	if(rx[3] == ID_STM){
		Stm_t* mensaje = (Stm_t*)rx;
		if(mensaje->final != crc8((uint8_t*)mensaje, sizeof(Stm_t) - 1)){
			fallos++;
			return;
		}
		fallos = 0;
		telemetria = *mensaje;
	}else if(rx[3] == ID_OK){
		Ok_t* mensaje = (Ok_t*)rx;
		if(mensaje->final != crc8((uint8_t*)mensaje, sizeof(Ok_t) - 1)){
			fallos++;
			return;
		}
		fallos = 0;
		ok = *mensaje;
	}else{
		fallos++;
	}
}

void Spi::recolectar(){
	if(!transaccionEnCurso){
		return;
	}

	spi_transaction_t* tran;

	esp_err_t err = spi_device_get_trans_result(stm32_handle, &tran, 0);
	if(err == ESP_ERR_TIMEOUT){
		return;
	}if(err != ESP_OK){
		fallos++;
		transaccionEnCurso = false;
		return;
	}else{
		transaccionEnCurso = false;
		procesarRespuesta();
	}
}

Stm_t Spi::getTelemetria() const{
	return telemetria;
}
