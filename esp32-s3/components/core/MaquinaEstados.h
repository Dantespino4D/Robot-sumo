#ifndef MAQUINAESTADOS_H
#define MAQUINAESTADOS_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "EstrategiaBase.h"
#include "Spi.h"

// comandos de movimiento
#include <cstdint>

class Estrategia1;
class Estrategia2;

class MaquinaEstados{
	friend class EstrategiaEstandar;
	friend class Estrategia1;
	friend class Estrategia2;
	private:
		// variables que cuentan el tiempo
		unsigned long tempTC;
		unsigned long tempTL;
		unsigned long tempE1;
		unsigned long tempE2;
		unsigned long tempS;
		unsigned long tempEva;

		//tiempos establecidos
		int tiempo2;
		int tiempo3;
		int tiempo4;
		int tiempo5;
		private:
		//variables de control
		volatile int modo;
		volatile int estrategia;
		volatile int ciclo;
		int ini;

		Spi* spi;
		//variable de la evasion
		volatile bool evasion;

		int memo_eva;

		int memo_TC;//control de los ToF a corto plazo
				   //0 = sin memoria, 1 = AI pro, 2 = AD pro, 3 = BI pro, 4 = BD pro, 5 = AI suave, 6 = AD suave, 7 = BI suave, 8 = BD suave, 9 = frente, 10 = atras

		int memo_TL;//control de los ToF a largo plazo
			   //0 = sin memoria, 1 = AI, 2 = AD, 3 = BI, 4 = BD

		bool memo_E;//contol estrella

		// Instancias de estrategias
		Estrategia1* estE1;
		Estrategia2* estE2;

		//puntero de la estrategia actual
		EstrategiaBase* estActual;

		//metodo de validacion del tiempo
		void tiempo();

		//lee los datos de la nvs
		void nvsLeer();

	public:
		//constructor
		MaquinaEstados(int _tiempo2, int _tiempo3, int tiempo4, int _tiempo5, Spi* spi);

		//destructor
		~MaquinaEstados();

		//bloqueo de copia
		MaquinaEstados(const MaquinaEstados&) = delete;
		MaquinaEstados& operator=(const MaquinaEstados&) = delete;

		//maquina de estados
		void logica();

		//metodo que define la evasion
		void definirEvasion(bool _evasion);

		bool getEvasion() const { return evasion; }

		//se obtiene el ciclo
		void cicloR(int c, int i);

		//metodo que entrega el estado actual
		void datos(int* _modo, int* _estra, int* _ciclo, int* _ini);
};
#endif // !DEBUG
