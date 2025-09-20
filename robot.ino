#include <Adafruit_TCS34725.h>
#include <Arduino.h>
#include <Musica.h>
#include <NewPing.h>
#include <Wire.h>
int banc = 0;
SemaphoreHandle_t alerta;
bool estado = false;
bool start = false;
int modo = 0;
bool memo1 = false;
bool memo2 = false;

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
int scl_2 = 5;
int ena_1 = 33; // quizas no se use
int ena_2 = 32;
int swi = 4;
int cal = 15;
int limCol = 200;
Adafruit_TCS34725 sc_1 =
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 sc_2 =
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int mot[2][2] = {{26, 25}, {14, 27}};
NewPing ojos_1(trig_1, echo_1, maxd);
NewPing ojos_2(trig_2, echo_2, maxd);
uint16_t lcr = 800, lcg = 700, lcb = 500;

void dir_a(int x) {
  alto(0);
  digitalWrite(mot[0][0], HIGH);
  digitalWrite(mot[1][0], HIGH);
  delay(x);
}
void atras(int x) {
  dir_b(0);
  digitalWrite(mot[1][1], HIGH);
  digitalWrite(mot[0][1], HIGH);
  delay(x);
}
void giro(int x) {
  digitalWrite(mot[1][0], HIGH);
  digitalWrite(mot[0][1], HIGH);
  delay(x);
}
void alto(int x) {
  digitalWrite(mot[0][0], LOW);
  digitalWrite(mot[0][1], LOW);
  digitalWrite(mot[1][0], LOW);
  digitalWrite(mot[1][1], LOW);
  delay(x);
}
void robot(void *pvParameters) {
  while (1) {
    // prende al precionar el boton
    if (digitalRead(ini) == HIGH) {
      start = true;
    }
    // inicia
    while (start) {

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
      // si detecta al robot por ojos 1
      else if (ojos_1.ping_cm != 0) {
        modo = 2;
      }
      // si detecta al robot por ojos 2
      else if (ojos_2.ping_cm != 0) {
        modo = 3;
      }
      // si deja de detectar el robot por ojos 1
      else if (memo1) {
        modo = 4;
      }
      // si deja de detectar el robot por ojos 2
      else if (memo2) {
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
        break;
      // detiene el movimiento y retrocede en direccion a
      case 1:
        break;
      // avanza en direccion a
      case 2:
        break;
      // avanza en direccion b
      case 3:
        break;
      // avanza por un tiempo definido en direccion a
      case 4:
        break;
      // avanza por un tiempo definido en direccion b
      case 5:
        break;
      // da vueltas hasta encontrar el robot
      case 6:
        break;
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  }
}

void senColor(void *pvParameters) {
  while (1) {
    uint16_t r, g, b, c;
    if (!estado) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }
    sc_1.getRawData(&r, &g, &b, &c);
    long difCol = abs(r - lcr) + abs(g - lcg) + abs(b - lcb);
    if (difCol < limCol) {
      xSemaphoreGive(alerta);
    }
    vTaskDelay(20 / portTICK_PERIOD_MS);
    // digitalWrite(19, HIGH);
  }
}
void calCol() {}
void setup() {
  alerta = xSemaphoreCreateBinary();
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

  pinMode(mot[0][0], OUTPUT);
  pinMode(mot[0][1], OUTPUT);
  pinMode(mot[1][0], OUTPUT);
  pinMode(mot[1][1], OUTPUT);
  digitalWrite(led_1, HIGH);

  if (sc_1.begin()) {
    estado = true;
  } else {
    estado = false;
    digitalWrite(23, HIGH);
  }

  if (digitalRead(cal) == LOW) {
    calCol();
  } else {
    lcr = 800;
    lcg = 700;
    lcb = 500;
  }

  xTaskCreatePinnedToCore(robot, "robot", 1024, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(senColor, "sensorColor", 2048, NULL, 1, NULL, 0);
}
void loop() {}
