#include <Adafruit_TCS34725.h>
#include <Arduino.h>
#include <Musica.h>
#include <NewPing.h>
#include <Wire.h>
// variables que establecen el tiemṕo
unsigned long temp1 = 0;
unsigned long temp2 = 0;

// bandera(no recuerdo que hace)
int banc = 0;
// alerta del limite
SemaphoreHandle_t alerta;
// variables de control
bool estado = false;
bool start = false;
int modo = 0;
bool memo1 = false;
bool memo2 = false;
// variables de los pines(y la de distancia maxima)
int ini = 2;
int trig_1 = 13;
int echo_1 = 34;
int maxd = 40;
int trig_2 = 12;
int echo_2 = 35;
int led_1 = 19;
int sda_1 = 21;
int scl_1 = 22;
int led_2 = 5;
int sda_2 = 18;
int scl_2 = 23;
int ena_1 = 33; // quizas no se use
int ena_2 = 32;
int swi = 4;
int cal = 15;
int limCol = 200;
// se crean los objetos sc_1 y sc_2
Adafruit_TCS34725 sc_1 =
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 sc_2 =
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
// arreglo de los pines de los puentes h
int mot[2][2] = {{26, 25}, {14, 27}};
// se crean objetos ojos_1 y ojos_2
NewPing ojos_1(trig_1, echo_1, maxd);
NewPing ojos_2(trig_2, echo_2, maxd);
// valores establecidos del limite(pasar mas tarde a variables)
uint16_t lcr = 800, lcg = 700, lcb = 500;

// funcion que avanza en la direccion a(por definir en el robot fisico)
void dir_a() {
  alto();
  digitalWrite(mot[0][0], HIGH);
  digitalWrite(mot[1][0], HIGH);
}
// funcion que avanza en la direccion b(por definir en el robot fisico)
void dir_b() {
  digitalWrite(mot[1][1], HIGH);
  digitalWrite(mot[0][1], HIGH);
}
// gira al robot
void giro() {
  digitalWrite(mot[1][0], HIGH);
  digitalWrite(mot[0][1], HIGH);
}
// para el robot
void alto() {
  digitalWrite(mot[0][0], LOW);
  digitalWrite(mot[0][1], LOW);
  digitalWrite(mot[1][0], LOW);
  digitalWrite(mot[1][1], LOW);
}

// TAREA DE LA LOGICA DEL ROBOT

void robot(void *pvParameters) {
  // CORRECTO: Variables que necesitan "recordar" su valor van aquí.
  unsigned long temp1 = 0;
  unsigned long temp2 = 0;

  while (1) {
    // Espera el botón de inicio fuera del bucle principal de lógica.
    if (!start && digitalRead(ini) == HIGH) {
      start = true;
      modo = 6; // Establece un estado inicial claro al arrancar.
      memo1 = false;
      memo2 = false;
      auxilio();
    }

    // El bucle principal solo se ejecuta si start es true.
    while (start) {

      // Lectura de sensores al inicio de cada ciclo
      int dist_1 = ojos_1.ping_cm();
      int dist_2 = ojos_2.ping_cm();

      // ================================================================
      // MAQUINA DE ESTADOS: Lógica de Selección de Estado (Transiciones)
      // ================================================================

      // Primero, gestionamos las transiciones basadas en tiempo
      // Si estamos en modo "búsqueda tras perder" (2 o 3) y se acaba el tiempo,
      // pasamos a modo giro (6)
      if (modo == 2 && (millis() - temp1 >= 4000)) {
        modo = 6;
      } else if (modo == 3 && (millis() - temp2 >= 4000)) {
        modo = 6;
      }

      // Ahora, gestionamos las transiciones basadas en eventos (sensores)
      // Esta estructura 'if-else if' asegura que solo una condición se cumpla
      // por ciclo.

      // PRIORIDAD 1: Límite del dohyo
      if (xSemaphoreTake(alerta, 0) == pdTRUE) {
        modo = 0; // O el estado que corresponda a sc_1
      }
      // else if (/* condición para sensor de color 2 */) {
      //   modo = 1;
      // }

      // PRIORIDAD 2: Detectar al oponente
      else if (dist_1 != 0) {
        modo = 4; // Atacar en dirección A
      } else if (dist_2 != 0) {
        modo = 5; // Atacar en dirección B
      }

      // PRIORIDAD 3: Perder al oponente que se estaba siguiendo
      // Si lo veíamos con el sensor 1 (memo1) y ya no lo vemos (dist_1 == 0)
      else if (memo1 && dist_1 == 0) {
        modo = 2;      // Iniciar búsqueda temporal en dirección A
        memo1 = false; // Desactivamos la memoria para que esta condición no se
                       // repita
        temp1 = millis(); // INICIAMOS el temporizador de 4 segundos AHORA
      }
      // Si lo veíamos con el sensor 2 (memo2) y ya no lo vemos (dist_2 == 0)
      else if (memo2 && dist_2 == 0) {
        modo = 3;         // Iniciar búsqueda temporal en dirección B
        memo2 = false;    // Desactivamos la memoria
        temp2 = millis(); // INICIAMOS el temporizador de 4 segundos AHORA
      }

      // PRIORIDAD 4: No hay oponente a la vista y no se acaba de perder
      else {
        // Solo entra a modo 6 si no está ya en una búsqueda temporal (modos 2 o
        // 3)
        if (modo != 2 && modo != 3) {
          modo = 6; // Girar para buscar
        }
      }

      // ================================================================
      // MAQUINA DE ESTADOS: Ejecución de Acciones por Estado
      // ================================================================
      switch (modo) {
      case 0:    // Límite detectado por sc_1
        dir_b(); // Retrocede
        break;
      case 1:    // Límite detectado por sc_2
        dir_a(); // Retrocede
        break;
      case 2:    // Perdió al robot por ojos_1, avanza 4 seg
        dir_a(); // La transición fuera de este switch se encargará de cambiar
                 // de estado
        break;
      case 3:    // Perdió al robot por ojos_2, avanza 4 seg
        dir_b(); // La transición fuera de este switch se encargará de cambiar
                 // de estado
        break;
      case 4: // Detecta por ojos_1, avanza
        dir_a();
        memo1 = true;  // Recordamos que lo vimos con este sensor
        memo2 = false; // Apagamos la otra memoria para evitar conflictos
        break;
      case 5: // Detecta por ojos_2, avanza
        dir_b();
        memo2 = true;  // Recordamos que lo vimos con este sensor
        memo1 = false; // Apagamos la otra memoria
        break;
      case 6: // No detecta nada, buscar
        giro();
        memo1 = false; // Limpiamos memorias al empezar a buscar
        memo2 = false;
        break;
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    // Si por alguna razón start se vuelve false (ej. un botón de stop),
    // detenemos el robot.
    alto();
    vTaskDelay(50 / portTICK_PERIOD_MS); // Pequeña pausa
  }
}

// TAREA DE LOS SENSORES DE LOS SENSORES DE COLOR

void senColor(void *pvParameters) {
  while (1) {
    // variables de los colores detectados
    uint16_t r, g, b, c;
    // detecta si el sensor de color funciona bien
    if (!estado) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }
    // sc_1 lee el color
    sc_1.getRawData(&r, &g, &b, &c);
    // sc_1 determina si el color detectado es el mismo del limite
    long difCol = abs(r - lcr) + abs(g - lcg) + abs(b - lcb);
    if (difCol < limCol) {
      // manda alerta para alejarse del limite
      xSemaphoreGive(alerta);
    }
    vTaskDelay(20 / portTICK_PERIOD_MS);
    // digitalWrite(19, HIGH);
  }
}

// aun no hace nada
void calCol() {}

// setup
void setup() {
  // se crea la alerta
  alerta = xSemaphoreCreateBinary();
  // se inicializan los pines
  pinMode(echo_1, INPUT);
  pinMode(echo_2, INPUT);
  pinMode(trig_1, OUTPUT);
  pinMode(trig_2, OUTPUT);
  pinMode(led_1, OUTPUT);
  pinMode(sda_1, OUTPUT);
  pinMode(scl_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(sda_2, OUTPUT);
  pinMode(scl_2, OUTPUT);
  pinMode(ena_1, OUTPUT);
  pinMode(ena_2, OUTPUT);
  pinMode(ini, INPUT_PULLUP);
  pinMode(swi, INPUT);
  pinMode(cal, INPUT_PULLUP);
  pinMode(23, OUTPUT);
  // se inicializan los pines de los motores(puentes h)
  pinMode(mot[0][0], OUTPUT);
  pinMode(mot[0][1], OUTPUT);
  pinMode(mot[1][0], OUTPUT);
  pinMode(mot[1][1], OUTPUT);

  digitalWrite(led_1, HIGH);
  digitalWrite(led_2, HIGH);

  // verifica el funcionamiento de sc_1
  if (sc_1.begin()) {
    // todo bien
    estado = true;
  } else {
    // no funciona y desantiva su funcionamiento
    estado = false;
    // prende el led de sc_1
    digitalWrite(23, HIGH);
  }
  // aun no tiene propocito(determinara la calibracion)
  if (digitalRead(cal) == LOW) {
    calCol();
  } else {
    // valores predeterminados del sensor de color
    lcr = 800;
    lcg = 700;
    lcb = 500;
  }
  // se crean las tareas
  xTaskCreatePinnedToCore(robot, "robot", 1024, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(senColor, "sensorColor", 2048, NULL, 1, NULL, 0);
}
void loop() {
  // DESCRIPCIONES A TOMAR EN CUENTA:
  // ojos_1 y sc_1 en direccion "a"
  // ojos_2 y sc_2 en direccion "b"
}
