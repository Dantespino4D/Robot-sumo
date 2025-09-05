#include <Arduino.h>
// #include <Musica.h>
#include <NewPing.h>
int b = 4;
#define TRI_1 6
#define ECHO_1 7
#define MAXD 40
#define TRI_2 8
#define ECHO_2 9
int led = 5;
NewPing ojos_1(TRI_1, ECHO_1, MAXD);
NewPing ojos_2(TRI_2, ECHO_2, MAXD);

void setup() {
  pinMode(ECHO_1, INPUT);
  pinMode(ECHO_2, INPUT);
  pinMode(TRI_1, OUTPUT);
  pinMode(TRI_2, OUTPUT);
  pinMode(b, INPUT);
  pinMode(led, OUTPUT);
}
void loop() {
  if (digitalRead(b) == true) {
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
  }
}
