#include <Arduino.h>
// #include <Musica.h>
#include <Adafruit_TCS34725.h>
#include <NewPing.h>
#include <Wire.h>

int ini = 2;
int trig_1 = 13;
int echo_1 = 34;
int maxd = 40;
int trig_2 = 12;
int echo_2 = 35;
int led_1 = 15;
int sda_1 = 21;
int scl_1 = 22;
int led_2 = 5;
int sda_2 = 18;
int scl_2 = 19;
int ena_1 = 33; // quizas no se use
int ena_2 = 32;
// int led = 5;
int mot[2][2] = {{26, 25}, {14, 27}};
NewPing ojos_1(trig_1, echo_1, maxd);
NewPing ojos_2(trig_2, echo_2, maxd);

void adelante(int x) {
  alto(0);
  digitalWrite(mot[0][0], HIGH);
  digitalWrite(mot[1][0], HIGH);
  delay(x);
}
void atras(int x) {
  alto(0);
  digitalWrite(mot[1][1], HIGH);
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
    adelante(2000);
    alto(1000);
    atras(2000);
    alto(1000);
  }
}

void senColor(void *pvParameters) {
  while (1) {

    delay(100);
    // digitalWrite(19, HIGH);
  }
}
void setup() {
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
  pinMode(ini, INPUT);

  pinMode(mot[0][0], OUTPUT);
  pinMode(mot[0][1], OUTPUT);
  pinMode(mot[1][0], OUTPUT);
  pinMode(mot[1][1], OUTPUT);

  xTaskCreatePinnedToCore(robot, "robot", 1024, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(senColor, "sensorColor", 2048, NULL, 1, NULL, 1);
}
void loop() {}
