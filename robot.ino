#include <Arduino.h>
// #include <Musica.h>
#include <NewPing.h>
int trig_1 = 13;
int echo_1 = 12;
int maxd = 40;
int trig_2 = 14;
int echo_2 = 27;
int led_1 = 26;
int sda_1 = 25;
int scl_1 = 33;
int led_2 = 32;
int sda_2 = 15;
int scl_2 = 2;
int pinClock = 4;
int lanch = 5;
int dato = 18;
// int led = 5;
int mot[4][2];
NewPing ojos_1(trig_1, echo_1, maxd);
NewPing ojos_2(trig_2, echo_2, maxd);
TaskHandle_t robotH = NULL;

void puetH(byte data) {
  digitalWrite(lanch, LOW);

  shiftOut(dato, pinClock, MSBFIRST, data);

  digitalWrite(lanch, HIGH);
}
void robot(void *pvParameters) {
  while (1) {
    digitalWrite(13, HIGH);
  }
}

void senColor(void *pvParameters) {
  while (1) {
    puetH(1);
    digitalWrite(13, HIGH);
  }
}
void setup() {
  pinMode(19, OUTPUT);
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
  pinMode(pinClock, OUTPUT);
  pinMode(lanch, OUTPUT);
  pinMode(dato, OUTPUT);

  xTaskCreatePinnedToCore(robot, "robot", 1024, NULL, 1, &robotH, 1);
  xTaskCreatePinnedToCore(senColor, "sensorColor", 2048, NULL, 1, NULL, 1);
}
void loop() {
  digitalWrite(19, HIGH);
  // digitalWrite
  /*if (digitalRead(b) == true) {
    while (true) {
      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);
      delay(1000);
      if (digitalRead(8) == true) {
        if (ojos_2.ping_cm() <= 40 || ojos_1.ping_cm() <= 40) {
          while (ojos_1.ping_cm() <= 40) {
          }
          while (ojos_2.ping_cm() <= 40) {
          }
        } else {
        }
      }
    }
  }*/
}
