#include <Adafruit_TCS34725.h>
#include <Arduino.h>
#include <Musica.h>
#include <NewPing.h>
#include <Wire.h>
// variables que establecen el tiemṕo
// unsigned long temp1 = 0;
// unsigned long temp2 = 0;

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
  alto();
  digitalWrite(mot[1][1], HIGH);
  digitalWrite(mot[0][1], HIGH);
}
// gira al robot
void giro() {
  alto();
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
  while (1) {
    unsigned long temp1 = 0;
    unsigned long temp2 = 0;

    // prende al precionar el boton
    if (digitalRead(ini) == HIGH) {
      start = true;
      auxilio();
    }
    // inicia
    while (start) {

      int dist_1 = ojos_1.ping_cm();
      int dist_2 = ojos_2.ping_cm();
      if (modo == 4) {
        temp1 = millis();
      }
      if (modo == 5) {
        temp2 = millis();
      }
      if (millis() - temp1 >= 4000) {
        memo1 = false;
      }
      if (millis() - temp2 >= 4000) {
        memo2 = false;
      }

      // MAQUINA DE ESTADOS

      // selecciona el estado

      // si detecta el limite por sc_1
      if (xSemaphoreTake(alerta, 0) == pdTRUE) {
        modo = 0;
      }
      // si detecta el limite por sc_2
      else if (false) {
        modo = 1;
      }
      // si deja de detectar al robot por ojos 1
      else if (memo1 && dist_1 == 0) {
        modo = 2;
      }
      // si deja de detectar al robot por ojos 2
      else if (memo2 && dist_2 == 0) {
        modo = 3;
      }
      // si detecta el robot por ojos 1
      else if (dist_1 != 0) {
        modo = 4;
      }
      // si detecta el robot por ojos 2
      else if (dist_2 != 0) {
        modo = 5;
      }
      // si no detecta nada
      else {
        modo = 6;
      }

      // ejecuta el estado
      switch (modo) {
      // detiene el movimiento y retrocede en direccion b
      case 0:
        dir_b();
        break;
      // detiene el movimiento y retrocede en direccion a
      case 1:
        dir_a();
        break;
      // avanza por un tiempo definido de 4 segundo en direccion a
      case 2:
        dir_a();
        break;
      // avanza por un tiempo definido de 4 segundos en direccion b
      case 3:
        dir_b();
        break;
      // avanza en direccion a
      case 4:
        dir_a();
        memo1 = true;
        break;
      // avanza en direccion b
      case 5:
        dir_b();
        memo2 = true;
        break;
      // da vueltas hasta encontrar el robot
      case 6:
        giro();
        break;
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
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
