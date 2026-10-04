#ifndef DATOS_H
#define DATOS_H

#include <cstdint>

struct Datos{
	//variabel del estado actual
	int estado;
	int estrategia;
	int inicio;
	int evasion;

	//variable de los sensores ToF
	uint16_t ToF1;
	uint16_t ToF2;
	uint16_t ToF3;
	uint16_t ToF4;
	uint16_t ToF5;
	uint16_t ToF6;

	//variables de la fiablilidad de los sensores ToF
	uint16_t estadoToF1;
	uint16_t estadoToF2;
	uint16_t estadoToF3;
	uint16_t estadoToF4;
	uint16_t estadoToF5;
	uint16_t estadoToF6;

	uint16_t señalTof1;
	uint16_t señalTof2;
	uint16_t señalTof3;
	uint16_t señalTof4;
	uint16_t señalTof5;
	uint16_t señalTof6;

	uint16_t ambienteToF1;
	uint16_t ambienteToF2;
	uint16_t ambienteToF3;
	uint16_t ambienteToF4;
	uint16_t ambienteToF5;
	uint16_t ambienteToF6;

	//variable de la pila
	uint16_t bateria;

	//variables de los motores
	int16_t pwm1;
	int16_t pwm2;
	uint16_t drv1;
	uint16_t drv2;
	uint16_t drv3;
	uint16_t drv4;

	//variables del sistema
	uint32_t tiempo;
	uint32_t heap;
	int wifi;
	int ciclo;

	// Variables de sensores TCRT
	uint16_t Tcrt1;
	uint16_t Tcrt2;
	uint16_t Tcrt3;
	uint16_t Tcrt4;
};

#endif
