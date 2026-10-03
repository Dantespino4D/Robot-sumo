#include "EstrategiaEstandar.h"
#include "eventos.h"
#include "MaquinaEstados.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

//selecciona el estado
void EstrategiaEstandar::seleccion(MaquinaEstados* ctx) {
	//se reciben los bits de los sensores
	uint32_t noti = 0;
	noti = xEventGroupGetBits(eventos);
	//se detecta si los sensores 1, 2, y 3 o sus convinaciones fueron activados
	if(noti & MASK_TOF_A) {
		//si detecta al robot por ToF1, ToF2 y ToF3
		if((noti & MASK_TOF_A) == MASK_TOF_A) {
			ctx->modo = 0;
		}
		//si detecta al robot por ToF1 y Tof2
		else if((noti & MASK_TOF_1_2) == MASK_TOF_1_2) {
			ctx->modo = 1;
		}
		//si detecta al robot por ToF1 y ToF3
		else if((noti & MASK_TOF_1_3) == MASK_TOF_1_3) {
			ctx->modo = 2;
		}
		//si detecta al robot por ToF2 y ToF3
		else if((noti & MASK_TOF_2_3) == MASK_TOF_2_3) {
			ctx->modo = 3;
		}
		// si detecta el robot por ToF1
		else if (noti & BIT_TOF_AI) {
			ctx->modo = 4;
		}
		// si detecta el robot por ToF2
		else if (noti & BIT_TOF_AC) {
			ctx->modo = 5;
		}
		//si detecta al robot por ToF3
		else if (noti & BIT_TOF_AD) {
		  	ctx->modo = 6;
		}
	//detecta si los sensores 4, 5, y 6 o sus convinaciones fueron activados
	}else if(noti & MASK_TOF_B) {
		//si detecta al robot por ToF4, ToF5 y ToF6
		if((noti & MASK_TOF_B) == MASK_TOF_B) {
			ctx->modo = 7;
		}
		//si detecta al robot por ToF4 y ToF5
		else if((noti & MASK_TOF_4_5) == MASK_TOF_4_5) {
			ctx->modo = 8;
		}
		//si detecta al robot por ToF4 y ToF6
		else if((noti & MASK_TOF_4_6) == MASK_TOF_4_6) {
			ctx->modo = 9;
		}
		//si detecta al robot por ToF5 y ToF6
		else if((noti & MASK_TOF_5_6) == MASK_TOF_5_6) {
			ctx->modo = 10;
		}
		// si detecta al robot por ToF4
		else if (noti & BIT_TOF_BI) {
		  	ctx->modo = 11;
		}
		// si detecta al robot por ToF5
		else if (noti & BIT_TOF_BC) {
			ctx->modo = 12;
		}
		//si detecta al robot por ToF6
		else if (noti & BIT_TOF_BD) {
			ctx->modo = 13;
		}
	}else if(ctx->memo_TC != 0 || ctx->memo_TL != 0) {
		//si deja de detectar al robot por direccion AI_P a corto plazo
		if (ctx->memo_TC == 1) {
			ctx->modo = 14;
		}
		//si deja de detectar al robot por direccion AD_P a corto plazo
		else if (ctx->memo_TC == 2) {
			ctx->modo = 15;
		}
		//si deja de detectar al robot por direccion BI_P a corto plazo
		else if (ctx->memo_TC == 3) {
			ctx->modo = 16;
		}
		//si deja de detectar al robot por direccion BD_P a corto plazo
		else if (ctx->memo_TC == 4) {
			ctx->modo = 17;
		}
		//si deja de detectar al robot por direccion AI_S a corto plazo
		else if (ctx->memo_TC == 5) {
			ctx->modo = 18;
		}
		//si deja de detectar al robot por direccion AD_S a corto plazo
		else if (ctx->memo_TC == 6) {
			ctx->modo = 19;
		}
		//si deja de detectar al robot por direccion BI_S a corto plazo
		else if (ctx->memo_TC == 7) {
			ctx->modo = 20;
		}
		//si deja de detectar al robot por direccion BD_S a corto plazo
		else if (ctx->memo_TC == 8) {
			ctx->modo = 21;
		}
		//si deja de detectar al robot de frente a corto plazo
		else if (ctx->memo_TC == 9) {
			ctx->modo = 22;
		}
		//si deja de detectar al robot por atras a corto plazo
		else if (ctx->memo_TC == 10) {
			ctx->modo = 23;
		}
		//si deja de detectar al robot por direccion AI a largo plazo
		else if (ctx->memo_TL == 1) {
		  	ctx->modo = 24;
		}
		//si deja de detectar al robot por direccion AD a largo plazo
		else if(ctx->memo_TL == 2) {
		  	ctx->modo = 25;
		}
		//si deja de detectar al robot por direccion BI a largo plazo
		else if (ctx->memo_TL == 3) {
			ctx->modo = 26;
		}
		//si deja de detectar al robot por direccion BD a largo plazo
		else if (ctx->memo_TL == 4) {
		  	ctx->modo = 27;
		}
	}else{
		//memoria de avance de estrella
		if (ctx->memo_E) {
			ctx->modo = 28;
		}
		// si no detecta nada
		else{
			ctx->modo = 29;
		}
	}
}
