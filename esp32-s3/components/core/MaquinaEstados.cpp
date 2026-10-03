#include "MaquinaEstados.h"
#include "Nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <cstdint>

#include "Velocidades.h"
#include "Estados.h"
#include "Estrategia1.h"
#include "Estrategia2.h"
#include "Spi.h"

MaquinaEstados::MaquinaEstados(int _tiempo2, int _tiempo3, int _tiempo4, int _tiempo5, Spi* _spi):
	tempTL(0),
	tempE1(0),
	tempE2(0),
	tempS(0),
	tempEva(0),
	tiempo2(_tiempo2),
	tiempo3(_tiempo3),
	tiempo4(_tiempo4),
	tiempo5(_tiempo5),
	modo(29),
	estrategia(0),
	ciclo(0),
	ini(0),
	spi(_spi),
	memo_eva(0),
	memo_TC(0),
	memo_TL(0),
	memo_E(false)
{
	estE1 = new Estrategia1();
	estE2 = new Estrategia2();

	nvsLeer();
	switch(estrategia){
		case 0:
			estActual = estE1;
			break;
		case 1:
			estActual = estE2;
			break;
		default:
			estActual = estE1;
			break;
	}
}

MaquinaEstados::~MaquinaEstados() {
	delete estE1;
	delete estE2;
}

// se cuentan kis tiempos
void MaquinaEstados::tiempo(){
	unsigned long temp = (xTaskGetTickCount() * portTICK_PERIOD_MS);

	// condiciones que evaluan si ya pasaron los tiempos
	if (temp - tempTC >= (unsigned long)tiempo2) {
		//tiempo de memoria a corto plazo
		memo_TC = 0;
	}
    if (temp - tempTL >= (unsigned long)tiempo3) {
		//tiempo de la memoria a largo plazo
    	memo_TL = 0;
    }
}

void MaquinaEstados::logica(){
	//evaluar si esta en curso la maniobra de evacion
	if(evasion){
		spi->armarOrden(0, 0);
		return;
	}
	tiempo();
	if(estActual != nullptr){
		estActual->seleccion(this);
		estActual->ejecucion(this);
	}
}

void MaquinaEstados::nvsLeer(){
	Nvs nvs("tiempos");
	tiempo2 = nvs.leer("tof_corto_plazo",tiempo2);
	tiempo3 = nvs.leer("tof_largo_plazo",tiempo3);
	tiempo4 = nvs.leer("recta_star",tiempo4);
	tiempo5 = nvs.leer("giro_star",tiempo5);
	estrategia = nvs.leer("estrategia",estrategia);
}

void IRAM_ATTR MaquinaEstados::definirEvasion(bool _evasion){
	evasion = _evasion;
}

//metodo que recibe la duracion del ciclo
void MaquinaEstados::cicloR(int c, int i){
	ciclo = c;
	ini = i;
}

//metodo que entrega el estado actual
void MaquinaEstados::datos(int* _modo, int* _estra, int* _ciclo, int* _ini){
	 *_modo = modo;
	 *_estra = estrategia;
	 *_ciclo =ciclo;
	 *_ini = ini;
}
