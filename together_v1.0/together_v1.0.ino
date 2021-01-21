#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>

Encoder myEnc(19, 18);
float Position_KP = 3, Position_KI = 0, Position_KD = 100;
int motor = 6;
int order;
float target;
int direct = 7;
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
float temperature;
char comchar;

void setup() {
  pinMode (motor, OUTPUT);
  pinMode (direct, OUTPUT);
  Serial.begin(9600);
  while (Serial.read() >= 0) {} //clear serialbuffer
  Serial.println("Start Test:");
  cli(); // Désactive l'interruption globale
  //calibrate();
  bitClear (TCCR2A, WGM20); // WGM20 = 0
  bitClear (TCCR2A, WGM21); // WGM21 = 0
  TCCR2B = 0b00000101; // Clock / 128 soit 16 micro-s et WGM22 = 0
  TIMSK2 = 0b00000001; // Interruption locale autorisée par TOIE2
  sei(); // Active l'interruption globale
}
byte varCompteur = 0; // La variable compteur
long nowPosition = -999;

// Routine d'interruption
ISR(TIMER2_OVF_vect) {
  TCNT2 = 128 - 125; // 125 x 8 µS = 1 ms
  if (varCompteur++ > 10) { // 10 * 1 ms = 10 ms (demi-période)
    varCompteur = 0;
    long newPosition = myEnc.read();
    if (newPosition - nowPosition > 10 || newPosition - nowPosition < -10) {
      nowPosition = newPosition;
      Serial.print("nowPosition: "); Serial.println(nowPosition);
    }
    //Serial.print("nowPosition: "); Serial.println(nowPosition);
    turn(nowPosition, target);
  }
}

void loop() {
  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    delay(1000);
    return;
  }

  temperature = baro.getTemperature();
  Serial.print("temperature: "); Serial.println(temperature);
  target = map(temperature, 10, 30, 0, 475);
  Serial.print("target: "); Serial.println(target);
  delay(1000);
}

void turn (int nowPosition, int target)
{
  static float Bias, Pwm, Integral_bias, Last_Bias;
  Bias = target - nowPosition;
  if (Bias > 0) {
    digitalWrite(direct, HIGH);
  } else {
    Bias = -Bias;
    digitalWrite(direct, LOW);
  }
  Integral_bias += Bias;
  Pwm = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);
  Last_Bias = Bias;
  if (Pwm > 50) {
    Pwm = 50;
  }  else if (Pwm < 0) {
    Pwm = 0;
  }
  analogWrite(motor, Pwm);
}

void calibrate() {
  // read data from serial port to calibrate
  while (1) {
    while (Serial.available() > 0) {
      comchar = Serial.read();//读串口第一个字节
      Serial.print("Serial.read: ");
      Serial.println(comchar);
      if (comchar == '-') {
        digitalWrite(direct, LOW);
        analogWrite(motor, 50);
        delay(20);
        analogWrite(motor, 0);
        while (Serial.read() >= 0) {}
      } else if (comchar == '+') {
        digitalWrite(direct, HIGH);
        analogWrite(motor, 50);
        delay(20);
        analogWrite(motor, 0);
        while (Serial.read() >= 0) {}
      } else if (comchar == '1') {
        digitalWrite(direct, HIGH);
        analogWrite(motor, 50);
        delay(100);
        analogWrite(motor, 0);
        while (Serial.read() >= 0) {}
      } else if (comchar == '2') {
        digitalWrite(direct, LOW);
        analogWrite(motor, 50);
        delay(100);
        analogWrite(motor, 0);
        while (Serial.read() >= 0) {}
      } else if (comchar == '=') {
        while (Serial.read() >= 0) {}
        return;
      } else {
        Serial.println("Error!");
        while (Serial.read() >= 0) {}
      }
      delay(100);
    }
  }
}
