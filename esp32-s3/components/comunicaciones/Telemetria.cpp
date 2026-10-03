#include "Telemetria.h"
#include "DatosT.h"
#include "Mqtt.h"
#include "MaquinaEstados.h"
#include "Wifi.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstdio>
#include <inttypes.h>

#ifndef COMMIT
#define COMMIT 0
#endif

// Constructor limpio, sin dependencias de hardware esclavo
Telemetria::Telemetria(MaquinaEstados* e, Mqtt* q, Wifi* w):
	d{},
	me(e),
	mq(q),
	wf(w)
{}

void Telemetria::recopilar(){
	me->datos(&d.estado, &d.estrategia, &d.ciclo, &d.inicio);
	d.stall = me->getEvasion() ? 1 : 0;

	wf->signalW(&d.wifi);
	d.tiempo = xTaskGetTickCount() * portTICK_PERIOD_MS;
	d.heap = esp_get_free_heap_size();
}

void Telemetria::enviar(){
	recopilar();

	char json[NJSON];

	// JSON estricto. Cero ramas condicionales. Cero variables de prototipo o sensores inútiles.
	int lon = snprintf(json, NJSON,
		"{\"sistema\":{\"commit\":%d,\"tiempo\":%" PRIu32 ",\"heap\":%" PRIu32 ",\"pila\":%f,\"wifi\":%d,\"ciclo\":%d},\"estado\":{\"modo\":%d,\"estrategia\":%d,\"inicio\":%d},\"stm32\":{\"pwm_izq\":%d,\"pwm_der\":%d,\"stall\":%d,\"corriente\":%f},\"sensores\":{\"tof\":[%d,%d,%d,%d,%d,%d],\"f_estado\":[%d,%d,%d,%d,%d,%d]}}",
		COMMIT, d.tiempo, d.heap, d.pila, d.wifi, d.ciclo,
		d.estado, d.estrategia, d.inicio,
		d.pwm1, d.pwm2, d.stall, d.corriente,
		d.ToF1, d.ToF2, d.ToF3, d.ToF4, d.ToF5, d.ToF6,
		d.estadoToF1, d.estadoToF2, d.estadoToF3, d.estadoToF4, d.estadoToF5, d.estadoToF6
	);

	if(lon > 0){
		mq->pub(json, "robot/telemetria", 0, 0);
	}
}
