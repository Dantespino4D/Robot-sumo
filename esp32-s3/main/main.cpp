#include "sdkconfig.h"
#include "esp_task_wdt.h"
#include "Wifi.h"
#include "Mqtt.h"
#include "Spi.h"
#include "MaquinaEstados.h"
#include "Estados.h"
#include "Velocidades.h"
#include "GestorBorde.h"
#include "GestorI2C.h"
#include "SensorTof.h"
#include "Telemetria.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "rgb.h"
#include "Nvs.h"
#include "Musica.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <cstdint>
#include "configuracion.h"
#include "pines.h"
#include "eventos.h"

static const char* TAG = "main";

//event group para sincronizar tareas
EventGroupHandle_t eventos = NULL;

// variables de control
volatile bool start = false;
int32_t modo;

//objeto de Wifi
Wifi wi;

//objeto del protocolo MQTT
Mqtt mq;

//objeto que gestiona el bus I2C
GestorI2C i2c;

//objeto del SPI
Spi spi;

// objeto del sensor rival
SensorTof* sr = nullptr;

// puntero de la maquina de estados
MaquinaEstados *me = nullptr;

// objeto de la telemetria
Telemetria* tm = nullptr;

//handle de la tarea del robot
TaskHandle_t th_robot = NULL;

//buffer para el TCB
StaticTask_t tcbRobot;
StaticTask_t tcbMusica;
StaticTask_t tcbTelemetria;

//stack de las tareas
StackType_t stackRobot[4096];
StackType_t stackMusica[1024];
StackType_t stackTelemetria[10240];



//protoripos de las tareas
void robot(void *pvParameters);
void musica(void *pvParameters);
void telemetria(void *pvParameters);

//prototipos de las funciones
void begin();
void begin_hardware();
void comunicaciones();

// APP MAIN

extern "C" void app_main(void){
	begin();

    begin_hardware();

    comunicaciones();

  	// se crean las tareas
  	th_robot = xTaskCreateStaticPinnedToCore(robot, "robot", sizeof(stackRobot), NULL, 2, stackRobot, &tcbRobot, 1);
	xTaskCreateStaticPinnedToCore(musica, "musica", sizeof(stackMusica), NULL, 1, stackMusica, &tcbMusica, 0);

	ESP_LOGI(TAG, "se inicializo las tareas");
}


// LOGICA DEL ROBOT


void robot(void *pvParameters) {

	//CONFIGURACION WATCHDOG

	//suscribir la tarea al watchdog
	esp_err_t err = esp_task_wdt_add(NULL);
	if(err != ESP_OK){
		ESP_LOGE(TAG, "Error al suscribir al watchdog: %s", esp_err_to_name(err));
	}

	//BOTON Y ESPERA DE 5 SEGUNDOS

	esp_reset_reason_t rason = esp_reset_reason();
	bool watchdog = (rason == ESP_RST_TASK_WDT || rason == ESP_RST_WDT);
	if(!watchdog){
  		// prende al precionar el boton
  		while (gpio_get_level(INI) == 1) {
    		vTaskDelay(pdMS_TO_TICKS(100));
			esp_task_wdt_reset();
  		}

		//espera de 5 segundos
  		ESP_LOGI(TAG, "boton precionado");
		for(int i = 0; i < 50; i++){
            vTaskDelay(pdMS_TO_TICKS(100));
            esp_task_wdt_reset(); // <-- VITAL
        }
	}else{
		ESP_LOGW(TAG, "Reinicio por Watchdog detectado, omitiendo boton");
	}

	//INICIO DEL COMBATE

	//prende el led en verde para indicar que todo esta bien
	rgb(1023, 0);
	ESP_LOGI(TAG,"iniciando combate");

	//la variable star la cual activa las tareas
  	start = true;

	//variables del tiempo freertos
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(1);

	//bucle del robot
	while (true) {
		// inicia

		//espera hasta el siguiente ciclo
		vTaskDelayUntil(&xLastWakeTime, xFrequency);

		// se resetea el watchdog
		esp_task_wdt_reset();

	  	uint64_t Tini = esp_timer_get_time();

    	// MAQUINA DE ESTADOS
    	me->logica();

		//se calcula y envia la duracion de un ciclo
		uint64_t Tfin = esp_timer_get_time();
		int ciclo = (int)((Tfin - Tini)/1000);
		me->cicloR(ciclo, 1);
  	}
}


// MUSICA


void musica(void *pvParameters) {
  	while (true) {
    	while (start) {
			//se toca la musica
    		adestes();
      		vTaskDelay(10);
    	}
    	vTaskDelay(10);
  	}
}


// TELEMETRIA


void telemetria(void *pvParameters){
	while(true){
		//se manda la informacion actual para su procesamiento y analisis
		tm->enviar();
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}



// funcion que inicializa el sistema
void begin() {
	//inicializar la memoria nvs personalizada
	esp_err_t err = nvs_flash_init_partition("configuracion");
	if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND){
		ESP_ERROR_CHECK(nvs_flash_erase_partition("configuracion"));
      	err = nvs_flash_init_partition("configuracion");
	}
	ESP_ERROR_CHECK(err);

	//inicializar el monitor del sistema
	err = nvs_flash_init();
	if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND){
		ESP_ERROR_CHECK(nvs_flash_erase());
      	err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

	//se lee las instrucciones del monitor serial
	Nvs sys("sistema");

	//se crea el event group
	eventos = xEventGroupCreate();
	if(eventos == NULL){
		ESP_LOGE(TAG, "Error al crear el event group");
	}

	//se inicializan pines input pullup
	gpio_config_t io_conf_input;
	io_conf_input.pin_bit_mask = (1ULL << INI);
	io_conf_input.mode = GPIO_MODE_INPUT;
	io_conf_input.pull_up_en = GPIO_PULLUP_ENABLE;
	io_conf_input.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf_input.intr_type = GPIO_INTR_DISABLE;
	gpio_config(&io_conf_input);

	vTaskDelay(pdMS_TO_TICKS(100));

	if(gpio_get_level(INI) == 0){
		ESP_LOGI(TAG, "modo de prueba activado por boton");
		sys.guardar("modo", 0);
		modo = 0;
	}else{
        modo = sys.leer("modo", 1);
	}
	int32_t val = sys.leer("monitor", 2);
	//se aplica el valor elegido
    switch(val){
		case 0:
			ESP_LOGI(TAG, "desactivando monitor");
			esp_log_level_set("*", ESP_LOG_NONE);
			break;
        case 1:
			ESP_LOGI(TAG, "monitor solo errores");
			esp_log_level_set("*", ESP_LOG_ERROR);
			break;
		case 2:
			ESP_LOGI(TAG, "monitor solo info");
			esp_log_level_set("*", ESP_LOG_INFO);
			break;
		case 3:
			ESP_LOGI(TAG, "monitor todo");
			esp_log_level_set("*", ESP_LOG_VERBOSE);
			break;
		default:
			ESP_LOGI(TAG, "monitor solo info");
			esp_log_level_set("*", ESP_LOG_INFO);
			break;
	}


  	pwm_rgb();
  	rgb(1023, 800);
}

//inicializacion del hardware
void begin_hardware() {
  	//ajustes iniciales
    ESP_LOGI(TAG, "Iniciando hardware...");
  	i2c.begin();

  	//pin de la musica
  	pinMus(MUS);

	//se le asigna al puntero los el objeto correspondiente
    sr = new SensorTof(i2c, dir, maxd);

    if (sr != nullptr) {
        sr->begin();
    } else {
        ESP_LOGE(TAG, "No se pudo crear el sensor rival");
    }

	//objeto que maneja la memoria NVS de las velocidades
	Nvs vel("motores");

	//velocidades de avance normal
	vels_1[DIR_A] = vel.leer("velocidad_nI", vels_1[DIR_A]);
	vels_2[DIR_A] = vel.leer("velocidad_nD", vels_2[DIR_A]);
	vels_1[DIR_B] = -vel.leer("velocidad_nI", vels_1[DIR_A]);
	vels_2[DIR_B] = -vel.leer("velocidad_nD", vels_2[DIR_A]);

	//velocidades de ataque con giro a la izquierda
	vels_1[ATAQUE_AI] = vel.leer("velocidad_aI", vels_1[ATAQUE_AI]);
	vels_2[ATAQUE_AI] = vel.leer("velocidad_aD", vels_2[ATAQUE_AI]);
	vels_1[ATAQUE_BI] = -vel.leer("velocidad_aI", vels_1[ATAQUE_AI]);
	vels_2[ATAQUE_BI] = -vel.leer("velocidad_aD", vels_2[ATAQUE_AI]);

	//velocidades de ataque con giro a la derecha
	vels_1[ATAQUE_AD] = vel.leer("velocidad_aD", vels_1[ATAQUE_AD]);
	vels_2[ATAQUE_AD] = vel.leer("velocidad_aI", vels_2[ATAQUE_AD]);
	vels_1[ATAQUE_BD] = -vel.leer("velocidad_aD", vels_1[ATAQUE_AD]);
	vels_2[ATAQUE_BD] = -vel.leer("velocidad_aI", vels_2[ATAQUE_AD]);

	//velocidades de ataque con giro pronunciado a la izquierda
	vels_1[PRO_AI] = vel.leer("velocidad_pI", vels_1[PRO_AI]);
	vels_2[PRO_AI] = vel.leer("velocidad_pD", vels_2[PRO_AI]);
	vels_1[PRO_BI] = -vel.leer("velocidad_pI", vels_1[PRO_AI]);
	vels_2[PRO_BI] = -vel.leer("velocidad_pD", vels_2[PRO_AI]);

	//velocidades de ataque con giro pronunciado a la derecha
	vels_1[PRO_AD] = vel.leer("velocidad_pD", vels_1[PRO_AD]);
	vels_2[PRO_AD] = vel.leer("velocidad_pI", vels_2[PRO_AD]);
	vels_1[PRO_BD] = -vel.leer("velocidad_pD", vels_1[PRO_AD]);
	vels_2[PRO_BD] = -vel.leer("velocidad_pI", vels_2[PRO_AD]);

	//velocidades de avance a maxima velocidad
	vels_1[MAX_A] = vel.leer("velocidad_mI", vels_1[MAX_A]);
	vels_2[MAX_A] = vel.leer("velocidad_mD", vels_2[MAX_A]);
	vels_1[MAX_B] = -vel.leer("velocidad_mI", vels_1[MAX_A]);
	vels_2[MAX_B] = -vel.leer("velocidad_mD", vels_2[MAX_A]);


	//velocidades del giro de busqueda
	vels_1[GIRO] = vel.leer("velocidad_gI", vels_1[GIRO]);
	vels_2[GIRO] = vel.leer("velocidad_gD", vels_2[GIRO]);

	//se inicializa la maquina de estados
    me = new MaquinaEstados(tiempo2, tiempo3, tiempo4, tiempo5, &spi);

	//se inicializa el gestor de border
	GestorBorde gb;
	gb.begin(me);
	ESP_LOGI(TAG, "se inicializo todo");
}

void comunicaciones() {
	if(modo == 0){
		//modo de prueba
        ESP_LOGI(TAG, "Modo 0: Test y Telemetria");

		//inicializar el tcp/ip
		esp_netif_init();
		esp_event_loop_create_default();

		//se inicializa el wifi y el MQTT
 		wi.begin();
		wi.espera();
 		mq.begin();

		tm = new Telemetria(me, &mq, &wi);

		//tarea de telemetria
		xTaskCreateStaticPinnedToCore(telemetria, "telemetria", sizeof(stackTelemetria), NULL, 1, stackTelemetria, &tcbTelemetria, 0);
	}else{
		ESP_LOGI(TAG, "monitor desactivado por modo combate");
		esp_log_level_set("*", ESP_LOG_NONE);
	}
	spi.begin();
	spi.enviarConfiguracion();
}

// funcion que libera la memoria de los objetos creados
void limpiar_memoria() {
    if (tm != nullptr) { delete tm; tm = nullptr; }
    if (me != nullptr) { delete me; me = nullptr; }
    ESP_LOGI(TAG, "Memoria de objetos liberada.");
}
