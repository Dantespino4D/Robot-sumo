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
	d.evasion = me->getEvasion() ? 1 : 0;

	wf->signalW(&d.wifi);
	d.tiempo = xTaskGetTickCount() * portTICK_PERIOD_MS;
	d.heap = esp_get_free_heap_size();
}

void Telemetria::enviar(){
	recopilar();

	char json[NJSON];

	// JSON estricto. Cero ramas condicionales. Cero variables de prototipo o sensores inútiles.
	int lon = snprintf(json, NJSON,
		"{\"sistema\":{\"commit\":%d,\"tiempo\":%" PRIu32 ",\"heap\":%" PRIu32 ",\"bateria\":%u,\"wifi\":%d,\"evasion\":%d,\"ciclo\":%d},"
		"\"estado\":{\"modo\":%d,\"estrategia\":%d,\"inicio\":%d},"
		"\"motores\":{\"pwm_izq\":%d,\"pwm_der\":%d,\"drv1\":%u,\"drv2\":%u,\"drv3\":%u,\"drv4\":%u},"
		"\"sensores\":{\"tof\":[%u,%u,%u,%u,%u,%u],\"f_estado\":[%u,%u,%u,%u,%u,%u]}}",
		COMMIT, d.tiempo, d.heap, (unsigned)d.bateria, d.wifi, d.evasion, d.ciclo,
		d.estado, d.estrategia, d.inicio,
		d.pwm1, d.pwm2, (unsigned)d.drv1, (unsigned)d.drv2, (unsigned)d.drv3, (unsigned)d.drv4,
		(unsigned)d.ToF1, (unsigned)d.ToF2, (unsigned)d.ToF3, (unsigned)d.ToF4, (unsigned)d.ToF5, (unsigned)d.ToF6,
		(unsigned)d.estadoToF1, (unsigned)d.estadoToF2, (unsigned)d.estadoToF3, (unsigned)d.estadoToF4, (unsigned)d.estadoToF5, (unsigned)d.estadoToF6
	);

	if(lon > 0){
		mq->pub(json, "robot/telemetria", 0, 0);
	}
}
