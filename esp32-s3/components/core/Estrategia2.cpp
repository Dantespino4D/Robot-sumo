#include "Estrategia2.h"
#include "MaquinaEstados.h"
#include "Estados.h"
#include "Velocidades.h"

void Estrategia2::ejecucion(MaquinaEstados* ctx) {
	Estado com = ALTO;

	unsigned long temp = (xTaskGetTickCount() * portTICK_PERIOD_MS);

	ctx->spi->recolectar();

	switch (ctx->modo) {
        case 0:
            com = MAX_A;
			ctx->memo_TC = 9;
			ctx->tempTC = temp;
            break;
        case 1:
            com = ATAQUE_AI;
			ctx->memo_TC = 5;
			ctx->tempTC = temp;
			break;
        case 2:
            com = MAX_A;
			ctx->memo_TC = 9;
			ctx->tempTC = temp;
            break;
        case 3:
            com = ATAQUE_AD;
			ctx->memo_TC = 6;
			ctx->tempTC = temp;
            break;
        case 4:
            com = PRO_AI;
			ctx->memo_TC = 1;
			ctx->tempTC = temp;
			break;
        case 5:
            com = DIR_A;
			ctx->memo_TC = 9;
			ctx->tempTC = temp;
            break;
        case 6:
            com = PRO_AD;
			ctx->memo_TC = 2;
			ctx->tempTC = temp;
            break;
        case 7:
            com = MAX_B;
			ctx->memo_TC = 10;
			ctx->tempTC = temp;
            break;
        case 8:
            com = ATAQUE_BI;
			ctx->memo_TC = 7;
			ctx->tempTC = temp;
            break;
        case 9:
            com = MAX_B;
			ctx->memo_TC = 10;
			ctx->tempTC = temp;
            break;
        case 10:
            com = ATAQUE_BD;
			ctx->memo_TC = 8;
			ctx->tempTC = temp;
            break;
        case 11:
            com = PRO_BI;
			ctx->memo_TC = 3;
			ctx->tempTC = temp;
            break;
        case 12:
            com = DIR_B;
			ctx->memo_TC = 10;
			ctx->tempTC = temp;
            break;
        case 13:
            com = PRO_BD;
			ctx->memo_TC = 4;
			ctx->tempTC = temp;
            break;
		case 14:
			com = PRO_AI;
			ctx->memo_TL = 1;
			ctx->tempTL = temp;
			break;
		case 15:
			com = PRO_AD;
			ctx->memo_TL = 2;
			ctx->tempTL = temp;
			break;
		case 16:
			com = PRO_BI;
			ctx->memo_TL = 3;
			ctx->tempTL = temp;
			break;
		case 17:
			com = PRO_BD;
			ctx->memo_TL = 4;
			ctx->tempTL = temp;
			break;
		case 18:
			com = ATAQUE_AI;
			ctx->memo_TL = 1;
			ctx->tempTL = temp;
			break;
		case 19:
			com = ATAQUE_AD;
			ctx->memo_TL = 2;
			ctx->tempTL = temp;
			break;
		case 20:
			com = ATAQUE_BI;
			ctx->memo_TL = 3;
			ctx->tempTL = temp;
			break;
		case 21:
			com = ATAQUE_BD;
			ctx->memo_TL = 4;
			ctx->tempTL = temp;
			break;
		case 22:
			com = MAX_A;
			ctx->memo_TL = 0;
			break;
		case 23:
			com = MAX_B;
			ctx->memo_TL = 0;
			break;
		case 24:
			com = PRO_AD;
			break;
		case 25:
			com = PRO_AI;
			break;
		case 26:
			com = PRO_BD;
			break;
		case 27:
			com = PRO_BI;
			break;
		case 28:
			if (temp - ctx->tempE1 >= (unsigned long)ctx->tiempo4) {
				com = GIRO;
    			ctx->memo_E = false;
    			ctx->tempE2 = temp;
			}else{
      			com = DIR_A;
			}
      		break;
		case 29:
    		if(temp - ctx->tempE2 >= (unsigned long)ctx->tiempo5){
				com = DIR_A;
    		    ctx->memo_E = true;
    		    ctx->tempE1 = temp;
    		} else {
    		    com = GIRO;
    		}
            break;
        default:
            com = ALTO;
            break;
    }
	ctx->spi->armarOrden(vels_1[com], vels_2[com]);
}
